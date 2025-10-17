from fastapi import FastAPI, WebSocket, WebSocketDisconnect, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from typing import List, Dict
import json
import asyncio
import uvicorn
import logging

# Настройка логирования
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

from models import (
    ChatRoomCreate, ChatRoomJoin, MessageSend, 
    EncryptionAlgorithm, EncryptionMode, PaddingMode
)
from chat_manager import ChatManager

app = FastAPI(
    title="Secure Chat API",
    description="API для защищенного обмена сообщениями",
    version="1.0.0"
)

# CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

chat_manager = ChatManager()

# WebSocket connections
class ConnectionManager:
    def __init__(self):
        self.active_connections: Dict[str, WebSocket] = {}
    
    async def connect(self, websocket: WebSocket, user_id: str):
        await websocket.accept()
        self.active_connections[user_id] = websocket
    
    def disconnect(self, user_id: str):
        if user_id in self.active_connections:
            del self.active_connections[user_id]
    
    async def send_personal_message(self, message: str, user_id: str):
        if user_id in self.active_connections:
            await self.active_connections[user_id].send_text(message)
    
    async def broadcast_to_room(self, message: str, room_id: str):
        """Отправка сообщения всем пользователям в комнате"""
        if room_id in chat_manager.room_users:
            for user_id in chat_manager.room_users[room_id]:
                if user_id in self.active_connections:
                    await self.active_connections[user_id].send_text(message)

manager = ConnectionManager()

@app.get("/")
async def root():
    return {"message": "Secure Chat API", "version": "1.0.0"}

@app.get("/health")
async def health_check():
    return {"status": "healthy", "active_rooms": len(chat_manager.active_rooms)}

@app.post("/rooms/create")
async def create_room(room_data: ChatRoomCreate):
    try:
        logger.debug(f"Received room creation request: {room_data}")
        room_id = chat_manager.create_room(room_data)
        logger.debug(f"Room created successfully: {room_id}")
        return {
            "room_id": room_id,
            "message": "Room created successfully",
            "room_data": room_data.dict()
        }
    except Exception as e:
        logger.error(f"Error creating room: {str(e)}", exc_info=True)
        raise HTTPException(status_code=400, detail=str(e))

@app.post("/rooms/{room_id}/join")
async def join_room(room_id: str, join_data: ChatRoomJoin):
    logger.debug(f"Join room request: room_id={room_id}, user_id={join_data.user_id}")
    success = chat_manager.join_room(room_id, join_data.user_id)
    if not success:
        raise HTTPException(status_code=404, detail="Room not found or cannot join")
    
    room_info = chat_manager.get_room_info(room_id)
    return {
        "message": "Joined room successfully",
        "room_info": room_info
    }

@app.post("/rooms/{room_id}/leave")
async def leave_room(room_id: str, join_data: ChatRoomJoin):
    chat_manager.leave_room(room_id, join_data.user_id)
    return {"message": "Left room successfully"}

@app.post("/messages/send")
async def send_message(message: MessageSend):
    try:
        encrypted_msg = chat_manager.encrypt_message(
            message.room_id, 
            message.content, 
            message.content_type
        )
        chat_manager.publish_message(encrypted_msg)
        
        # Уведомление пользователей в комнате
        await manager.broadcast_to_room(
            json.dumps({
                'type': 'new_message',
                'room_id': message.room_id,
                'encrypted_data': encrypted_msg.encrypted_data,
                'content_type': message.content_type,
                'file_name': message.file_name
            }),
            message.room_id
        )
        
        return {"message": "Message sent successfully"}
    except Exception as e:
        logger.error(f"Error sending message: {str(e)}", exc_info=True)
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/rooms/active")
async def get_active_rooms():
    rooms = []
    for room_id in chat_manager.active_rooms:
        room_info = chat_manager.get_room_info(room_id)
        if room_info:
            rooms.append(room_info)
    return rooms

@app.get("/rooms/{room_id}")
async def get_room_info(room_id: str):
    room_info = chat_manager.get_room_info(room_id)
    if not room_info:
        raise HTTPException(status_code=404, detail="Room not found")
    return room_info

@app.websocket("/ws/{user_id}")
async def websocket_endpoint(websocket: WebSocket, user_id: str):
    await manager.connect(websocket, user_id)
    try:
        while True:
            data = await websocket.receive_text()
            await handle_websocket_message(user_id, data)
    except WebSocketDisconnect:
        manager.disconnect(user_id)
        # Покидаем все комнаты при отключении
        for room_id in list(chat_manager.room_users.keys()):
            if user_id in chat_manager.room_users[room_id]:
                chat_manager.leave_room(room_id, user_id)

async def handle_websocket_message(user_id: str, message_data: str):
    try:
        data = json.loads(message_data)
        message_type = data.get('type')
        
        if message_type == 'join_room':
            room_id = data.get('room_id')
            if room_id and chat_manager.join_room(room_id, user_id):
                await manager.send_personal_message(
                    json.dumps({
                        'type': 'room_joined',
                        'room_id': room_id,
                        'room_info': chat_manager.get_room_info(room_id)
                    }),
                    user_id
                )
        
        elif message_type == 'leave_room':
            room_id = data.get('room_id')
            if room_id:
                chat_manager.leave_room(room_id, user_id)
                await manager.send_personal_message(
                    json.dumps({
                        'type': 'room_left',
                        'room_id': room_id
                    }),
                    user_id
                )
        
        elif message_type == 'send_message':
            room_id = data.get('room_id')
            content = data.get('content')
            content_type = data.get('content_type', 'text')
            file_name = data.get('file_name')
            
            if room_id and content:
                try:
                    encrypted_msg = chat_manager.encrypt_message(room_id, content, content_type)
                    chat_manager.publish_message(encrypted_msg)
                    
                    await manager.broadcast_to_room(
                        json.dumps({
                            'type': 'new_message',
                            'room_id': room_id,
                            'encrypted_data': encrypted_msg.encrypted_data,
                            'content_type': content_type,
                            'file_name': file_name
                        }),
                        room_id
                    )
                    
                except Exception as e:
                    await manager.send_personal_message(
                        json.dumps({
                            'type': 'error',
                            'message': f"Encryption failed: {str(e)}"
                        }),
                        user_id
                    )
        
        elif message_type == 'decrypt_message':
            room_id = data.get('room_id')
            encrypted_data = data.get('encrypted_data')
            
            if room_id and encrypted_data:
                try:
                    decrypted_content = chat_manager.decrypt_message(room_id, encrypted_data)
                    
                    await manager.send_personal_message(
                        json.dumps({
                            'type': 'decrypted_message',
                            'room_id': room_id,
                            'content': decrypted_content
                        }),
                        user_id
                    )
                    
                except Exception as e:
                    await manager.send_personal_message(
                        json.dumps({
                            'type': 'error',
                            'message': f"Decryption failed: {str(e)}"
                        }),
                        user_id
                    )
    
    except json.JSONDecodeError:
        await manager.send_personal_message(
            json.dumps({
                'type': 'error',
                'message': 'Invalid JSON format'
            }),
            user_id
        )
    except Exception as e:
        await manager.send_personal_message(
            json.dumps({
                'type': 'error',
                'message': f"Processing error: {str(e)}"
            }),
            user_id
        )

if __name__ == "__main__":
    uvicorn.run(
        "main:app",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="debug"
    )