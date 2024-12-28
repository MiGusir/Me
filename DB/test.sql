CREATE TABLE IF NOT EXISTS Users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS Models (
    id SERIAL PRIMARY KEY,
    user_id INT,
    model_name VARCHAR(100),
    manufacturer VARCHAR(200),
    launch_date DATE,
    description TEXT,
    FOREIGN KEY (user_id) REFERENCES Users(id)  ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Satellites (
    id SERIAL PRIMARY KEY,
    user_id INT,
    name VARCHAR(255),
    model_id INT,
    FOREIGN KEY (user_id) REFERENCES Users(id)  ON DELETE CASCADE,
    FOREIGN KEY (model_id) REFERENCES Models(id)  ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Characteristics (
    id SERIAL PRIMARY KEY,
    user_id INT,
    satellite_id INT,
    characteristic_type VARCHAR(50),
    value FLOAT,
    unit VARCHAR(20),
    FOREIGN KEY (user_id) REFERENCES Users(id)  ON DELETE CASCADE,
    FOREIGN KEY (satellite_id) REFERENCES Satellites(id)  ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Trajectories (
    id SERIAL PRIMARY KEY,
    user_id INT,
    satellite_id INT,
    trajectory_data JSON,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES Users(id)  ON DELETE CASCADE,
    FOREIGN KEY (satellite_id) REFERENCES Satellites(id)  ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Reports (
    id SERIAL PRIMARY KEY,
    user_id INT,
    satellite_id INT,
    report_date DATE,
    status VARCHAR(30),
    details TEXT,
    FOREIGN KEY (user_id) REFERENCES Users(id)  ON DELETE CASCADE,
    FOREIGN KEY (satellite_id) REFERENCES Satellites(id)  ON DELETE CASCADE
);

ALTER TABLE Reports ADD CONSTRAINT chk_status CHECK (status IN ('OK', 'FAIL', 'IN_PROGRESS'));

CREATE OR REPLACE FUNCTION get_user_by_username(p_username VARCHAR(50))
RETURNS TABLE (
    id INT,
    username VARCHAR(50),
    password_hash VARCHAR(255),
    created_at TIMESTAMP
) AS

$$
BEGIN
    RETURN QUERY
    SELECT u.id, u.username, u.password_hash, u.created_at
    FROM users u
    WHERE u.username = p_username;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_user(
    p_username VARCHAR(50), 
    p_password_hash VARCHAR(255)
) RETURNS VOID AS

$$
BEGIN
    INSERT INTO users(username, password_hash)
    VALUES (p_username, p_password_hash);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_user_info(p_username VARCHAR(50))
RETURNS TABLE (
    id INT,
    password_hash VARCHAR(255)
) AS

$$
BEGIN
    RETURN QUERY
    SELECT u.id, u.password_hash
    FROM users u
    WHERE u.username = p_username;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_username_by_id(p_user_id INT)
RETURNS TABLE(username VARCHAR(50)) AS

$$
BEGIN
    RETURN QUERY
        SELECT u.username
        FROM Users u
        WHERE u.id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_satellites_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, name VARCHAR(255), model_id INT) AS

$$
BEGIN
    RETURN QUERY
        SELECT s.id, s.user_id, s.name, s.model_id
        FROM Satellites s
        WHERE s.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_models_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, model_name VARCHAR(100), manufacturer VARCHAR(200), launch_date DATE, description TEXT) AS

$$
BEGIN
    RETURN QUERY
        SELECT m.id, m.user_id, m.model_name, m.manufacturer, m.launch_date, m.description
        FROM Models m
        WHERE m.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_characteristics_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, satellite_id INT, characteristic_type VARCHAR(50), value FLOAT, unit VARCHAR(20)) AS

$$
BEGIN
    RETURN QUERY
        SELECT c.id, c.user_id, c.satellite_id, c.characteristic_type, c.value, c.unit
        FROM Characteristics c
        WHERE c.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_characteristics_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, satellite_id INT, characteristic_type VARCHAR(50), value FLOAT, unit VARCHAR(20)) AS

