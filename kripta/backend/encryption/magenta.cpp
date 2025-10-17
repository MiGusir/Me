#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <stdexcept>

class Magenta {
private:
    std::vector<uint8_t> key;
    int rounds;
    
    static const uint8_t SBOX[256];
    
public:
    Magenta(const std::vector<uint8_t>& key, int rounds = 6) : key(key), rounds(rounds) {
        if (key.size() != 16 && key.size() != 24 && key.size() != 32) {
            throw std::invalid_argument("Key must be 16, 24 or 32 bytes long");
        }
    }
    
private:
    uint16_t F(uint16_t x) {
        uint8_t a = (x >> 8) & 0xFF;
        uint8_t b = x & 0xFF;
        
        uint8_t f_a = SBOX[a ^ b];
        uint8_t f_b = SBOX[b ^ f_a];
        
        return (static_cast<uint16_t>(f_a) << 8) | f_b;
    }
    
    std::vector<std::vector<uint16_t>> generateRoundKeys() {
        std::vector<std::vector<uint16_t>> roundKeys(rounds, std::vector<uint16_t>(4));
        
        std::vector<uint16_t> K;
        for (size_t i = 0; i < key.size(); i += 2) {
            uint16_t word = (static_cast<uint16_t>(key[i]) << 8) | key[i + 1];
            K.push_back(word);
        }
        
        for (int i = 0; i < rounds; i++) {
            for (int j = 0; j < 4; j++) {
                roundKeys[i][j] = K[(i + j) % K.size()];
            }
        }
        
        return roundKeys;
    }
    
    void PE(uint16_t& a, uint16_t& b) {
        uint16_t temp = a;
        a = (a + b) & 0xFFFF;
        b = (temp + b) & 0xFFFF;
    }
    
    void P(uint16_t& a, uint16_t& b) {
        for (int i = 0; i < 3; i++) {
            PE(a, b);
        }
    }
    
    void T(uint16_t& a, uint16_t& b) {
        P(a, b);
    }
    
    std::vector<uint16_t> C(int k) {
        std::vector<uint16_t> result;
        if (k == 2) {
            result = {0x0123, 0x4567, 0x89AB, 0xCDEF};
        } else if (k == 3) {
            result = {0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC, 0xBA98};
        } else if (k == 4) {
            result = {0x0123, 0x4567, 0x89AB, 0xCDEF, 0xFEDC, 0xBA98, 0x7654, 0x3210};
        }
        return result;
    }
    
public:
    std::vector<uint8_t> encryptBlock(const std::vector<uint8_t>& block) {
        if (block.size() != 8) {
            throw std::invalid_argument("Block size must be 8 bytes");
        }
        
        auto roundKeys = generateRoundKeys();
        
        uint16_t x0 = (static_cast<uint16_t>(block[0]) << 8) | block[1];
        uint16_t x1 = (static_cast<uint16_t>(block[2]) << 8) | block[3];
        uint16_t x2 = (static_cast<uint16_t>(block[4]) << 8) | block[5];
        uint16_t x3 = (static_cast<uint16_t>(block[6]) << 8) | block[7];
        
        for (int i = 0; i < rounds; i++) {
            x0 ^= roundKeys[i][0];
            x1 ^= roundKeys[i][1];
            x2 ^= roundKeys[i][2];
            x3 ^= roundKeys[i][3];
            
            uint16_t f0 = F(x0);
            uint16_t f1 = F(x1);
            uint16_t f2 = F(x2);
            uint16_t f3 = F(x3);
            
            uint16_t t0 = x0;
            uint16_t t1 = x1;
            uint16_t t2 = x2;
            uint16_t t3 = x3;
            
            uint16_t old0 = x0, old1 = x1, old2 = x2, old3 = x3;

            x0 = F(old0) ^ old2;
            x1 = F(old1) ^ old3;
            x2 = old0;
            x3 = old1;

            if (i < rounds - 1) {
                uint16_t temp = x0;
                x0 = x1;
                x1 = x2;
                x2 = x3;
                x3 = temp;
            }
        }
        
        std::vector<uint8_t> result(8);
        result[0] = (x0 >> 8) & 0xFF;
        result[1] = x0 & 0xFF;
        result[2] = (x1 >> 8) & 0xFF;
        result[3] = x1 & 0xFF;
        result[4] = (x2 >> 8) & 0xFF;
        result[5] = x2 & 0xFF;
        result[6] = (x3 >> 8) & 0xFF;
        result[7] = x3 & 0xFF;
        
        return result;
    }
    
