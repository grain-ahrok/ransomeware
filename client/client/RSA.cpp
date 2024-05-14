#include "RSA.hpp"
#include <windows.h>
#include <wincrypt.h>
#include <openssl/err.h>

// 공개키 -> RSA
RSA* getPubKey() {
    FILE* file = fopen("./public_key.der", "rb");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", "public_key");
        return NULL;
    }
    RSA* rsa = d2i_RSA_PUBKEY_fp(file, NULL);
    if (!rsa) {
        fprintf(stderr, "Error reading RSA key from file %s\n", "public_key");
        return NULL;
    }
    fclose(file);
    std::cout << "RSA Public Key loaded successfully." << std::endl;
    return rsa;
}

// 개인키 -> RSA
RSA* getPriKey() {
    const char* privateKeyPath = "./private_key.der";
    
    FILE* privateKeyFile = fopen(privateKeyPath, "rb");
    if (!privateKeyFile) {
        std::cerr << "Error opening private key file." << std::endl;
    }
    RSA* rsaPrivateKey = d2i_RSAPrivateKey_fp(privateKeyFile, NULL);
    fclose(privateKeyFile);
    if (!rsaPrivateKey) {
        std::cerr << "Error loading private key." << std::endl;
    }
    return rsaPrivateKey;
}


void GetPubKeyFromServer(SOCKET clientSocket) {
    // 서버로부터 공개키 수신
    unsigned char publicKeyBuf[294];
    int publicKeyLen = recv(clientSocket, reinterpret_cast<char*>(publicKeyBuf), 294, 0);
    if (publicKeyLen <= 0) {
        cerr << "Failed to receive public key from server! Quitting" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    HANDLE hFile = CreateFile(L"public_key.der", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Failed to create file." << endl;
        return;
    }
    DWORD bytesWritten;
    if (!WriteFile(hFile, publicKeyBuf, sizeof(publicKeyBuf), &bytesWritten, NULL)) {
        cerr << "Failed to write to file." << endl;
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);
}


void GetPriKeyFromServer(SOCKET clientSocket) {
    // 서버로부터 개인키 수신
    unsigned char privateKeyBuf[1192];
    int privateKeyLen = recv(clientSocket, reinterpret_cast<char*>(privateKeyBuf), 1192, 0);

    if (privateKeyLen <= 0) {
        std::cerr << "Failed to receive public key from server! Quitting" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // 받아온 공개키 저장
    HANDLE hFile = CreateFile(L"private_key.der", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Failed to create file." << endl;
        return;
    }
    DWORD bytesWritten;
    if (!WriteFile(hFile, privateKeyBuf, sizeof(privateKeyBuf), &bytesWritten, NULL)) {
        cerr << "Failed to write to file." << endl;
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);
}
