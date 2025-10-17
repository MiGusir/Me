#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>
#include <string>
#include <iomanip>

class SimpleSHA256 {
private:
    static uint32_t rightRotate(uint32_t value, uint32_t shift) {
        return (value >> shift) | (value << (32 - shift));
    }
    
public:
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> result(32, 0);
        
        for (size_t i = 0; i < input.size(); i++) {
            result[i % 32] ^= input[i];
        }
        
        return result;
    }
};

class DiffieHellman {
private:
    uint64_t private_key;
    uint64_t public_key;
    uint64_t shared_secret;
    
    static const uint64_t DEFAULT_P = 0xFFFFFFFFFFFFFFC5;
    static const uint64_t DEFAULT_G = 5;
    
    uint64_t p;
    uint64_t g;
    
public:
    DiffieHellman(uint64_t prime = DEFAULT_P, uint64_t generator = DEFAULT_G) 
        : p(prime), g(generator), private_key(0), public_key(0), shared_secret(0) {
        generateKeys();
    }
    
private:
    void generatePrivateKey() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(2, p - 2);
        private_key = dis(gen);
    }
    
    uint64_t modExp(uint64_t base, uint64_t exponent, uint64_t modulus) {
        uint64_t result = 1;
        base = base % modulus;
        
        while (exponent > 0) {
            if (exponent & 1) {
                result = (result * base) % modulus;
            }
            exponent = exponent >> 1;
            base = (base * base) % modulus;
        }
        
        return result;
    }
    
public:
    void generateKeys() {
        generatePrivateKey();
        public_key = modExp(g, private_key, p);
    }
    
    void computeSharedSecret(uint64_t other_public_key) {
        shared_secret = modExp(other_public_key, private_key, p);
    }
    
    uint64_t getPublicKey() const { return public_key; }
    uint64_t getPrivateKey() const { return private_key; }
    uint64_t getSharedSecret() const { return shared_secret; }
    uint64_t getPrime() const { return p; }
    uint64_t getGenerator() const { return g; }
    
    std::vector<uint8_t> deriveEncryptionKey() const {
        std::vector<uint8_t> secret_bytes(8);
        for (int i = 0; i < 8; i++) {
            secret_bytes[i] = (shared_secret >> (8 * i)) & 0xFF;
        }
        
        return SimpleSHA256::hash(secret_bytes);
    }
};

class SimpleCipher {
private:
    std::vector<uint8_t> key;
    
public:
    SimpleCipher(const std::vector<uint8_t>& encryption_key) : key(encryption_key) {}
    
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> encrypted(data.size());
        
        for (size_t i = 0; i < data.size(); i++) {
            encrypted[i] = data[i] ^ key[i % key.size()];
        }
        
        return encrypted;
    }
    
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encrypted_data) {
        return encrypt(encrypted_data); // XOR обратим
    }
};

std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    return data;
}

void writeFile(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create file: " + filename);
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void printHex(const std::string& label, const std::vector<uint8_t>& data, size_t max_bytes = 16) {
    std::cout << label << " (" << data.size() << " bytes): ";
    for (size_t i = 0; i < std::min(data.size(), max_bytes); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                 << static_cast<int>(data[i]) << " ";
    }
    if (data.size() > max_bytes) {
        std::cout << "...";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    try {
        std::cout << "=== Протокол Диффи-Хеллмана ===" << std::endl;
        std::cout << "Генерация ключей для Алисы и Боба..." << std::endl;
        
        DiffieHellman alice;
        DiffieHellman bob(alice.getPrime(), alice.getGenerator());
        
        std::cout << "\n--- Параметры ---" << std::endl;
        std::cout << "Простое число p: " << std::hex << alice.getPrime() << std::dec << std::endl;
        std::cout << "Генератор g: " << alice.getGenerator() << std::endl;
        
        std::cout << "\n--- Ключи Алисы ---" << std::endl;
        std::cout << "Приватный ключ: " << std::hex << alice.getPrivateKey() << std::dec << std::endl;
        std::cout << "Публичный ключ: " << std::hex << alice.getPublicKey() << std::dec << std::endl;
        
        std::cout << "\n--- Ключи Боба ---" << std::endl;
        std::cout << "Приватный ключ: " << std::hex << bob.getPrivateKey() << std::dec << std::endl;
        std::cout << "Публичный ключ: " << std::hex << bob.getPublicKey() << std::dec << std::endl;
        
        std::cout << "\n--- Обмен ключами ---" << std::endl;
        alice.computeSharedSecret(bob.getPublicKey());
        bob.computeSharedSecret(alice.getPublicKey());
        
        std::cout << "Общий секрет Алисы: " << std::hex << alice.getSharedSecret() << std::dec << std::endl;
        std::cout << "Общий секрет Боба: " << std::hex << bob.getSharedSecret() << std::dec << std::endl;
        
        if (alice.getSharedSecret() == bob.getSharedSecret()) {
            std::cout << "✓ Общие секреты совпадают!" << std::endl;
        } else {
            std::cout << "✗ Ошибка: общие секреты не совпадают!" << std::endl;
            return 1;
        }
        
        std::cout << "\n=== Демонстрация шифрования файла ===" << std::endl;
        
        std::string inputFile = "input.txt";
        std::string encryptedFile = "encrypted_dh.bin";
        std::string decryptedFile = "decrypted_dh.txt";
        
        auto alice_key = alice.deriveEncryptionKey();
        SimpleCipher alice_cipher(alice_key);
        
        auto bob_key = bob.deriveEncryptionKey();
        SimpleCipher bob_cipher(bob_key);
        
        if (alice_key == bob_key) {
            std::cout << "✓ Ключи шифрования совпадают!" << std::endl;
        } else {
            std::cout << "✗ Ошибка: ключи шифрования не совпадают!" << std::endl;
            return 1;
        }
        
        printHex("Ключ шифрования", alice_key);
        
        std::cout << "\nШифрование файла Алисой..." << std::endl;
        auto inputData = readFile(inputFile);
        std::cout << "Размер исходного файла: " << inputData.size() << " байт" << std::endl;
        
        auto encryptedData = alice_cipher.encrypt(inputData);
        writeFile(encryptedFile, encryptedData);
        std::cout << "Зашифрованный файл сохранен: " << encryptedFile << std::endl;
        
        std::cout << "\nДешифрование файла Бобом..." << std::endl;
        auto encryptedFileData = readFile(encryptedFile);
        auto decryptedData = bob_cipher.decrypt(encryptedFileData);
        writeFile(decryptedFile, decryptedData);
        std::cout << "Расшифрованный файл сохранен: " << decryptedFile << std::endl;
        
        if (inputData == decryptedData) {
            std::cout << "✓ Исходные и расшифрованные данные совпадают!" << std::endl;
        } else {
            std::cout << "✗ Ошибка: исходные и расшифрованные данные не совпадают!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}