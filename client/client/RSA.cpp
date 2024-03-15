/**
#include "RSA.hpp"

int GetPublicKey() {
    
    return 0;
}


#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <openssl/applink.c>
#include "framework.h"
#include "RSATEST.h"
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define MAX_LOADSTRING 100

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

std::vector<unsigned char> rsaEncrypt(const unsigned char* plainText, size_t plainTextLen, const char* publicKeyPath) {
    // 공개 키를 DER 형식으로 읽어옴
    FILE* publicKeyFile = fopen(publicKeyPath, "rb");
    if (!publicKeyFile) {
        std::cerr << "Error opening public key file." << std::endl;
        return std::vector<unsigned char>();
    }

    RSA* rsaPublicKey = d2i_RSAPublicKey_fp(publicKeyFile, NULL);
    fclose(publicKeyFile);
    if (!rsaPublicKey) {
        std::cerr << "Error loading public key." << std::endl;
        return std::vector<unsigned char>();
    }

    // RSA 암호화
    std::vector<unsigned char> encryptedData(RSA_size(rsaPublicKey), 0);
    int result = RSA_public_encrypt(plainTextLen, plainText, encryptedData.data(), rsaPublicKey, RSA_PKCS1_PADDING);
    if (result == -1) {
        std::cerr << "Error encrypting data." << std::endl;
        RSA_free(rsaPublicKey);
        return std::vector<unsigned char>();
    }

    RSA_free(rsaPublicKey);
    return encryptedData;
}



std::vector<unsigned char> rsaDecrypt(const unsigned char* cipherText, size_t cipherTextLen, const char* privateKeyPath) {
    // 개인 키를 DER 형식으로 읽어옴
    FILE* privateKeyFile = fopen(privateKeyPath, "rb");
    if (!privateKeyFile) {
        std::cerr << "Error opening private key file." << std::endl;
        return std::vector<unsigned char>();
    }

    RSA* rsaPrivateKey = d2i_RSAPrivateKey_fp(privateKeyFile, NULL);
    fclose(privateKeyFile);
    if (!rsaPrivateKey) {
        std::cerr << "Error loading private key." << std::endl;
        return std::vector<unsigned char>();
    }

    // RSA 복호화
    std::vector<unsigned char> decryptedData(RSA_size(rsaPrivateKey), 0);
    int result = RSA_private_decrypt(cipherTextLen, cipherText, decryptedData.data(), rsaPrivateKey, RSA_PKCS1_PADDING);
    if (result == -1) {
        std::cerr << "Error decrypting data." << std::endl;
        RSA_free(rsaPrivateKey);
        return std::vector<unsigned char>();
    }

    RSA_free(rsaPrivateKey);
    return decryptedData;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
{
    const char* publicKeyPath = "./public_key.der";
    const char* plainText = "Hello, World!";
    size_t plainTextLen = strlen(plainText);

    std::vector<unsigned char> encryptedData = rsaEncrypt(reinterpret_cast<const unsigned char*>(plainText), plainTextLen, publicKeyPath);
    if (encryptedData.empty()) {
        std::cerr << "RSA encryption failed." << std::endl;
        return 1;
    }

    std::cout << "Encrypted data: ";
    for (const auto& byte : encryptedData) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    const char* privateKeyPath = "private_key.der";

    std::vector<unsigned char> decryptedData = rsaDecrypt(reinterpret_cast<unsigned char*>(encryptedData.data()), encryptedData.size(), privateKeyPath);
    if (decryptedData.empty()) {
        std::cerr << "RSA decryption failed." << std::endl;
        return 1;
    }

    std::cout << "Decrypted data: " << std::string(decryptedData.begin(), decryptedData.end()) << std::endl;


    return 0;

}

*/