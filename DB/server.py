from flask import Flask, request, render_template, redirect, url_for, flash, session
import psycopg2
import bcrypt
import os
import subprocess
from config import *
import subprocess
import json

app = Flask(__name__)
app.secret_key = 'your_secret_key'

def restore_database_from_backup(backup_file_path):
    conn = create_connection()
    cursor = conn.cursor()
    with open(backup_file_path, 'r') as f:
        sql_commands = f.read()
    try:
        cursor.executescript(sql_commands)
        conn.commit()
        flash("База данных успешно восстановлена.", "success")
    except Exception as e:
        conn.rollback()
        flash(f"Ошибка при восстановлении базы данных: {e}", "error")
    finally:
        cursor.close()
        conn.close()
@app.route('/restore-database', methods=['POST'])
def restore_database():
    if request.method == "POST":
        backup_file_path = request.form['backup_file']
        if not os.path.exists(backup_file_path):
            flash("Файл резервной копии не найден.", "error")
            return redirect(url_for("dashboard"))
        restore_database_from_backup(backup_file_path)
        return redirect(url_for("dashboard"))
    else:
        flash("Невозможно восстановить базу данных.", "error")
        return redirect(url_for("dashboard"))
def create_backup(container_name, database_name, username, password, output_file):
    command = f"docker exec -i {container_name} sh -c \"PGPASSWORD={password} psql -U {username} -d {database_name} -f /tmp/{output_file}\""
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()
    if process.returncode != 0:
        raise Exception(f"Ошибка при создании резервной копии: {error.decode('utf-8')}")
    print(f"Резервная копия создана: {output_file}")
def restore_backup(container_name, database_name, username, password, input_file):
    copy_command = f"docker cp {input_file} {container_name}:/tmp/{input_file}"
    subprocess.run(copy_command, shell=True, check=True)
    command = f"docker exec -i {container_name} sh -c \"PGPASSWORD={password} psql -U {username} -d {database_name} -f /tmp/{input_file}\""
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()
    if process.returncode != 0:
        raise Exception(f"Ошибка при восстановлении базы данных: {error.decode('utf-8')}")
    print(f"База данных восстановлена из {input_file}")



# subprocess.run(['docker', 'stop', 'my-postgres'])
# subprocess.run(['docker', 'rm', 'my-postgres'])
# subprocess.run(['docker', 'run', '--name', 'my-postgres', '-p', '{DB_PORT}:{5432}', '-e', 'POSTGRES_USER={DB_USER}', '-e', 'POSTGPG_PASSWORD={DB_PASS}', '-e', 'POSTGRES_DB={DB_NAME}', '-d', 'postgres:latest'])
# subprocess.run(['sudo', 'docker', 'cp', '../KP', 'my-postgres:/KP'])
# subprocess.run(['docker', 'exec', '-it', 'my-postgres', 'psql', '-U', '{DB_USER}', '-d', '{DB_NAME}', '-f', '/KP/test.sql'])
subprocess.run(['psql', '-U', DB_USER, '-e', DB_PASS, '-f', 'backup_file.sql'])

def create_connection():
    """Создает соединение с базой данных."""
    return psycopg2.connect(host=DB_HOST, port=DB_PORT, database=DB_NAME, user=DB_USER, password=DB_PASS)