$$
BEGIN
    RETURN QUERY
        SELECT c.id, c.user_id, c.satellite_id, c.characteristic_type, c.value, c.unit
        FROM Characteristics c
        WHERE c.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_trajectories_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, satellite_id INT, trajectory_data JSON, start_time TIMESTAMP, end_time TIMESTAMP) AS

$$
BEGIN
    RETURN QUERY
        SELECT t.id, t.user_id, t.satellite_id, t.trajectory_data, t.start_time, t.end_time
        FROM Trajectories t
        WHERE t.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_reports_by_user_id(p_user_id INT)
RETURNS TABLE(id INT, user_id INT, satellite_id INT, report_date DATE, status VARCHAR(30), details TEXT) AS

$$
BEGIN
    RETURN QUERY
        SELECT r.id, r.user_id, r.satellite_id, r.report_date, r.status, r.details
        FROM Reports r
        WHERE r.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_satellite_by_name_and_user_id(
    p_name VARCHAR(255),
    p_user_id INT
)
RETURNS TABLE(id INT, user_id INT, name VARCHAR(255), model_id INT) AS

$$
BEGIN
    RETURN QUERY
        SELECT s.id, s.user_id, s.name, s.model_id
        FROM Satellites s
        WHERE s.name = p_name AND s.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_satellite(
    p_user_id INT,
    p_name VARCHAR(255),
    p_model_id INT
)
RETURNS VOID
AS

$$
BEGIN
    INSERT INTO Satellites (user_id, name, model_id)
    VALUES (p_user_id, p_name, p_model_id);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_model_by_name_and_user(p_model_name VARCHAR(100), p_user_id INT)
RETURNS TABLE (
    result_id INT,
    result_manufacturer VARCHAR(200),
    result_launch_date DATE,
    result_description TEXT
) AS

$$
BEGIN
    RETURN QUERY
    SELECT 
        m.id as result_id,
        m.manufacturer as result_manufacturer,
        m.launch_date as result_launch_date,
        m.description as result_description
    FROM Models m
    WHERE m.model_name = p_model_name
      AND m.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_into_models(
    p_user_id INT,
    p_model_name VARCHAR(100),
    p_manufacturer VARCHAR(200),
    p_launch_date DATE,
    p_description TEXT
) RETURNS VOID AS

$$
BEGIN
    INSERT INTO Models (user_id, model_name, manufacturer, launch_date, description)
    VALUES (p_user_id, p_model_name, p_manufacturer, p_launch_date, p_description);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_model_by_name_and_user(
    p_model_name VARCHAR(100),
    p_user_id INT
)
RETURNS TABLE(result_id INT, result_manufacturer VARCHAR(200), result_launch_date DATE, result_description TEXT)
AS

$$
BEGIN
    RETURN QUERY
        SELECT 
            m.id as result_id,
            m.manufacturer as result_manufacturer,
            m.launch_date as result_launch_date,
            m.description as result_description
        FROM Models m
        WHERE m.model_name = p_model_name
          AND m.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_into_models(
    p_user_id INT,
    p_model_name VARCHAR(100),
    p_manufacturer VARCHAR(200),
    p_launch_date DATE,
    p_description TEXT
)
RETURNS VOID
AS

