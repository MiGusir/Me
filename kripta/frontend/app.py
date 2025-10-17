import streamlit as st
import requests
import json
import websocket
import threading
import time
import base64
import os
from datetime import datetime
import sqlite3
from typing import List, Dict, Optional
import io

# Конфигурация
BACKEND_URL = "http://localhost:8000"
WS_URL = "ws://localhost:8000/ws"

class SecureChatApp:
    def __init__(self):
        self.setup_database()
        self.setup_session_state()
        self.setup_websocket()
    
    def setup_database(self):
        """Инициализация локальной SQLite базы данных"""
        self.conn = sqlite3.connect('secure_chat.db', check_same_thread=False)
        cursor = self.conn.cursor()
        
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS messages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                room_id TEXT,
                user_id TEXT,
                content_type TEXT,
                content TEXT,
                file_name TEXT,
                is_encrypted BOOLEAN DEFAULT 1,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS chat_rooms (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                room_id TEXT UNIQUE,
                room_name TEXT,
                algorithm TEXT,
                encryption_mode TEXT,
                padding_mode TEXT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        self.conn.commit()
    
    def setup_session_state(self):
        """Инициализация состояния сессии"""
        if 'user_id' not in st.session_state:
            st.session_state.user_id = f"user_{int(time.time())}"
        
        if 'current_room' not in st.session_state:
            st.session_state.current_room = None
        
        if 'active_rooms' not in st.session_state:
            st.session_state.active_rooms = []
        
        if 'messages' not in st.session_state:
            st.session_state.messages = {}
        
        if 'websocket' not in st.session_state:
            st.session_state.websocket = None
        
        if 'connection_status' not in st.session_state:
            st.session_state.connection_status = "disconnected"
        
        # Флаг для предотвращения повторного создания комнаты
        if 'room_creation_in_progress' not in st.session_state:
            st.session_state.room_creation_in_progress = False
        
        # Храним ID созданной комнаты
        if 'created_room_id' not in st.session_state:
            st.session_state.created_room_id = None
        
        # Флаг для предотвращения повторного присоединения
        if 'join_room_in_progress' not in st.session_state:
            st.session_state.join_room_in_progress = False
        
        # Время последнего обновления списка комнат
        if 'last_rooms_update' not in st.session_state:
            st.session_state.last_rooms_update = 0
        
        # Кэш активных комнат
        if 'rooms_cache' not in st.session_state:
            st.session_state.rooms_cache = []
        
        # Флаг для ручного обновления
        if 'force_refresh' not in st.session_state:
            st.session_state.force_refresh = False
        
        # Счетчик обновлений для принудительного ререндера
        if 'refresh_counter' not in st.session_state:
            st.session_state.refresh_counter = 0
        
        # Флаг для предотвращения повторной отправки сообщений
        if 'message_sent' not in st.session_state:
            st.session_state.message_sent = False
        
        # Временное хранилище для ввода сообщения
        if 'temp_message' not in st.session_state:
            st.session_state.temp_message = ""
        
        # Время последнего обновления интерфейса
        if 'last_ui_update' not in st.session_state:
            st.session_state.last_ui_update = time.time()
    
    def setup_websocket(self):
        if st.session_state.websocket is not None:
            # Уже подключено
            return

        try:
            ws_url = f"{WS_URL}/{st.session_state.user_id}"
            ws = websocket.WebSocketApp(
                ws_url,
                on_message=self.on_websocket_message,
                on_error=self.on_websocket_error,
                on_close=self.on_websocket_close,
                on_open=self.on_websocket_open
            )
            st.session_state.websocket = ws

            def run_websocket():
                ws.run_forever(ping_interval=30, ping_timeout=10)  # добавим пинги

            thread = threading.Thread(target=run_websocket, daemon=True)
            thread.start()

        except Exception as e:
            st.error(f"WebSocket setup error: {e}")
            st.session_state.websocket = None
    
    def on_websocket_message(self, ws, message):
        """Обработка входящих WebSocket сообщений"""
        try:
            data = json.loads(message)
            message_type = data.get('type')
            
            if message_type == 'new_message':
                room_id = data.get('room_id')
                encrypted_data = data.get('encrypted_data')
                content_type = data.get('content_type')
                file_name = data.get('file_name')
                
                # Запрос на дешифрование
                self.send_websocket_message({
                    'type': 'decrypt_message',
                    'room_id': room_id,
                    'encrypted_data': encrypted_data
                })
            
            elif message_type == 'decrypted_message':
                room_id = data.get('room_id')
                content = data.get('content')
                user_id = data.get('user_id', 'remote_user')
                
                # Сохранение дешифрованного сообщения
                self.save_message(room_id, user_id, content, "text")
                # Устанавливаем флаг для обновления интерфейса
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
            
            elif message_type == 'room_joined':
                st.session_state.join_room_in_progress = False
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
            
            elif message_type == 'room_left':
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
            
            elif message_type == 'error':
                st.session_state.join_room_in_progress = False
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
                
        except Exception as e:
            print(f"WebSocket message processing error: {e}")
    
    def on_websocket_error(self, ws, error):
        """Обработка ошибок WebSocket"""
        st.session_state.connection_status = "error"
        print(f"WebSocket error: {error}")
    
    def on_websocket_close(self, ws, close_status_code, close_msg):
        st.session_state.connection_status = "disconnected"
        print("WebSocket connection closed")
        # Планируем переподключение
        st.session_state.websocket = None
        self.setup_websocket()
    
    def on_websocket_open(self, ws):
        """Обработка открытия WebSocket"""
        st.session_state.connection_status = "connected"
        print("WebSocket connection established")
    
    def send_websocket_message(self, message):
        """Отправка сообщения через WebSocket"""
        try:
            if st.session_state.websocket:
                st.session_state.websocket.send(json.dumps(message))
        except Exception as e:
            st.error(f"Failed to send WebSocket message: {e}")
    
    def save_message(self, room_id: str, user_id: str, content: str, 
                    content_type: str = "text", file_name: str = None):
        """Сохранение сообщения в локальную базу данных"""
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO messages (room_id, user_id, content_type, content, file_name)
            VALUES (?, ?, ?, ?, ?)
        ''', (room_id, user_id, content_type, content, file_name))
        self.conn.commit()
        
        # Обновление сообщений в состоянии сессии
        if room_id not in st.session_state.messages:
            st.session_state.messages[room_id] = []
        
        # Проверяем, нет ли уже такого сообщения (предотвращение дублирования)
        message_exists = any(
            msg['user_id'] == user_id and 
            msg['content'] == content and 
            abs((datetime.fromisoformat(msg['timestamp']) - datetime.now()).total_seconds()) < 2
            for msg in st.session_state.messages[room_id]
        )
        
        if not message_exists:
            st.session_state.messages[room_id].append({
                'user_id': user_id,
                'content_type': content_type,
                'content': content,
                'file_name': file_name,
                'timestamp': datetime.now().isoformat()
            })
    
    def get_room_messages(self, room_id: str) -> List[Dict]:
        """Получение сообщений для комнаты"""
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT user_id, content_type, content, file_name, timestamp
            FROM messages 
            WHERE room_id = ?
            ORDER BY timestamp
        ''', (room_id,))
        
        messages = []
        for row in cursor.fetchall():
            messages.append({
                'user_id': row[0],
                'content_type': row[1],
                'content': row[2],
                'file_name': row[3],
                'timestamp': row[4]
            })
        
        return messages
    
    def create_chat_room(self, name: str, algorithm: str, 
                        encryption_mode: str, padding_mode: str):
        """Создание новой чат-комнаты"""
        # Проверяем, не создается ли уже комната
        if st.session_state.room_creation_in_progress:
            return st.session_state.created_room_id
        
        try:
            st.session_state.room_creation_in_progress = True
            
            response = requests.post(f"{BACKEND_URL}/rooms/create", json={
                "name": name,
                "algorithm": algorithm,
                "encryption_mode": encryption_mode,
                "padding_mode": padding_mode
            })
            
            if response.status_code == 200:
                room_data = response.json()
                room_id = room_data['room_id']
                
                # Сохранение комнаты в локальной базе
                cursor = self.conn.cursor()
                cursor.execute('''
                    INSERT OR IGNORE INTO chat_rooms (room_id, room_name, algorithm, encryption_mode, padding_mode)
                    VALUES (?, ?, ?, ?, ?)
                ''', (room_id, name, algorithm, encryption_mode, padding_mode))
                self.conn.commit()
                
                st.session_state.created_room_id = room_id
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
                return room_id
            else:
                st.error("Ошибка при создании комнаты")
                st.session_state.room_creation_in_progress = False
                return None
                
        except Exception as e:
            st.error(f"Ошибка создания комнаты: {e}")
            st.session_state.room_creation_in_progress = False
            return None
    
    def join_chat_room(self, room_id: str):
        """Присоединение к существующей чат-комнате"""
        # Проверяем, не присоединяемся ли мы уже к комнате
        if st.session_state.join_room_in_progress:
            return True
            
        # Проверяем, не находимся ли мы уже в этой комнате
        if st.session_state.current_room == room_id:
            return True
        
        try:
            st.session_state.join_room_in_progress = True
            
            response = requests.post(
                f"{BACKEND_URL}/rooms/{room_id}/join",
                json={"user_id": st.session_state.user_id}
            )
            
            if response.status_code == 200:
                st.session_state.current_room = room_id
                
                # Отправка WebSocket сообщения о присоединении
                self.send_websocket_message({
                    'type': 'join_room',
                    'room_id': room_id
                })
                
                # Загрузка существующих сообщений
                if room_id not in st.session_state.messages:
                    st.session_state.messages[room_id] = self.get_room_messages(room_id)
                
                st.session_state.force_refresh = True
                st.session_state.refresh_counter += 1
                return True
            else:
                st.error("Ошибка при присоединении к комнате")
                st.session_state.join_room_in_progress = False
                return False
                
        except Exception as e:
            st.error(f"Ошибка присоединения к комнате: {e}")
            st.session_state.join_room_in_progress = False
            return False
    
    def leave_chat_room(self):
        if st.session_state.current_room:
            room_id = st.session_state.current_room
            try:
                requests.post(
                    f"{BACKEND_URL}/rooms/{room_id}/leave",
                    json={"user_id": st.session_state.user_id}
                )
                self.send_websocket_message({'type': 'leave_room', 'room_id': room_id})
            except:
                pass

            st.session_state.current_room = None
            st.session_state.join_room_in_progress = False
            # Опционально: очистить кэш сообщений комнаты
            if room_id in st.session_state.messages:
                del st.session_state.messages[room_id]
            st.session_state.force_refresh = True
            st.session_state.refresh_counter += 1
    
    def send_message(self, content: str, content_type: str = "text", file_name: str = None):
        """Отправка сообщения в текущую комнату"""
        if not st.session_state.current_room:
            st.error("Вы не в чат-комнате")
            return False
        
        # Проверяем, не отправляется ли уже сообщение
        if st.session_state.message_sent:
            return False
        
        try:
            st.session_state.message_sent = True
            
            # Отправка через WebSocket
            self.send_websocket_message({
                'type': 'send_message',
                'room_id': st.session_state.current_room,
                'content': content,
                'content_type': content_type,
                'file_name': file_name
            })
            
            # Локальное сохранение сообщения
            self.save_message(
                st.session_state.current_room,
                st.session_state.user_id,
                content,
                content_type,
                file_name
            )
            
            # Устанавливаем флаг для обновления интерфейса
            st.session_state.force_refresh = True
            st.session_state.refresh_counter += 1
            
            return True
                
        except Exception as e:
            st.error(f"Ошибка отправки сообщения: {e}")
            st.session_state.message_sent = False
            return False
    
    def get_active_rooms(self):
        """Получение списка активных комнат с кэшированием"""
        current_time = time.time()
        
        # Обновляем список комнат если:
        # 1. Прошло больше 5 секунд с последнего обновления
        # 2. Или запрошено принудительное обновление
        # 3. Или кэш пустой
        if (current_time - st.session_state.last_rooms_update > 5 or 
            st.session_state.force_refresh or 
            not st.session_state.rooms_cache):
            
            try:
                response = requests.get(f"{BACKEND_URL}/rooms/active", timeout=3)
                if response.status_code == 200:
                    st.session_state.rooms_cache = response.json()
                    st.session_state.last_rooms_update = current_time
                    st.session_state.force_refresh = False
            except Exception as e:
                print(f"Error fetching active rooms: {e}")
        
        return st.session_state.rooms_cache
    
    def refresh_rooms(self):
        """Принудительное обновление списка комнат"""
        st.session_state.force_refresh = True
        st.session_state.last_rooms_update = 0
        st.session_state.refresh_counter += 1
    
    def render_sidebar(self):
        """Отрисовка боковой панели приложения"""
        with st.sidebar:
            st.title("🔒 Secure Chat")
            
            # Информация о пользователе
            st.write(f"👤 User: `{st.session_state.user_id}`")
            
            # Статус соединения
            status_color = {
                "connected": "🟢",
                "disconnected": "🔴", 
                "error": "🟠"
            }.get(st.session_state.connection_status, "⚪")
            st.write(f"{status_color} Status: {st.session_state.connection_status}")
            
            st.divider()
            
            # Создание комнаты
            st.subheader("Создать комнату")
            
            with st.form("create_room_form", clear_on_submit=True):
                room_name = st.text_input("Название комнаты")
                
                col1, col2 = st.columns(2)
                with col1:
                    algorithm = st.selectbox(
                        "Алгоритм",
                        ["rc5", "magenta"]
                    )
                    encryption_mode = st.selectbox(
                        "Режим шифрования",
                        ["ecb", "cbc", "pcbc", "cfb", "ofb", "ctr", "random_delta"]
                    )
                
                with col2:
                    padding_mode = st.selectbox(
                        "Режим дополнения", 
                        ["zeros", "ansi_x923", "pkcs7", "iso_10126"]
                    )
                
                submitted = st.form_submit_button("Создать комнату")
                if submitted:
                    if room_name:
                        with st.spinner("Создание комнаты..."):
                            room_id = self.create_chat_room(
                                room_name, algorithm, encryption_mode, padding_mode
                            )
                            if room_id:
                                st.success(f"Комната '{room_name}' создана успешно!")
            
            st.divider()
            
            # Активные комнаты
            st.subheader("Активные комнаты")
            
            # Кнопка ручного обновления
            if st.button("🔄 Обновить список", key="refresh_rooms_btn"):
                self.refresh_rooms()
            
            active_rooms = self.get_active_rooms()
            
            if active_rooms:
                for room in active_rooms:
                    col1, col2 = st.columns([3, 1])
                    with col1:
                        st.write(f"**{room['name']}**")
                        st.caption(f"Алгоритм: {room['algorithm']}")
                        st.caption(f"Пользователей: {len(room.get('users', []))}")
                        if room.get('is_key_exchanged'):
                            st.caption("🔑 Ключ установлен")
                        else:
                            st.caption("⏳ Ожидание ключа")
                    
                    with col2:
                        if st.session_state.current_room != room['room_id']:
                            if st.button("Присоединиться", key=f"join_{room['room_id']}"):
                                with st.spinner("Присоединение к комнате..."):
                                    if self.join_chat_room(room['room_id']):
                                        st.success("Успешно присоединились к комнате!")
                        else:
                            st.button("✅ В комнате", key=f"in_{room['room_id']}", disabled=True)
            else:
                st.info("Нет активных комнат")
            
            st.divider()
            
            # Управление текущей комнатой
            if st.session_state.current_room:
                st.info(f"Текущая комната: {st.session_state.current_room}")
                if st.button("Покинуть комнату"):
                    self.leave_chat_room()
                    st.success("Вы вышли из комнаты")
    
    def render_chat_interface(self):
        """Отрисовка основного интерфейса чата"""
        if not st.session_state.current_room:
            st.info("🎯 Присоединитесь к чат-комнате чтобы начать общение")
            return
        
        room_id = st.session_state.current_room
        
        # Заголовок чата
        col1, col2, col3 = st.columns([4, 1, 1])
        with col1:
            st.header(f"💬 Чат: {room_id}")
        with col2:
            if st.button("🔄 Обновить", key="refresh_chat_btn"):
                st.session_state.refresh_counter += 1
        with col3:
            if st.button("🚪 Выйти", key="leave_chat_btn"):
                self.leave_chat_room()
                return
        
        # Отображение сообщений
        st.subheader("Сообщения:")
        if room_id in st.session_state.messages:
            for i, msg in enumerate(st.session_state.messages[room_id]):
                self.render_message(msg, i)
        else:
            st.info("Нет сообщений в этой комнате")
        
        # Ввод сообщения
        st.divider()
        
        tab1, tab2 = st.tabs(["📝 Текст", "📎 Файл"])
        
        with tab1:
            with st.form("message_form", clear_on_submit=True):
                message_text = st.text_area("Введите сообщение:", height=100, key="message_input")
                
                submitted = st.form_submit_button("Отправить", type="primary")
                if submitted:
                    if message_text.strip():
                        success = self.send_message(message_text.strip(), "text")
                        if success:
                            st.session_state.message_sent = False
                            st.success("Сообщение отправлено!")
                    else:
                        st.error("Введите текст сообщения")
        
        with tab2:
            uploaded_file = st.file_uploader("Выберите файл", type=None, key="file_uploader")
            if uploaded_file is not None:
                file_content = uploaded_file.read()
                encoded_content = base64.b64encode(file_content).decode()
                
                if st.button("Отправить файл", key="send_file_btn"):
                    success = self.send_message(encoded_content, "file", uploaded_file.name)
                    if success:
                        st.session_state.message_sent = False
                        st.success("Файл отправлен!")
    
    def render_message(self, message: Dict, index: int):
        """Отрисовка отдельного сообщения"""
        is_own = message['user_id'] == st.session_state.user_id
        
        # Заголовок сообщения
        if is_own:
            st.markdown(f"**Вы** *({message['timestamp']})*")
        else:
            st.markdown(f"**{message['user_id']}** *({message['timestamp']})*")
        
        # Содержимое сообщения
        if message['content_type'] == 'text':
            st.write(message['content'])
        elif message['content_type'] == 'file':
            # Уникальный ключ для каждой кнопки скачивания
            st.download_button(
                label=f"📎 Скачать {message['file_name']}",
                data=base64.b64decode(message['content']),
                file_name=message['file_name'],
                mime="application/octet-stream",
                key=f"download_{index}_{message['timestamp']}"
            )
        
        st.divider()
    
    def run(self):
        """Запуск основного приложения"""
        st.set_page_config(
            page_title="Secure Chat App",
            page_icon="🔒",
            layout="wide",
            initial_sidebar_state="expanded"
        )
        
        # Главный заголовок
        st.title("🔒 Secure Chat Application")
        st.markdown("---")
        
        # В методе run()
        current_time = time.time()
        if current_time - st.session_state.last_ui_update > 1.5:  # чаще — 1.5 сек
            st.session_state.last_ui_update = current_time
            if st.session_state.force_refresh:
                st.session_state.force_refresh = False  # сброс флага
                st.rerun()

        # Автоматическое обновление интерфейса каждые 2 секунды
        current_time = time.time()
        if current_time - st.session_state.last_ui_update > 2:
            st.session_state.last_ui_update = current_time
            if st.session_state.force_refresh:
                st.rerun()
        
        # Принудительное обновление при изменении счетчика
        refresh_counter = st.session_state.refresh_counter
        
        # Основной layout
        col1, col2 = st.columns([1, 3])
        
        with col1:
            self.render_sidebar()
        
        with col2:
            self.render_chat_interface()

# Запуск приложения
if __name__ == "__main__":
    app = SecureChatApp()
    app.run()