    std::vector<uint8_t> decryptBlock(const std::vector<uint8_t>& block) {
        if (block.size() != 8) {
            throw std::invalid_argument("Block size must be 8 bytes");
        }
        
        auto roundKeys = generateRoundKeys();
        
        uint16_t x0 = (static_cast<uint16_t>(block[0]) << 8) | block[1];
        uint16_t x1 = (static_cast<uint16_t>(block[2]) << 8) | block[3];
        uint16_t x2 = (static_cast<uint16_t>(block[4]) << 8) | block[5];
        uint16_t x3 = (static_cast<uint16_t>(block[6]) << 8) | block[7];
        
        for (int i = rounds - 1; i >= 0; i--) {
            if (i < rounds - 1) {
                uint16_t temp = x3;
                x3 = x2;
                x2 = x1;
                x1 = x0;
                x0 = temp;
            }

            uint16_t old0 = x2;
            uint16_t old1 = x3;
            uint16_t old2 = x0 ^ F(old0);
            uint16_t old3 = x1 ^ F(old1);

            x0 = old0;
            x1 = old1;
            x2 = old2;
            x3 = old3;

            x0 ^= roundKeys[i][0];
            x1 ^= roundKeys[i][1];
            x2 ^= roundKeys[i][2];
            x3 ^= roundKeys[i][3];
        }
        
        std::vector<uint8_t> result(8);
        result[0] = (x0 >> 8) & 0xFF;
        result[1] = x0 & 0xFF;
        result[2] = (x1 >> 8) & 0xFF;
        result[3] = x1 & 0xFF;
        result[4] = (x2 >> 8) & 0xFF;
        result[5] = x2 & 0xFF;
        result[6] = (x3 >> 8) & 0xFF;
        result[7] = x3 & 0xFF;
        
        return result;
    }
    
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;
        size_t blockSize = 8;
        
        std::vector<uint8_t> paddedData = data;
        size_t padding = blockSize - (paddedData.size() % blockSize);
        if (padding == 0) padding = blockSize;
        
        for (size_t i = 0; i < padding; i++) {
            paddedData.push_back(static_cast<uint8_t>(padding));
        }
        
        for (size_t i = 0; i < paddedData.size(); i += blockSize) {
            std::vector<uint8_t> block(paddedData.begin() + i, paddedData.begin() + i + blockSize);
            auto encryptedBlock = encryptBlock(block);
            result.insert(result.end(), encryptedBlock.begin(), encryptedBlock.end());
        }
        
        return result;
    }
    
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;
        size_t blockSize = 8;
        
        if (data.size() % blockSize != 0) {
            throw std::runtime_error("Invalid data size for decryption");
        }
        
        for (size_t i = 0; i < data.size(); i += blockSize) {
            std::vector<uint8_t> block(data.begin() + i, data.begin() + i + blockSize);
            auto decryptedBlock = decryptBlock(block);
            result.insert(result.end(), decryptedBlock.begin(), decryptedBlock.end());
        }
        
        if (!result.empty()) {
            uint8_t padding = result.back();
            if (padding > 0 && padding <= blockSize) {
                bool validPadding = true;
                for (size_t i = result.size() - padding; i < result.size(); i++) {
                    if (result[i] != padding) {
                        validPadding = false;
                        break;
                    }
                }
                if (validPadding) {
                    result.resize(result.size() - padding);
                }
            }
        }
        
        return result;
    }
};

const uint8_t Magenta::SBOX[256] = {
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,
    0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,
    0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
    0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,
    0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,
    0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
    0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
    0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe,
    0x1b, 0x19, 0x1f, 0x1d, 0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,
    0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d, 0x23, 0x21, 0x27, 0x25,
    0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55, 0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45,
    0x7b, 0x79, 0x7f, 0x7d, 0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65,
    0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d, 0x83, 0x81, 0x87, 0x85,
    0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5, 0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5,
    0xdb, 0xd9, 0xdf, 0xdd, 0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5,
    0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed, 0xe3, 0xe1, 0xe7, 0xe5
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

std::vector<uint8_t> createKey(const std::string& keyStr, size_t keySize = 16) {
    std::vector<uint8_t> key(keySize, 0);
    
    for (size_t i = 0; i < keyStr.length() && i < keySize; i++) {
        key[i] = static_cast<uint8_t>(keyStr[i]);
    }
    
    return key;
}

int main() {
    try {
        std::string inputFile = "input.txt";
        std::string encryptedFile = "encrypted_magenta.bin";
        std::string decryptedFile = "decrypted_magenta.txt";
        
        std::string keyString = "MyMagentaKey123456";
        auto key = createKey(keyString, 16);
        
        Magenta magenta(key, 6);
        
        std::cout << "Reading input file..." << std::endl;
        auto inputData = readFile(inputFile);
        std::cout << "Input file size: " << inputData.size() << " bytes" << std::endl;
        
        std::cout << "Encrypting with Magenta..." << std::endl;
        auto encryptedData = magenta.encrypt(inputData);
        writeFile(encryptedFile, encryptedData);
        std::cout << "Encrypted file saved: " << encryptedFile << std::endl;
        std::cout << "Encrypted size: " << encryptedData.size() << " bytes" << std::endl;
        
        std::cout << "\nReading encrypted file..." << std::endl;
        auto encryptedFileData = readFile(encryptedFile);
        
        std::cout << "Decrypting with Magenta..." << std::endl;
        auto decryptedData = magenta.decrypt(encryptedFileData);
        writeFile(decryptedFile, decryptedData);
        std::cout << "Decrypted file saved: " << decryptedFile << std::endl;
        std::cout << "Decrypted size: " << decryptedData.size() << " bytes" << std::endl;
        
        if (inputData == decryptedData) {
            std::cout << "\n✓ Success! Original and decrypted data match." << std::endl;
        } else {
            std::cout << "\n✗ Error! Original and decrypted data don't match." << std::endl;
            
            std::cout << "First 16 bytes of original: ";
            for (int i = 0; i < std::min(16, (int)inputData.size()); i++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << (int)inputData[i] << " ";
            }
            std::cout << std::endl;
            
            std::cout << "First 16 bytes of decrypted: ";
            for (int i = 0; i < std::min(16, (int)decryptedData.size()); i++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << (int)decryptedData[i] << " ";
            }
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}