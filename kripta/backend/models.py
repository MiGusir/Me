from pydantic import BaseModel
from typing import Optional, List, Dict, Any
from enum import Enum

class EncryptionAlgorithm(str, Enum):
    RC5 = "rc5"
    MAGENTA = "magenta"

class EncryptionMode(str, Enum):
    ECB = "ecb"
    CBC = "cbc"
    PCBC = "pcbc"
    CFB = "cfb"
    OFB = "ofb"
    CTR = "ctr"
    RANDOM_DELTA = "random_delta"

class PaddingMode(str, Enum):
    ZEROS = "zeros"
    ANSI_X923 = "ansi_x923"
    PKCS7 = "pkcs7"
    ISO_10126 = "iso_10126"

class ChatRoomCreate(BaseModel):
    name: str
    algorithm: EncryptionAlgorithm
    encryption_mode: EncryptionMode
    padding_mode: PaddingMode

class ChatRoomJoin(BaseModel):
    user_id: str

class MessageSend(BaseModel):
    room_id: str
    user_id: str
    content_type: str
    content: str
    file_name: Optional[str] = None
    file_size: Optional[int] = None

class EncryptedMessage(BaseModel):
    room_id: str
    user_id: str
    encrypted_data: str
    iv: Optional[str] = None
    content_type: str
    file_name: Optional[str] = None