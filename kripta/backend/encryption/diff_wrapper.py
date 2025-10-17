import subprocess
import os
import tempfile
import json

class DiffieHellmanWrapper:
    def __init__(self):
        self._compile_if_needed()
        self.public_key = None
        self.private_key = None
        self.shared_secret = None
    
    def _compile_if_needed(self):
        """Компилирует C++ код если нужно"""
        cpp_dir = os.path.dirname(os.path.abspath(__file__))
        diff_cpp = os.path.join(cpp_dir, "diff.cpp")
        diff_exe = os.path.join(cpp_dir, "diff")
        
        if not os.path.exists(diff_exe) or \
           os.path.getmtime(diff_exe) < os.path.getmtime(diff_cpp):
            print("Компиляция Diffie-Hellman...")
            compile_cmd = f"g++ -O3 -std=c++11 {diff_cpp} -o {diff_exe}"
            subprocess.run(compile_cmd, shell=True, check=True)
    
    def generate_keys(self):
        """Генерация ключей Диффи-Хеллмана"""
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            diff_exe = os.path.join(cpp_dir, "diff")
            
            cmd = [diff_exe, "generate_keys"]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"Key generation failed: {result.stderr}")
            
            # Парсинг вывода для получения ключей
            lines = result.stdout.strip().split('\n')
            keys = {}
            for line in lines:
                if ':' in line:
                    key, value = line.split(':', 1)
                    keys[key.strip()] = value.strip()
            
            self.public_key = int(keys.get('Публичный ключ', '0'), 16)
            self.private_key = int(keys.get('Приватный ключ', '0'), 16)
            
            return self.public_key
            
        except Exception as e:
            raise Exception(f"Key generation error: {e}")
    
    def compute_shared_secret(self, other_public_key: int) -> bytes:
        """Вычисление общего секрета"""
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            diff_exe = os.path.join(cpp_dir, "diff")
            
            cmd = [diff_exe, "compute_secret", str(other_public_key), str(self.private_key)]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"Shared secret computation failed: {result.stderr}")
            
            # Получение общего секрета
            lines = result.stdout.strip().split('\n')
            for line in lines:
                if 'Общий секрет' in line:
                    secret_hex = line.split(':')[1].strip()
                    self.shared_secret = int(secret_hex, 16)
                    break
            
            # Деривация ключа шифрования
            return self.derive_key()
            
        except Exception as e:
            raise Exception(f"Shared secret computation error: {e}")
    
    def derive_key(self, key_length: int = 32) -> bytes:
        """Деривация ключа шифрования из общего секрета"""
        if not self.shared_secret:
            raise ValueError("Shared secret not computed")
        
        key = b''
        secret_bytes = self.shared_secret.to_bytes(32, 'big')
        
        # Простая деривация ключа (в реальном приложении используйте HKDF)
        for i in range(key_length):
            key += bytes([secret_bytes[i % 32] ^ i])
        
        return key