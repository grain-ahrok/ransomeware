#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "aes.h"
#include "files.h"
#include "files_enc.h"

#define IP "127.0.0.1"
#define PORT 27015

int main() {

    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    int c_socket;
    struct sockaddr_in c_addr;
    unsigned char sendBuffer[BUFFER_SIZE];
    int len;
    WSADATA wsaData;
    int iSendResult;
    char return_value;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        return -1 ;
    }

    c_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&c_addr, 0, sizeof(c_addr));
    c_addr.sin_addr.s_addr = inet_addr(IP);
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(PORT);
    if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1) {
        printf("Cannot connect\n");
        closesocket(c_socket);
        return -1;
    }

        sendBuffer[0] = '*'; // 메시지 시작 문자
        sendBuffer[1] = 16;
        strcpy(&sendBuffer[2], key);

        len =  (18) * sizeof(char); // 전체 메시지 사이즈

        iSendResult = send(c_socket, sendBuffer, len, 0);

        recv(c_socket, &return_value, sizeof(1), 0);
        if (iSendResult == SOCKET_ERROR && return_value == '1') {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(c_socket);
            WSACleanup();
            return -1;
        }
    WSACleanup();
    closesocket(c_socket);


}






