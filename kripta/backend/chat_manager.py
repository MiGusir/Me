import asyncio
import json
import pika
from typing import Dict, Set, List, Optional
import base64
import os
import random
import hashlib

from models import EncryptionAlgorithm, ChatRoomCreate, MessageSend, EncryptedMessage

class ChatManager:
    def __init__(self):
        self.active_rooms: Dict[str, Dict] = {}
        self.room_users: Dict[str, Set[str]] = {}
        self.user_connections: Dict[str, any] = {}
        self.rabbit_connection = None
        self.setup_rabbitmq()
    
    def setup_rabbitmq(self):
        try:
            self.rabbit_connection = pika.BlockingConnection(
                pika.ConnectionParameters('localhost')
            )
            self.channel = self.rabbit_connection.channel()
            
            self.channel.exchange_declare(
                exchange='chat_messages',
                exchange_type='direct',
                durable=True
            )
        except Exception as e:
            print(f"RabbitMQ connection failed: {e}")
    
    def create_room(self, room_data: ChatRoomCreate) -> str:
        room_id = f"room_{len(self.active_rooms) + 1}_{random.randint(1000, 9999)}"
        
        # Упрощенная версия без Diffie-Hellman для тестирования
        # Генерируем демонстрационный ключ
        demo_key = hashlib.sha256(f"{room_id}_demo_key".encode()).digest()
        
        self.active_rooms[room_id] = {
            'name': room_data.name,
            'algorithm': room_data.algorithm,
            'encryption_mode': room_data.encryption_mode,
            'padding_mode': room_data.padding_mode,
            'session_key': demo_key,
            'creator_id': None,
            'participant_id': None,
            'is_key_exchanged': True  # Для демонстрации сразу установлен
        }
        
        self.room_users[room_id] = set()
        
        # Создание RabbitMQ очереди для комнаты
        if self.channel:
            self.channel.queue_declare(queue=room_id, durable=True)
            self.channel.queue_bind(
                exchange='chat_messages',
                queue=room_id,
                routing_key=room_id
            )
        
        return room_id
    
    def join_room(self, room_id: str, user_id: str) -> bool:
        if room_id not in self.active_rooms:
            return False
        
        room = self.active_rooms[room_id]
        self.room_users[room_id].add(user_id)
        
        # Если это первый пользователь, устанавливаем его как создателя
        if len(self.room_users[room_id]) == 1:
            room['creator_id'] = user_id
        
        # Если это второй пользователь, устанавливаем как участника
        elif len(self.room_users[room_id]) == 2:
            users = list(self.room_users[room_id])
            room['creator_id'] = users[0]
            room['participant_id'] = users[1]
        
        return True
    
    def leave_room(self, room_id: str, user_id: str):
        if room_id in self.room_users:
            self.room_users[room_id].discard(user_id)
            
            if not self.room_users[room_id]:
                # Комната пуста, очищаем
                if room_id in self.active_rooms:
                    del self.active_rooms[room_id]
                del self.room_users[room_id]
    
    def get_encryption_cipher(self, room_id: str):
        """Получение шифратора для комнаты"""
        if room_id not in self.active_rooms:
            raise ValueError("Room not found")
        
        room = self.active_rooms[room_id]
        
        if not room['session_key']:
            raise ValueError("Session key not established")
        
        # Упрощенная реализация для тестирования
        # В реальном приложении здесь должны быть реальные шифраторы
        class DemoCipher:
            def __init__(self, key):
                self.key = key
            
            def encrypt(self, data):
                # Простое XOR шифрование для демонстрации
                encrypted = bytearray()
                for i, byte in enumerate(data):
                    encrypted.append(byte ^ self.key[i % len(self.key)])
                return bytes(encrypted)
            
            def decrypt(self, data):
                # XOR обратим
                return self.encrypt(data)
        
        return DemoCipher(room['session_key'])
    
    def encrypt_message(self, room_id: str, message: str, content_type: str) -> EncryptedMessage:
        """Шифрование сообщения"""
        cipher = self.get_encryption_cipher(room_id)
        
        # Шифрование данных
        encrypted_data = cipher.encrypt(message.encode('utf-8'))
        
        return EncryptedMessage(
            room_id=room_id,
            user_id="system",
            encrypted_data=base64.b64encode(encrypted_data).decode(),
            content_type=content_type
        )
    
    def decrypt_message(self, room_id: str, encrypted_data: str) -> str:
        """Дешифрование сообщения"""
        cipher = self.get_encryption_cipher(room_id)
        
        encrypted_bytes = base64.b64decode(encrypted_data)
        decrypted_data = cipher.decrypt(encrypted_bytes)
        
        return decrypted_data.decode('utf-8')
    
    def publish_message(self, encrypted_message: EncryptedMessage):
        """Публикация сообщения в RabbitMQ"""
        if self.channel:
            try:
                self.channel.basic_publish(
                    exchange='chat_messages',
                    routing_key=encrypted_message.room_id,
                    body=json.dumps(encrypted_message.dict()),
                    properties=pika.BasicProperties(
                        delivery_mode=2,  # Сообщение сохраняется
                    )
                )
            except Exception as e:
                print(f"Error publishing message: {e}")
    
    def get_room_info(self, room_id: str) -> Optional[Dict]:
        """Получение информации о комнате"""
        if room_id in self.active_rooms:
            room = self.active_rooms[room_id]
            return {
                'room_id': room_id,
                'name': room['name'],
                'algorithm': room['algorithm'],
                'encryption_mode': room['encryption_mode'],
                'users': list(self.room_users[room_id]),
                'is_key_exchanged': room['is_key_exchanged']
            }
        return None