$$
BEGIN
    INSERT INTO Models (user_id, model_name, manufacturer, launch_date, description)
    VALUES (p_user_id, p_model_name, p_manufacturer, p_launch_date, p_description);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_characteristics_by_satellite_and_type(
    p_satellite_id INT,
    p_characteristic_type VARCHAR(50),
    p_user_id INT
)
RETURNS TABLE(
    result_id INT,
    result_value FLOAT,
    result_unit VARCHAR(20)
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT 
            c.id AS result_id,
            c.value AS result_value,
            c.unit AS result_unit
        FROM Characteristics c
        WHERE c.satellite_id = p_satellite_id
          AND c.characteristic_type = p_characteristic_type
          AND c.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_into_characteristics(
    p_user_id INT,
    p_satellite_id INT,
    p_characteristic_type VARCHAR(50),
    p_value FLOAT,
    p_unit VARCHAR(20)
)
RETURNS VOID
AS

$$
BEGIN
    INSERT INTO Characteristics (user_id, satellite_id, characteristic_type, value, unit)
    VALUES (p_user_id, p_satellite_id, p_characteristic_type, p_value, p_unit);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_trajectory_by_params(
    p_satellite_id INT,
    p_start_time TIMESTAMP,
    p_end_time TIMESTAMP,
    p_user_id INT
)
RETURNS TABLE(
    result_id INT,
    result_trajectory_data JSON,
    result_start_time TIMESTAMP,
    result_end_time TIMESTAMP
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT t.id, t.trajectory_data, t.start_time, t.end_time
        FROM Trajectories t
        WHERE t.satellite_id = p_satellite_id
          AND t.start_time = p_start_time
          AND t.end_time = p_end_time
          AND t.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_trajectory(
    p_user_id INT,
    p_satellite_id INT,
    p_trajectory_data JSONB,
    p_start_time TIMESTAMP,
    p_end_time TIMESTAMP
)
RETURNS VOID
AS

$$
BEGIN
    INSERT INTO Trajectories (user_id, satellite_id, trajectory_data, start_time, end_time)
    VALUES (p_user_id, p_satellite_id, p_trajectory_data, p_start_time, p_end_time);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_report_by_params(
    p_satellite_id INT,
    p_report_date DATE,
    p_user_id INT
)
RETURNS TABLE(
    result_id INT,
    result_report_date DATE,
    result_status VARCHAR(30),
    result_details TEXT
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT r.id, r.report_date, r.status, r.details
        FROM Reports r
        WHERE r.satellite_id = p_satellite_id
          AND r.report_date = p_report_date
          AND r.user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION insert_report(
    p_user_id INT,
    p_satellite_id INT,
    p_report_date DATE,
    p_status VARCHAR(100),
    p_details TEXT
)
RETURNS VOID
AS

$$
BEGIN
    INSERT INTO Reports (user_id, satellite_id, report_date, status, details)
    VALUES (p_user_id, p_satellite_id, p_report_date, p_status, p_details);
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_model_by_id(
    p_model_id INT
)
RETURNS TABLE(
    result_id INT,
    result_user_id INT,
    result_model_name VARCHAR(100),
    result_manufacturer VARCHAR(200),
    result_launch_date DATE,
    result_description TEXT
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT m.id, m.user_id, m.model_name, m.manufacturer, m.launch_date, m.description
        FROM Models m
        WHERE m.id = p_model_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_model(
    p_model_id INT,
    p_model_name VARCHAR(255),
    p_manufacturer VARCHAR(255),
    p_launch_date DATE,
    p_description TEXT
)
RETURNS VOID
AS

$$
BEGIN
    UPDATE Models
    SET model_name = p_model_name,
        manufacturer = p_manufacturer,
        launch_date = p_launch_date,
        description = p_description
    WHERE id = p_model_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_satellite_by_id(
    p_satellite_id INT
)
RETURNS TABLE(
    result_id INT,
    result_user_id INT,
    result_name VARCHAR(255),
    result_model_id INT
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT s.id, s.user_id, s.name, s.model_id
        FROM Satellites s
        WHERE s.id = p_satellite_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_satellite(
    p_satellite_id INT,
    p_name VARCHAR(255),
    p_model_id INT
)
RETURNS VOID
AS

$$
BEGIN
    UPDATE Satellites
    SET name = p_name,
        model_id = p_model_id
    WHERE id = p_satellite_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_characteristic_by_id(
    p_characteristic_id INT
)
RETURNS TABLE(
    result_id INT,
    result_user_id INT,
    result_satellite_id INT,
    result_characteristic_type VARCHAR(50),
    result_value DECIMAL,
    result_unit VARCHAR(20)
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT c.id, c.user_id, c.satellite_id, c.characteristic_type, c.value, c.unit
        FROM Characteristics c
        WHERE c.id = p_characteristic_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_characteristic(
    p_characteristic_id INT,
    p_characteristic_type VARCHAR(255),
    p_value DECIMAL,
    p_unit VARCHAR(20)
)
RETURNS VOID
AS

$$
BEGIN
    UPDATE Characteristics
    SET characteristic_type = p_characteristic_type,
        value = p_value,
        unit = p_unit
    WHERE id = p_characteristic_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_trajectory_by_id(
    p_trajectory_id INT
)
RETURNS TABLE(
    result_id INT,
    result_user_id INT,
    result_satellite_id INT,
    result_trajectory_data JSONB,
    result_start_time TIMESTAMP,
    result_end_time TIMESTAMP
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT t.id, t.user_id, t.satellite_id, t.trajectory_data, t.start_time, t.end_time
        FROM Trajectories t
        WHERE t.id = p_trajectory_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_trajectory(
    p_trajectory_id INT,
    p_trajectory_data JSONB,
    p_start_time TIMESTAMP,
    p_end_time TIMESTAMP
)
RETURNS VOID
AS

$$
BEGIN
    UPDATE Trajectories
    SET trajectory_data = p_trajectory_data,
        start_time = p_start_time,
        end_time = p_end_time
    WHERE id = p_trajectory_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_report_by_id(
    p_report_id INT
)
RETURNS TABLE(
    result_id INT,
    result_user_id INT,
    result_satellite_id INT,
    result_report_date DATE,
    result_status VARCHAR(100),
    result_details TEXT
)
AS

$$
BEGIN
    RETURN QUERY
        SELECT r.id, r.user_id, r.satellite_id, r.report_date, r.status, r.details
        FROM Reports r
        WHERE r.id = p_report_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_report(
    p_report_id INT,
    p_report_date DATE,
    p_status VARCHAR(100),
    p_details TEXT
)
RETURNS VOID
AS

$$
BEGIN
    UPDATE Reports
    SET report_date = p_report_date,
        status = p_status,
        details = p_details
    WHERE id = p_report_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_satellite(
    p_satellite_id INT,
    p_user_id INT
)
RETURNS VOID
AS

$$
BEGIN
    DELETE FROM Satellites
    WHERE id = p_satellite_id AND user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_model(
    p_model_id INT,
    p_user_id INT
)
RETURNS VOID
AS

$$
BEGIN
    DELETE FROM Models
    WHERE id = p_model_id AND user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_characteristic(
    p_characteristic_id INT,
    p_user_id INT
)
RETURNS VOID
AS

$$
BEGIN
    DELETE FROM Characteristics
    WHERE id = p_characteristic_id AND user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_trajectory(
    p_trajectory_id INT,
    p_user_id INT
)
RETURNS VOID
AS

$$
BEGIN
    DELETE FROM Trajectories
    WHERE id = p_trajectory_id AND user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION delete_report(
    p_report_id INT,
    p_user_id INT
)
RETURNS VOID
AS

$$
BEGIN
    DELETE FROM Reports
    WHERE id = p_report_id AND user_id = p_user_id;
END;

$$ LANGUAGE plpgsql;

CREATE TABLE log (
    id SERIAL PRIMARY KEY,
    table_name VARCHAR(255),
    action_type VARCHAR(50), -- INSERT, UPDATE, DELETE
    old_data JSONB,
    new_data JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Логирование изменений в таблице Users
CREATE OR REPLACE FUNCTION log_users() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Users
CREATE TRIGGER trigger_log_users_insert
AFTER INSERT ON users
FOR EACH ROW EXECUTE PROCEDURE log_users();

CREATE TRIGGER trigger_log_users_update
AFTER UPDATE ON users
FOR EACH ROW EXECUTE PROCEDURE log_users();

CREATE TRIGGER trigger_log_users_delete
AFTER DELETE ON users
FOR EACH ROW EXECUTE PROCEDURE log_users();

-- Логирование изменений в таблице Models
CREATE OR REPLACE FUNCTION log_models() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Models
CREATE TRIGGER trigger_log_models_insert
AFTER INSERT ON models
FOR EACH ROW EXECUTE PROCEDURE log_models();

CREATE TRIGGER trigger_log_models_update
AFTER UPDATE ON models
FOR EACH ROW EXECUTE PROCEDURE log_models();

CREATE TRIGGER trigger_log_models_delete
AFTER DELETE ON models
FOR EACH ROW EXECUTE PROCEDURE log_models();

-- Логирование изменений в таблице Satellites
CREATE OR REPLACE FUNCTION log_satellites() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Satellites
CREATE TRIGGER trigger_log_satellites_insert
AFTER INSERT ON satellites
FOR EACH ROW EXECUTE PROCEDURE log_satellites();

CREATE TRIGGER trigger_log_satellites_update
AFTER UPDATE ON satellites
FOR EACH ROW EXECUTE PROCEDURE log_satellites();

CREATE TRIGGER trigger_log_satellites_delete
AFTER DELETE ON satellites
FOR EACH ROW EXECUTE PROCEDURE log_satellites();

-- Логирование изменений в таблице Characteristics
CREATE OR REPLACE FUNCTION log_characteristics() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Characteristics
CREATE TRIGGER trigger_log_characteristics_insert
AFTER INSERT ON characteristics
FOR EACH ROW EXECUTE PROCEDURE log_characteristics();

CREATE TRIGGER trigger_log_characteristics_update
AFTER UPDATE ON characteristics
FOR EACH ROW EXECUTE PROCEDURE log_characteristics();

CREATE TRIGGER trigger_log_characteristics_delete
AFTER DELETE ON characteristics
FOR EACH ROW EXECUTE PROCEDURE log_characteristics();

-- Логирование изменений в таблице Trajectories
CREATE OR REPLACE FUNCTION log_trajectories() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Trajectories
CREATE TRIGGER trigger_log_trajectories_insert
AFTER INSERT ON trajectories
FOR EACH ROW EXECUTE PROCEDURE log_trajectories();

CREATE TRIGGER trigger_log_trajectories_update
AFTER UPDATE ON trajectories
FOR EACH ROW EXECUTE PROCEDURE log_trajectories();

CREATE TRIGGER trigger_log_trajectories_delete
AFTER DELETE ON trajectories
FOR EACH ROW EXECUTE PROCEDURE log_trajectories();

-- Логирование изменений в таблице Reports
CREATE OR REPLACE FUNCTION log_reports() RETURNS TRIGGER AS

$$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO log(table_name, action_type, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO log(table_name, action_type, old_data, new_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW));
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO log(table_name, action_type, old_data)
            VALUES(TG_TABLE_NAME, TG_OP, row_to_json(OLD));
        RETURN OLD;
    END IF;
END;

$$ LANGUAGE plpgsql;

-- Создание триггеров для таблицы Reports
CREATE TRIGGER trigger_log_reports_insert
AFTER INSERT ON reports
FOR EACH ROW EXECUTE PROCEDURE log_reports();

CREATE TRIGGER trigger_log_reports_update
AFTER UPDATE ON reports
FOR EACH ROW EXECUTE PROCEDURE log_reports();

CREATE TRIGGER trigger_log_reports_delete
AFTER DELETE ON reports
FOR EACH ROW EXECUTE PROCEDURE log_reports();