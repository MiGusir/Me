#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

class RC5 {
private:
    static const uint32_t P32 = 0xB7E15163;
    static const uint32_t Q32 = 0x9E3779B9;
    
    uint32_t* S;
    int w = 32;
    int r = 12;
    int b = 16;
    int t;
    int c;
    
public:
    RC5(const std::vector<uint8_t>& key, int rounds = 12) : r(rounds) {
        t = 2 * (r + 1);
        c = (b + (w/8) - 1) / (w/8);
        S = new uint32_t[t];
        
        initializeKeySchedule(key);
    }
    
    ~RC5() {
        delete[] S;
    }
    
private:
    uint32_t rotateLeft(uint32_t value, uint32_t shift) {
        return (value << shift) | (value >> (w - shift));
    }
    
    uint32_t rotateRight(uint32_t value, uint32_t shift) {
        return (value >> shift) | (value << (w - shift));
    }
    
    void initializeKeySchedule(const std::vector<uint8_t>& key) {
        uint32_t* L = new uint32_t[c];
        for (int i = 0; i < c; i++) {
            L[i] = 0;
        }
        
        for (int i = 0; i < b; i++) {
            L[i / (w/8)] = (L[i / (w/8)] << 8) + key[i];
        }
        
        S[0] = P32;
        for (int i = 1; i < t; i++) {
            S[i] = S[i-1] + Q32;
        }
        
        uint32_t A = 0, B = 0;
        int i = 0, j = 0;
        
        for (int k = 0; k < 3 * std::max(t, c); k++) {
            A = S[i] = rotateLeft(S[i] + A + B, 3);
            B = L[j] = rotateLeft(L[j] + A + B, (A + B) % w);
            
            i = (i + 1) % t;
            j = (j + 1) % c;
        }
        
        delete[] L;
    }
    
public:
    void encryptBlock(uint32_t& left, uint32_t& right) {
        left += S[0];
        right += S[1];
        
        for (int i = 1; i <= r; i++) {
            left = rotateLeft(left ^ right, right % w) + S[2 * i];
            right = rotateLeft(right ^ left, left % w) + S[2 * i + 1];
        }
    }
    
    void decryptBlock(uint32_t& left, uint32_t& right) {
        for (int i = r; i >= 1; i--) {
            right = rotateRight(right - S[2 * i + 1], left % w) ^ left;
            left = rotateRight(left - S[2 * i], right % w) ^ right;
        }
        
        right -= S[1];
        left -= S[0];
    }
    
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;
        size_t blockSize = (w / 8) * 2;
        
        std::vector<uint8_t> paddedData = data;
        size_t padding = blockSize - (paddedData.size() % blockSize);
        if (padding == blockSize) padding = 0;
        
        for (size_t i = 0; i < padding; i++) {
            paddedData.push_back(static_cast<uint8_t>(padding));
        }
        
        for (size_t i = 0; i < paddedData.size(); i += blockSize) {
            uint32_t left = 0, right = 0;
            
            for (int j = 0; j < 4; j++) {
                left = (left << 8) | paddedData[i + j];
            }
            
            for (int j = 0; j < 4; j++) {
                right = (right << 8) | paddedData[i + 4 + j];
            }
            
            encryptBlock(left, right);
            
            for (int j = 3; j >= 0; j--) {
                result.push_back(static_cast<uint8_t>((left >> (8 * j)) & 0xFF));
            }
            for (int j = 3; j >= 0; j--) {
                result.push_back(static_cast<uint8_t>((right >> (8 * j)) & 0xFF));
            }
        }
        
        return result;
    }
    
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> result;
        size_t blockSize = (w / 8) * 2; // 8 байт для w=32
        
        if (data.size() % blockSize != 0) {
            throw std::runtime_error("Invalid data size for decryption");
        }
        
        for (size_t i = 0; i < data.size(); i += blockSize) {
            uint32_t left = 0, right = 0;
            
            for (int j = 0; j < 4; j++) {
                left = (left << 8) | data[i + j];
            }
            
            for (int j = 0; j < 4; j++) {
                right = (right << 8) | data[i + 4 + j];
            }
            
            decryptBlock(left, right);
            
            for (int j = 3; j >= 0; j--) {
                result.push_back(static_cast<uint8_t>((left >> (8 * j)) & 0xFF));
            }
            for (int j = 3; j >= 0; j--) {
                result.push_back(static_cast<uint8_t>((right >> (8 * j)) & 0xFF));
            }
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

std::vector<uint8_t> createKey(const std::string& keyStr) {
    std::vector<uint8_t> key(16, 0);
    
    for (size_t i = 0; i < keyStr.length() && i < 16; i++) {
        key[i] = static_cast<uint8_t>(keyStr[i]);
    }
    
    return key;
}

int main() {
    try {
        std::string inputFile = "input.txt";
        std::string encryptedFile = "encrypted.bin";
        std::string decryptedFile = "decrypted.txt";
        
        std::string keyString = "MySecretKey12345";
        auto key = createKey(keyString);
        
        RC5 rc5(key);
        
        std::cout << "Reading input file..." << std::endl;
        auto inputData = readFile(inputFile);
        std::cout << "Input file size: " << inputData.size() << " bytes" << std::endl;
        
        std::cout << "Encrypting..." << std::endl;
        auto encryptedData = rc5.encrypt(inputData);
        writeFile(encryptedFile, encryptedData);
        std::cout << "Encrypted file saved: " << encryptedFile << std::endl;
        std::cout << "Encrypted size: " << encryptedData.size() << " bytes" << std::endl;
        
        std::cout << "\nReading encrypted file..." << std::endl;
        auto encryptedFileData = readFile(encryptedFile);
        
        std::cout << "Decrypting..." << std::endl;
        auto decryptedData = rc5.decrypt(encryptedFileData);
        writeFile(decryptedFile, decryptedData);
        std::cout << "Decrypted file saved: " << decryptedFile << std::endl;
        std::cout << "Decrypted size: " << decryptedData.size() << " bytes" << std::endl;
        
        if (inputData == decryptedData) {
            std::cout << "\n✓ Success! Original and decrypted data match." << std::endl;
        } else {
            std::cout << "\n✗ Error! Original and decrypted data don't match." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}