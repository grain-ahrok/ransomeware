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