def create_user(username, password):
    """Создает нового пользователя с хешированным паролем."""
    hashed_password = bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt()).decode('utf-8')
    
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Users WHERE username = %s", (username,))
    # existing_user = cursor.fetchone()
    cursor.callproc("get_user_by_username", (username,))
    existing_user = cursor.fetchall()
    
    if existing_user:
        flash("Ошибка: пользователь с таким именем уже существует.", "error")
    else:
        try:
            # cursor.execute("INSERT INTO Users (username, password_hash) VALUES (%s, %s)", (username, hashed_password))
            cursor.callproc('insert_user', (username, hashed_password))
            conn.commit()
            flash("Пользователь успешно создан.", "success")
        except Exception as e:
            conn.rollback()
            flash(f"Ошибка при создании пользователя: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

def authenticate(username, password):
    """Аутентифицирует пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT id, password_hash FROM users WHERE username = %s", (username,))
    # user = cursor.fetchone()
    cursor.callproc('get_user_info', (username,))
    user = cursor.fetchall()

    if user:
        user_id, password_hash = user[0]
        
        if bcrypt.checkpw(password.encode('utf-8'), password_hash.encode('utf-8')):
            session['user_id'] = user_id
            return user_id
    
    return False

def get_user_by_id(user_id):
    """Получает информацию о пользователе по его идентификатору."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT username FROM Users WHERE id = %s", (user_id,))
    # user = cursor.fetchone()

    cursor.callproc('get_username_by_id', (user_id,))
    user = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return user


def get_satellites(user_id):
    """Получает список спутников для указанного пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Satellites WHERE user_id = %s", (user_id,))
    # satellites = cursor.fetchone()

    cursor.callproc('get_satellites_by_user_id', (user_id,))
    satellites = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return satellites


def get_models(user_id):
    """Получает список моделей спутников для указанного пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Models WHERE user_id = %s", (user_id,))
    # models = cursor.fetchall()
    cursor.callproc('get_models_by_user_id', (user_id,))
    models = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return models


def get_characteristics(user_id):
    """Получает список характеристик спутников для указанного пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Characteristics WHERE user_id = %s", (user_id,))
    # characteristics = cursor.fetchall()

    cursor.callproc('get_characteristics_by_user_id', (user_id,))
    characteristics = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return characteristics


def get_trajectories(user_id):
    """Получает список траекторий спутников для указанного пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Trajectories WHERE user_id = %s", (user_id,))
    # trajectories = cursor.fetchall()
    
    cursor.callproc('get_trajectories_by_user_id', (user_id,))
    trajectories = cursor.fetchall()

    cursor.close()
    conn.close()
    
    return trajectories


