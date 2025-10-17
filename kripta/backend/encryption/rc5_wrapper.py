import subprocess
import os
import tempfile
import base64

class RC5Wrapper:
    def __init__(self, key: bytes):
        self.key = key
        self._compile_if_needed()
    
    def _compile_if_needed(self):
        """Компилирует C++ код если нужно"""
        cpp_dir = os.path.dirname(os.path.abspath(__file__))
        rc5_cpp = os.path.join(cpp_dir, "rc5.cpp")
        rc5_exe = os.path.join(cpp_dir, "rc5")
        
        if not os.path.exists(rc5_exe) or \
           os.path.getmtime(rc5_exe) < os.path.getmtime(rc5_cpp):
            print("Компиляция RC5...")
            compile_cmd = f"g++ -O3 -std=c++11 {rc5_cpp} -o {rc5_exe}"
            subprocess.run(compile_cmd, shell=True, check=True)
    
    def encrypt(self, data: bytes) -> bytes:
        """Шифрование данных с помощью RC5"""
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(data)
            input_file = f.name
        
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(self.key)
            key_file = f.name
        
        output_file = tempfile.mktemp()
        
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            rc5_exe = os.path.join(cpp_dir, "rc5")
            
            # Запуск C++ программы для шифрования
            cmd = [rc5_exe, "encrypt", input_file, output_file, key_file]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"RC5 encryption failed: {result.stderr}")
            
            with open(output_file, 'rb') as f:
                encrypted_data = f.read()
            
            return encrypted_data
            
        finally:
            # Очистка временных файлов
            for file_path in [input_file, key_file, output_file]:
                if os.path.exists(file_path):
                    os.unlink(file_path)
    
    def decrypt(self, encrypted_data: bytes) -> bytes:
        """Дешифрование данных с помощью RC5"""
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(encrypted_data)
            input_file = f.name
        
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(self.key)
            key_file = f.name
        
        output_file = tempfile.mktemp()
        
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            rc5_exe = os.path.join(cpp_dir, "rc5")
            
            # Запуск C++ программы для дешифрования
            cmd = [rc5_exe, "decrypt", input_file, output_file, key_file]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"RC5 decryption failed: {result.stderr}")
            
            with open(output_file, 'rb') as f:
                decrypted_data = f.read()
            
            return decrypted_data
            
        finally:
            # Очистка временных файлов
            for file_path in [input_file, key_file, output_file]:
                if os.path.exists(file_path):
                    os.unlink(file_path)