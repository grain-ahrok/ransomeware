#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

using namespace std;

RSA* getPubKey();
RSA* getPriKey();