def get_reports(user_id):
    """Получает список отчетов о спутниках для указанного пользователя."""
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Reports WHERE user_id = %s", (user_id,))
    # reports = cursor.fetchall()

    cursor.callproc('get_reports_by_user_id', (user_id,))
    reports = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return reports

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        create_user(username, password)
        return redirect(url_for('register'))
    return render_template('register.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        user_id = authenticate(username, password)
        if authenticate(username, password):
            return redirect(url_for('dashboard'))
        else:
            flash("Ошибка: неверное имя пользователя или пароль.", "error")
    return render_template('login.html')

@app.route('/logout')
def logout():
    session.pop('user_id', None)
    return redirect(url_for('index'))

@app.route('/dashboard', methods=['GET'])
def dashboard():
    user_id = session.get('user_id')
    if user_id:
        user = get_user_by_id(user_id)
        satellites = get_satellites(user_id)
        models = get_models(user_id)
        characteristics = get_characteristics(user_id)
        trajectories = get_trajectories(user_id)
        reports = get_reports(user_id)
        return render_template('dashboard.html', user=user, satellites=satellites, models=models, characteristics=characteristics, trajectories=trajectories, reports=reports)
    else:
        flash("Пожалуйста, войдите в систему.", "info")
        return redirect(url_for('login'))

def add_new_satellite(name, model_id):
    """Добавляет новый спутник в базу данных."""
    conn = create_connection()
    cursor = conn.cursor()

    # cursor.execute("SELECT * FROM Satellites WHERE name = %s AND user_id = %s", (name, session.get('user_id')))
    # existing_satellite = cursor.fetchone()

    cursor.callproc('get_satellite_by_name_and_user_id', (name, session.get('user_id')))
    existing_satellite = cursor.fetchall()

    if existing_satellite:
        flash("Ошибка: спутник с таким именем уже существует.", "error")
    else:
        try:
            # cursor.execute(
            #     "INSERT INTO Satellites (user_id, name, model_id) VALUES (%s, %s, %s)",
            #     (session.get('user_id'), name, model_id)
            # )
            cursor.callproc('insert_satellite', (session.get('user_id'), name, model_id))
            conn.commit()
            flash("Спутник успешно добавлен.", "success")
        except Exception as e:
            conn.rollback()
            flash(f"Ошибка при добавлении спутника: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route("/add-satellite", methods=["GET"])
def add_satellite_get():
    models = get_models(session.get('user_id'))
    return render_template("add-satellite.html", models=models)

@app.route("/add-satellite", methods=["POST"])
def add_satellite():
    if request.method == "POST":
        form = request.form
        name = form["name"]
        model = form["model"]
        add_new_satellite(name, model)

        # flash("Спутник добавлен.")
        return redirect(url_for("dashboard"))
    else:
        flash("Ошибка при добавлении спутника.")
        return redirect(url_for("dashboard"))

def add_new_model(model_name, manufacturer, launch_date, description):
    conn = create_connection()
    cursor = conn.cursor()
    
    cursor.callproc('get_model_by_name_and_user', (model_name, session.get('user_id'),))
    existing_model = cursor.fetchall()
    
    if existing_model:
        flash("Ошибка: Модель с таким именем уже существует.", "error")
    else:
        try:
            # cursor.callproc("insert_into_models", (session.get('user_id'), model_name, manufacturer, launch_date, description))
            cursor.callproc("insert_into_models", (session.get('user_id'), model_name, manufacturer, launch_date, description))
            conn.commit()
            flash("Модель успешно добавлена.", "success")
        except Exception as e:
            conn.rollback()
            flash(f"Ошибка при добавлении модели: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route("/add-model", methods=["GET"])
def add_model_get():
    models = get_models(session.get('user_id'))
    return render_template("add-model.html", models=models)

@app.route("/add-model", methods=["POST"])
def add_model_post():
    if request.method == "POST":
        form = request.form
        model_name = form["model_name"]
        manufacturer = form["manufacturer"]
        launch_date = form["launch_date"]
        description = form["description"]
        
        add_new_model(model_name, manufacturer, launch_date, description)
        # flash("Модель добавлена.")
        return redirect(url_for("dashboard"))
    else:
        flash("Ошибка при добавлении модели.")
        return redirect(url_for("dashboard"))

def add_new_characteristic(satellite_id, characteristic_type, value, unit):
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("SELECT * FROM Characteristics WHERE satellite_id = %s AND characteristic_type = %s AND user_id = %s", (satellite_id, characteristic_type, session.get('user_id')))
    # existing_characteristic = cursor.fetchone()

    cursor.callproc("get_characteristics_by_satellite_and_type", (satellite_id, characteristic_type, session.get('user_id')))
    existing_characteristic = cursor.fetchall()
    
    if existing_characteristic:
        flash("Ошибка: Характеристика с такими параметрами уже существует.", "error")
    else:
        try:
            # cursor.execute(
            #     """
            #     INSERT INTO Characteristics (user_id, satellite_id, characteristic_type, value, unit)
            #     VALUES (%s, %s, %s, %s, %s)
            #     """,
            #     (session.get('user_id'), satellite_id, characteristic_type, value, unit)
            # )
            cursor.callproc("insert_into_characteristics", (session.get('user_id'), satellite_id, characteristic_type, value, unit))
            conn.commit() 
            flash("Характеристика успешно добавлена.", "success")
        except Exception as e:
            conn.rollback() 
            flash(f"Ошибка при добавлении характеристики: {e}", "error")
    
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route("/add-characteristic", methods=["GET"])
def add_characteristic_get():
    satellites = get_satellites(session.get('user_id')) 
    return render_template("add-characteristic.html", satellites=satellites)

@app.route("/add-characteristic", methods=["POST"])
def add_characteristic_post():
    if request.method == "POST":
        form = request.form
        satellite_id = form["satellite_id"]
        characteristic_type = form["characteristic_type"]
        value = float(form["value"])
        unit = form["unit"]
        
        add_new_characteristic(satellite_id, characteristic_type, value, unit)
        # flash("Характеристика добавлена.")
        return redirect(url_for("dashboard")) 
    else:
        flash("Ошибка при добавлении характеристики.")
        return redirect(url_for("dashboard"))

def add_new_trajectory(satellite_id, trajectory_data, start_time, end_time):
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("""
    #     SELECT *
    #     FROM Trajectories
    #     WHERE satellite_id = %s AND start_time = %s AND end_time = %s AND user_id = %s
    # """, (satellite_id, start_time, end_time, session.get('user_id')))
    # existing_trajectory = cursor.fetchone()

    cursor.callproc('get_trajectory_by_params', (satellite_id, start_time, end_time, session.get('user_id')))
    existing_trajectory = cursor.fetchone()
    
    if existing_trajectory:
        flash("Ошибка: Траектория с такими параметрами уже существует.", "error")
    else:
        try:
            # cursor.execute(
            #     """
            #     INSERT INTO Trajectories (user_id, satellite_id, trajectory_data, start_time, end_time)
            #     VALUES (%s, %s, %s, %s, %s)
            #     """,
            #     (session.get('user_id'), satellite_id, json.dumps(trajectory_data), start_time, end_time)
            # )
            cursor.callproc('insert_trajectory', (session.get('user_id'), satellite_id, json.dumps(trajectory_data), start_time, end_time))
            conn.commit() 
            flash("Траектория успешно добавлена.", "success")
        except Exception as e:
            conn.rollback()
            flash(f"Ошибка при добавлении траектории: {e}", "error")
    
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route("/add-trajectorie", methods=["GET"])
def add_trajectory_get():
    satellites = get_satellites(session.get('user_id')) 
    return render_template("add-trajectorie.html", satellites=satellites)

@app.route("/add-trajectorie", methods=["POST"])
def add_trajectory_post():
    if request.method == "POST":
        form = request.form
        satellite_id = form["satellite_id"]
        try:
            trajectory_data = json.loads(form['trajectory_data']) 
        except:
            flash("Произошла ошибка при добавлении траектории.", category='error')
            return redirect(url_for('dashboard'))
        start_time = form["start_time"]
        end_time = form["end_time"]
        
        add_new_trajectory(satellite_id, trajectory_data, start_time, end_time)
        # flash("Траектория добавлена.")
        return redirect(url_for("dashboard"))  
    else:
        flash("Ошибка при добавлении траектории.")
        return redirect(url_for("dashboard"))


# {
#     "points": [
#         {
#             "latitude": 55.755826,
#             "longitude": 37.6172999,
#             "altitude": 400000,
#             "timestamp": "2023-05-01T12:00:00Z"
#         },
#         {
#             "latitude": 56.755826,
#             "longitude": 38.6172999,
#             "altitude": 410000,
#             "timestamp": "2023-05-01T13:00:00Z"
#         },
#         {
#             "latitude": 57.755826,
#             "longitude": 39.6172999,
#             "altitude": 420000,
#             "timestamp": "2023-05-01T14:00:00Z"
#         }
#     ]
# }

def add_new_report(satellite_id, report_date, status, details):
    conn = create_connection()
    cursor = conn.cursor()
    
    # cursor.execute("""
    #     SELECT *
    #     FROM Reports
    #     WHERE satellite_id = %s AND report_date = %s AND user_id = %s
    # """, (satellite_id, report_date, session.get('user_id')))
    # existing_report = cursor.fetchone()

    cursor.callproc('get_report_by_params', (satellite_id, report_date, session.get('user_id')))
    existing_report = cursor.fetchone()
    
    if existing_report:
        flash("Ошибка: Отчет с такими параметрами уже существует.", "error")
    else:
        try:
            # cursor.execute(
            #     """
            #     INSERT INTO Reports (user_id, satellite_id, report_date, status, details)
            #     VALUES (%s, %s, %s, %s, %s)
            #     """,
            #     (session.get('user_id'), satellite_id, report_date, status, details)
            # )
            cursor.callproc('insert_report', (session.get('user_id'), satellite_id, report_date, status, details))
            conn.commit() 
            flash("Отчет успешно добавлен.", "success")
        except Exception as e:
            conn.rollback()
            flash(f"Ошибка при добавлении отчета: {e}", "error")
    
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route("/add-report", methods=["GET"])
def add_report_get():
    satellites = get_satellites(session.get('user_id'))  
    return render_template("add-report.html", satellites=satellites)

@app.route("/add-report", methods=["POST"])
def add_report_post():
    if request.method == "POST":
        form = request.form
        satellite_id = form["satellite_id"]
        report_date = form["report_date"]
        status = form["status"]
        details = form["details"]
        
        add_new_report(satellite_id, report_date, status, details)
        # flash("Отчет добавлен.")
        return redirect(url_for("dashboard")) 
    else:
        flash("Ошибка при добавлении отчета.")
        return redirect(url_for("dashboard"))

def update_model(model_id, model_name, manufacturer, launch_date, description):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute("SELECT * FROM Models WHERE id = %s", (model_id,))
        # model = cursor.fetchone()
        cursor.callproc('get_model_by_id', (model_id,))
        model = cursor.fetchone()
        
        if model:
            # cursor.execute(
            #     """
            #     UPDATE Models
            #     SET model_name = %s, manufacturer = %s, launch_date = %s, description = %s
            #     WHERE id = %s
            #     """,
            #     (model_name, manufacturer, launch_date, description, model_id)
            # )
            cursor.callproc('update_model', (model_id, model_name, manufacturer, launch_date, description))
            conn.commit() 
            subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
            cursor.close()
            conn.close()
            
            flash("Модель обновлена.")
            return redirect(url_for('dashboard'))
        else:
            flash("Модель не найдена.")
            return redirect(url_for('dashboard'))
    except Exception as e:
        conn.rollback() 
        flash(f"Ошибка при обновлении модели: {e}", "error")
        subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
        cursor.close()
        conn.close()
        
        flash("Ошибка при обновлении модели.")
        return redirect(url_for('dashboard'))

@app.route('/edit-model/<int:model_id>', methods=['GET'])
def edit_model_get(model_id):
    model = get_models(session.get('user_id'))  
    element_to_edit = None
    for m in model:
        if m[0] == model_id:
            element_to_edit = m
            break
    if element_to_edit:
        model.remove(element_to_edit) 
        model.insert(0, element_to_edit) 
    return render_template("edit-model.html", models=model, model_id=model_id)

@app.route('/edit-model/<int:model_id>', methods=['POST'])
def edit_model_post(model_id):
    if request.method == 'POST':
        form = request.form
        model_name = form['model_name']
        manufacturer = form['manufacturer']
        launch_date = form['launch_date']
        description = form['description']
        update_model(model_id, model_name, manufacturer, launch_date, description)
        flash("Модель изменена.")
        return redirect(url_for('dashboard'))
    else:
        flash("Ошибка при изменении модели.")
        return redirect(url_for('dashboard'))

def update_satellite(satellite_id, name, model_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute("SELECT * FROM Satellites WHERE id = %s", (satellite_id,))
        # satellite = cursor.fetchone()
        cursor.callproc('get_satellite_by_id', (satellite_id,))
        satellite = cursor.fetchone()
        
        if satellite:
            # cursor.execute(
            #     "UPDATE Satellites SET name = %s, model_id = %s WHERE id = %s",
            #     (name, model_id, satellite_id)
            # )
            cursor.callproc('update_satellite', (satellite_id, name, model_id))
            conn.commit()  
            subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
            cursor.close()
            conn.close()
            
            flash("Спутник обновлён.")
            return redirect(url_for('dashboard'))
        else:
            flash("Спутник не найден.")
            return redirect(url_for('dashboard'))
    except Exception as e:
        conn.rollback() 
        flash(f"Ошибка при обновлении спутника: {e}", "error")
        subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
        cursor.close()
        conn.close()
        
        flash("Ошибка при обновлении спутника.")
        return redirect(url_for('dashboard'))

@app.route('/edit-satellite/<int:satellite_id>', methods=['GET'])
def edit_satellite_get(satellite_id):
    satellite = get_satellites(session.get('user_id'))  
    models = get_models(session.get('user_id'))
    element_to_move = None
    for model in models:
        if model[0] == satellite_id:
            element_to_move = model
            break
    if element_to_move:
        models.remove(element_to_move)  
        models.insert(0, element_to_move)  
    return render_template("edit-satellite.html", models=models, satellite=satellite, satellite_id= satellite_id,)

@app.route('/edit-satellite/<int:satellite_id>', methods=['POST'])
def edit_satellite_post(satellite_id):
    print("asasasassas")
    if request.method == "POST":
        form = request.form
        name = form["name"]
        model = form["model"]
        update_satellite(satellite_id, name, model)
        flash("Спутник изменен.")
        return redirect(url_for("dashboard"))
    else:
        flash("Ошибка при изменении спутника.")
        return redirect(url_for("dashboard"))

def update_characteristic(characteristic_id, characteristic_type, value, unit):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute("SELECT * FROM Characteristics WHERE id = %s", (characteristic_id,))
        # characteristic = cursor.fetchone()
        cursor.callproc('get_characteristic_by_id', (characteristic_id,))
        characteristic = cursor.fetchone()
        
        if characteristic:
            # cursor.execute(
            #     """
            #     UPDATE Characteristics
            #     SET characteristic_type = %s, value = %s, unit = %s
            #     WHERE id = %s
            #     """,
            #     (characteristic_type, value, unit, characteristic_id)
            # )
            cursor.callproc('update_characteristic', (characteristic_id, characteristic_type, value, unit))
            conn.commit() 
            subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
            cursor.close()
            conn.close()
            
            flash("Характеристика обновлена.")
            return redirect(url_for('dashboard'))
        else:
            flash("Характеристика не найдена.")
            return redirect(url_for('dashboard'))
    except Exception as e:
        conn.rollback()  
        flash(f"Ошибка при обновлении характеристики: {e}", "error")
        subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
        cursor.close()
        conn.close()
        
        # flash("Ошибка при обновлении характеристики.")
        return redirect(url_for('dashboard'))

@app.route('/edit-characteristic/<int:characteristic_id>', methods=['GET'])
def edit_characteristic_get(characteristic_id):
    characteristics = get_characteristics(session.get('user_id')) 
    element_to_edit = None
    for char in characteristics:
        if char[0] == characteristic_id:
            element_to_edit = char
            break
    if element_to_edit:
        characteristics.remove(element_to_edit) 
        characteristics.insert(0, element_to_edit) 
    return render_template("edit-characteristic.html", characteristics=characteristics, characteristic_id=characteristic_id)

@app.route('/edit-characteristic/<int:characteristic_id>', methods=['POST'])
def edit_characteristic_post(characteristic_id):
    if request.method == 'POST':
        form = request.form
        characteristic_type = form['characteristic_type']
        value = form['value']
        unit = form['unit']
        update_characteristic(characteristic_id, characteristic_type, value, unit)
        # flash("Характеристика изменена.")
        return redirect(url_for('dashboard'))
    else:
        flash("Ошибка при изменении характеристики.")
        return redirect(url_for('dashboard'))

def update_trajectory(trajectory_id, trajectory_data, start_time, end_time):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute("SELECT * FROM Trajectories WHERE id = %s", (trajectory_id,))
        # trajectory = cursor.fetchone()
        cursor.callproc('get_trajectory_by_id', (trajectory_id,))
        trajectory = cursor.fetchone()
        
        if trajectory:
            # cursor.execute(
            #     """
            #     UPDATE Trajectories
            #     SET trajectory_data = %s, start_time = %s, end_time = %s
            #     WHERE id = %s
            #     """,
            #     (json.dumps(trajectory_data), start_time, end_time, trajectory_id)
            # )
            cursor.callproc('update_trajectory', (trajectory_id, json.dumps(trajectory_data), start_time, end_time))
            conn.commit() 
            subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
            cursor.close()
            conn.close()
            
            flash("Траектория обновлена.")
            return redirect(url_for('dashboard'))
        else:
            flash("Траектория не найдена.")
            return redirect(url_for('dashboard'))
    except Exception as e:
        conn.rollback()  
        flash(f"Ошибка при обновлении траектории: {e}", "error")
        subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
        cursor.close()
        conn.close()
        
        flash("Ошибка при обновлении траектории.")
        return redirect(url_for('dashboard'))

@app.route('/edit-trajectory/<int:trajectory_id>', methods=['GET'])
def edit_trajectory_get(trajectory_id):
    trajectories = get_trajectories(session.get('user_id')) 
    element_to_edit = None
    for traj in trajectories:
        if traj[0] == trajectory_id:
            element_to_edit = traj
            break

    if element_to_edit:
        trajectories.remove(element_to_edit)
        trajectories.insert(0, element_to_edit) 
    return render_template("edit-trajectory.html", trajectories=trajectories, trajectory_id=trajectory_id)

@app.route('/edit-trajectory/<int:trajectory_id>', methods=['POST'])
def edit_trajectory_post(trajectory_id):
    if request.method == 'POST':
        form = request.form
        try:
            trajectory_data = json.loads(form['trajectory_data']) 
        except:
            flash("Произошла ошибка при изменении траектории.", category='error')
            return redirect(url_for('dashboard'))
        start_time = form['start_time']
        end_time = form['end_time']
        update_trajectory(trajectory_id, trajectory_data, start_time, end_time)
        flash("Траектория изменена.")
        return redirect(url_for('dashboard'))
    else:
        flash("Ошибка при изменении траектории.")
        return redirect(url_for('dashboard'))

def update_report(report_id, report_date, status, details):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute("SELECT * FROM Reports WHERE id = %s", (report_id,))
        # report = cursor.fetchone()
        cursor.callproc('get_report_by_id', (report_id,))
        report = cursor.fetchone()
        
        if report:
            # cursor.execute(
            #     """
            #     UPDATE Reports
            #     SET report_date = %s, status = %s, details = %s
            #     WHERE id = %s
            #     """,
            #     (report_date, status, details, report_id)
            # )
            cursor.callproc('update_report', (report_id, report_date, status, details))
            conn.commit()  
            subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
            cursor.close()
            conn.close()
            
            flash("Отчет обновлен.")
            return redirect(url_for('dashboard'))
        else:
            flash("Отчет не найден.")
            return redirect(url_for('dashboard'))
    except Exception as e:
        conn.rollback()  
        flash(f"Ошибка при обновлении отчета: {e}", "error")
        subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
        cursor.close()
        conn.close()
        
        flash("Ошибка при обновлении отчета.")
        return redirect(url_for('dashboard'))

@app.route('/edit-report/<int:report_id>', methods=['GET'])
def edit_report_get(report_id):
    reports = get_reports(session.get('user_id'))  
    element_to_edit = None
    for rep in reports:
        if rep[0] == report_id:
            element_to_edit = rep
            break
    if element_to_edit:
        reports.remove(element_to_edit)  
        reports.insert(0, element_to_edit)  
    return render_template("edit-report.html", reports=reports, report_id=report_id)

@app.route('/edit-report/<int:report_id>', methods=['POST'])
def edit_report_post(report_id):
    if request.method == 'POST':
        form = request.form
        report_date = form['report_date']
        status = form['status']
        details = form['details']
        update_report(report_id, report_date, status, details)
        flash("Отчет изменен.")
        return redirect(url_for('dashboard'))
    else:
        flash("Ошибка при изменении отчета.")
        return redirect(url_for('dashboard'))

@app.route('/delete-satellite/<int:satellite_id>', methods=['POST'])
def delete_satellite(satellite_id):
    delete_satellite_from_db(satellite_id)
    return redirect(url_for('dashboard'))

def delete_satellite_from_db(satellite_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute(
        #     "DELETE FROM Satellites WHERE id = %s AND user_id = %s",
        #     (satellite_id, session.get('user_id'))
        # )
        cursor.callproc('delete_satellite', (satellite_id, session.get('user_id')))
        conn.commit()  
    except Exception as e:
        conn.rollback() 
        flash(f"Ошибка при удалении спутника: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route('/delete-model/<int:model_id>', methods=['POST'])
def delete_model(model_id):
    delete_model_from_db(model_id)
    return redirect(url_for('dashboard'))

def delete_model_from_db(model_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute(
        #     "DELETE FROM Models WHERE id = %s AND user_id = %s",
        #     (model_id, session.get('user_id'))
        # )
        cursor.callproc('delete_model', (model_id, session.get('user_id')))
        conn.commit()  
    except Exception as e:
        conn.rollback()
        flash(f"Ошибка при удалении модели: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route('/delete-characteristic/<int:characteristic_id>', methods=['POST'])
def delete_characteristic(characteristic_id):
    delete_characteristic_from_db(characteristic_id)
    return redirect(url_for('dashboard'))

def delete_characteristic_from_db(characteristic_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute(
        #     "DELETE FROM Characteristics WHERE id = %s AND user_id = %s",
        #     (characteristic_id, session.get('user_id'))
        # )
        cursor.callproc('delete_characteristic', (characteristic_id, session.get('user_id')))
        conn.commit()  
    except Exception as e:
        conn.rollback() 
        flash(f"Ошибка при удалении характеристики: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql']) 
    cursor.close()
    conn.close()

@app.route('/delete-trajectory/<int:trajectory_id>', methods=['POST'])
def delete_trajectory(trajectory_id):
    delete_trajectory_from_db(trajectory_id)
    return redirect(url_for('dashboard'))

def delete_trajectory_from_db(trajectory_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute(
        #     "DELETE FROM Trajectories WHERE id = %s AND user_id = %s",
        #     (trajectory_id, session.get('user_id'))
        # )
        cursor.callproc('delete_trajectory', (trajectory_id, session.get('user_id')))
        conn.commit() 
    except Exception as e:
        conn.rollback() 
        flash(f"Ошибка при удалении траектории: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

@app.route('/delete-report/<int:report_id>', methods=['POST'])
def delete_report_route(report_id):
    delete_report(report_id)
    return redirect(url_for('dashboard'))

def delete_report(report_id):
    conn = create_connection()
    cursor = conn.cursor()
    
    try:
        # cursor.execute(
        #     "DELETE FROM Reports WHERE id = %s AND user_id = %s",
        #     (report_id, session.get('user_id'))
        # )
        cursor.callproc('delete_report', (report_id, session.get('user_id')))
        conn.commit()  
        flash("Отчет удалён.", "info")
    except Exception as e:
        conn.rollback()  
        flash(f"Ошибка при удалении отчета: {e}", "error")
    subprocess.call(['pg_dump', '-U', DB_USER, '-W', DB_PASS, '-F', 'backup_file.sql'])
    cursor.close()
    conn.close()

if __name__ == '__main__':
    app.run(debug=True)