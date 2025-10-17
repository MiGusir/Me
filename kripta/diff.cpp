#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>
#include <string>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/err.h>

class SecureSHA256 {
public:
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> result(EVP_MAX_MD_SIZE);
        unsigned int length = 0;
        
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP context");
        }
        
        if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
            EVP_DigestUpdate(ctx, input.data(), input.size()) != 1 ||
            EVP_DigestFinal_ex(ctx, result.data(), &length) != 1) {
            EVP_MD_CTX_free(ctx);
            throw std::runtime_error("SHA256 computation failed");
        }
        
        EVP_MD_CTX_free(ctx);
        result.resize(length);
        return result;
    }
};

class DiffieHellman {
private:
    BIGNUM* private_key;
    BIGNUM* public_key;
    BIGNUM* shared_secret;
    BIGNUM* p;
    BIGNUM* g;
    BN_CTX* ctx;

    static const char* DEFAULT_P_HEX;
    static const int DEFAULT_G = 2;

    void generatePrivateKey() {
        int num_bits = 256;
        BN_rand(private_key, num_bits, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
        
        BIGNUM* temp = BN_new();
        BN_sub(temp, p, BN_value_one());
        BN_mod(private_key, private_key, temp, ctx);
        BN_free(temp);
        
        if (BN_is_zero(private_key) || BN_is_one(private_key)) {
            BN_add_word(private_key, 2);
        }
    }

public:
    DiffieHellman() {
        ctx = BN_CTX_new();
        ctx = BN_CTX_secure_new();
        private_key = BN_new();
        public_key = BN_new();
        shared_secret = BN_new();
        p = BN_new();
        g = BN_new();

        BN_hex2bn(&p, DEFAULT_P_HEX);
        BN_set_word(g, DEFAULT_G);
        
        generateKeys();
    }

    ~DiffieHellman() {
        BN_clear_free(private_key);
        BN_clear_free(public_key);
        BN_clear_free(shared_secret);
        BN_free(p);
        BN_free(g);
        BN_CTX_free(ctx);
    }

    DiffieHellman(const DiffieHellman&) = delete;
    DiffieHellman& operator=(const DiffieHellman&) = delete;

    void generateKeys() {
        generatePrivateKey();
        BN_mod_exp(public_key, g, private_key, p, ctx);
    }

    bool computeSharedSecret(const BIGNUM* other_public_key) {
        if (BN_is_zero(other_public_key) || BN_is_one(other_public_key)) {
            return false;
        }
        
        BIGNUM* p_minus_one = BN_new();
        BN_sub(p_minus_one, p, BN_value_one());
        
        if (BN_cmp(other_public_key, p_minus_one) >= 0) {
            BN_free(p_minus_one);
            return false;
        }
        BN_free(p_minus_one);

        BN_mod_exp(shared_secret, other_public_key, private_key, p, ctx);
        return true;
    }

    std::string getPublicKeyHex() const {
        char* hex = BN_bn2hex(public_key);
        std::string result(hex);
        OPENSSL_free(hex);
        return result;
    }

    std::string getPrivateKeyHex() const {
        char* hex = BN_bn2hex(private_key);
        std::string result(hex);
        OPENSSL_free(hex);
        return result;
    }

    std::string getSharedSecretHex() const {
        char* hex = BN_bn2hex(shared_secret);
        std::string result(hex);
        OPENSSL_free(hex);
        return result;
    }

    BIGNUM* getPublicKey() const { return public_key; }

    std::vector<uint8_t> deriveEncryptionKey() const {
        int size = BN_num_bytes(shared_secret);
        std::vector<uint8_t> secret_bytes(size);
        BN_bn2bin(shared_secret, secret_bytes.data());
        
        return SecureSHA256::hash(secret_bytes);
    }
};

const char* DiffieHellman::DEFAULT_P_HEX = 
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
    "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
    "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
    "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
    "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
    "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
    "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
    "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
    "15728E5A8AACAA68FFFFFFFFFFFFFFFF";

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
        std::cout << "=== Безопасный протокол Диффи-Хеллмана ===" << std::endl;
        std::cout << "Генерация ключей для Алисы и Боба..." << std::endl;
        
        DiffieHellman alice;
        DiffieHellman bob;
        
        std::cout << "\n--- Ключи Алисы ---" << std::endl;
        std::cout << "Публичный ключ: " << alice.getPublicKeyHex() << std::endl;
        
        std::cout << "\n--- Ключи Боба ---" << std::endl;
        std::cout << "Публичный ключ: " << bob.getPublicKeyHex() << std::endl;
        
        std::cout << "\n--- Обмен ключами ---" << std::endl;
        
        BIGNUM* bob_pub = NULL;
        BN_hex2bn(&bob_pub, bob.getPublicKeyHex().c_str());
        
        BIGNUM* alice_pub = NULL;
        BN_hex2bn(&alice_pub, alice.getPublicKeyHex().c_str());
        
        if (!alice.computeSharedSecret(bob_pub) || 
            !bob.computeSharedSecret(alice_pub)) {
            std::cout << "✗ Ошибка: небезопасные публичные ключи!" << std::endl;
            BN_free(bob_pub);
            BN_free(alice_pub);
            return 1;
        }
        
        BN_free(bob_pub);
        BN_free(alice_pub);
        
        std::cout << "Общий секрет Алисы: " << alice.getSharedSecretHex() << std::endl;
        std::cout << "Общий секрет Боба: " << bob.getSharedSecretHex() << std::endl;
        
        if (alice.getSharedSecretHex() == bob.getSharedSecretHex()) {
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


// mike@Gusar:~/Me/Крипта$ g++ -std=c++11 diff.cpp -lssl -lcrypto
// mike@Gusar:~/Me/Крипта$ ./a.out