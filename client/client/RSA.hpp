#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

using namespace std;

RSA* getPubKey();
RSA* getPriKey();

void GetPubKeyFromServer(SOCKET clientSocket);
void GetPriKeyFromServer(SOCKET clientSocket);