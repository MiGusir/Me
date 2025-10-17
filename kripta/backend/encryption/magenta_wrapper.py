import subprocess
import os
import tempfile

class MagentaWrapper:
    def __init__(self, key: bytes):
        self.key = key
        self._compile_if_needed()
    
    def _compile_if_needed(self):
        """Компилирует C++ код если нужно"""
        cpp_dir = os.path.dirname(os.path.abspath(__file__))
        magenta_cpp = os.path.join(cpp_dir, "magenta.cpp")
        magenta_exe = os.path.join(cpp_dir, "magenta")
        
        if not os.path.exists(magenta_exe) or \
           os.path.getmtime(magenta_exe) < os.path.getmtime(magenta_cpp):
            print("Компиляция Magenta...")
            compile_cmd = f"g++ -O3 -std=c++11 {magenta_cpp} -o {magenta_exe}"
            subprocess.run(compile_cmd, shell=True, check=True)
    
    def encrypt(self, data: bytes) -> bytes:
        """Шифрование данных с помощью Magenta"""
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(data)
            input_file = f.name
        
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(self.key)
            key_file = f.name
        
        output_file = tempfile.mktemp()
        
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            magenta_exe = os.path.join(cpp_dir, "magenta")
            
            # Запуск C++ программы для шифрования
            cmd = [magenta_exe, "encrypt", input_file, output_file, key_file]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"Magenta encryption failed: {result.stderr}")
            
            with open(output_file, 'rb') as f:
                encrypted_data = f.read()
            
            return encrypted_data
            
        finally:
            # Очистка временных файлов
            for file_path in [input_file, key_file, output_file]:
                if os.path.exists(file_path):
                    os.unlink(file_path)
    
    def decrypt(self, encrypted_data: bytes) -> bytes:
        """Дешифрование данных с помощью Magenta"""
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(encrypted_data)
            input_file = f.name
        
        with tempfile.NamedTemporaryFile(delete=False, mode='wb') as f:
            f.write(self.key)
            key_file = f.name
        
        output_file = tempfile.mktemp()
        
        try:
            cpp_dir = os.path.dirname(os.path.abspath(__file__))
            magenta_exe = os.path.join(cpp_dir, "magenta")
            
            # Запуск C++ программы для дешифрования
            cmd = [magenta_exe, "decrypt", input_file, output_file, key_file]
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode != 0:
                raise Exception(f"Magenta decryption failed: {result.stderr}")
            
            with open(output_file, 'rb') as f:
                decrypted_data = f.read()
            
            return decrypted_data
            
        finally:
            # Очистка временных файлов
            for file_path in [input_file, key_file, output_file]:
                if os.path.exists(file_path):
                    os.unlink(file_path)