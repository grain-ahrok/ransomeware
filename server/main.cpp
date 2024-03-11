#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <fstream>
#include <arpa/inet.h>
#include <ctime>
#include <openssl/ec.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

//g++ -std=c++14 -Wall main.cpp -o main -lssl -lcrypto

// 로그 파일 경로
const std::string LOG_FILE = "server_log.txt";
const std::string KEY_FILE = "key.txt";

// 클라이언트로부터 받은 메시지와 클라이언트의 IP 주소를 로그 파일에 저장하는 함수
void writeToLog(const std::string& message) {
    std::ofstream logFile(LOG_FILE, std::ios::app); // 파일을 추가 모드로 열기
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file!" << std::endl;
    }
}

// 클라이언트로부터 받은 키를 파일에 저장하는 함수
void saveKeyToFile(const std::string& clientIP, const std::string& key) {
    std::ofstream keyFile(KEY_FILE, std::ios::app); // 파일을 추가 모드로 열기
    if (keyFile.is_open()) {
        keyFile << "Client IP: " << clientIP << "\nKey: " << key << std::endl;
        keyFile.close();
    } else {
        std::cerr << "Unable to open key file!" << std::endl;
    }
}

bool generateAndSaveRSAKeys(const std::string& publicKeyFile, const std::string& privateKeyFile) {
    // RSA 키 쌍 생성
    RSA *rsa = RSA_new();
    BIGNUM *bne = BN_new();
    BN_set_word(bne, RSA_F4);
    RSA_generate_key_ex(rsa, 2048, bne, NULL);

    // 개인키를 파일에 저장
    FILE* privateKeyFilePtr = fopen(privateKeyFile.c_str(), "w");
    if (!privateKeyFilePtr) {
        std::cerr << "Failed to create private key file!" << std::endl;
        RSA_free(rsa);
        BN_free(bne);
        return false;
    }
    if (!PEM_write_RSAPrivateKey(privateKeyFilePtr, rsa, NULL, NULL, 0, NULL, NULL)) {
        std::cerr << "Failed to write private key to file!" << std::endl;
        RSA_free(rsa);
        BN_free(bne);
        fclose(privateKeyFilePtr);
        return false;
    }
    fclose(privateKeyFilePtr);

    // 공개키를 파일에 저장
    FILE* publicKeyFilePtr = fopen(publicKeyFile.c_str(), "w");
    if (!publicKeyFilePtr) {
        std::cerr << "Failed to create public key file!" << std::endl;
        RSA_free(rsa);
        BN_free(bne);
        return false;
    }
    if (!PEM_write_RSA_PUBKEY(publicKeyFilePtr, rsa)) {
        std::cerr << "Failed to write public key to file!" << std::endl;
        RSA_free(rsa);
        BN_free(bne);
        fclose(publicKeyFilePtr);
        return false;
    }
    fclose(publicKeyFilePtr);

    RSA_free(rsa);
    BN_free(bne);
    return true;
}

void handleClient(int clientSocket) {
    char buf[4096];
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    // 클라이언트의 소켓 주소 가져오기
    getpeername(clientSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

    // 현재 시간을 문자열로 변환
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char currentTime[80];
    strftime(currentTime, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    // 클라이언트 MAC 주소 가져오기
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        std::cerr << "Failed to receive MAC address from client! Quitting" << std::endl;
        close(clientSocket);
    }
    std::string macAddress(buffer, bytesRead);

    // 연결 시간과 클라이언트 IP 주소를 로그에 기록
    std::string logMessage = "Connected: " + std::string(currentTime) + " - Client IP: " + std::string(clientIP) + " - MAC Address: " + macAddress;
    writeToLog(logMessage);

    // RSA 공개키와 개인키 파일 경로
    const std::string publicKeyFile = "public_key_" + std::string(clientIP) + ".pem";
    const std::string privateKeyFile = "private_key_" + std::string(clientIP) + ".pem";

    // RSA 키 생성 및 파일로 저장
    if (!generateAndSaveRSAKeys(publicKeyFile, privateKeyFile)) {
        std::cerr << "Failed to generate and save RSA keys!" << std::endl;
        close(clientSocket);
        return;
    }

    // 클라이언트에게 공개키 전송
    std::ifstream publicKeyFileInputStream(publicKeyFile, std::ios::binary);
    std::string publicKey((std::istreambuf_iterator<char>(publicKeyFileInputStream)),
                           std::istreambuf_iterator<char>());
    publicKeyFileInputStream.close();

    send(clientSocket, publicKey.c_str(), publicKey.size(), 0);
    

    while (true) {
        ssize_t bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1) {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }
        if (bytesReceived == 0) {
            std::cout << "The client disconnected" << std::endl;

            // 연결 종료 시간을 기록
            time_t disconnectTime = time(0);
            struct tm* disconnectTimeinfo = localtime(&disconnectTime);
            char disconnectTimeStr[80];
            strftime(disconnectTimeStr, 80, "%Y-%m-%d %H:%M:%S", disconnectTimeinfo);
            std::string disconnectMessage = "Disconnected: " + std::string(disconnectTimeStr) + " - Client IP: " + std::string(clientIP)  + " - MAC Address: " + macAddress;
            writeToLog(disconnectMessage);

            break;
        }
        std::string receivedMessage = std::string(buf, 0, bytesReceived);
        std::cout << "Received: " << receivedMessage << std::endl;

        // 로그 파일에 클라이언트 IP 주소와 메시지 기록
        writeToLog(receivedMessage);

        // 클라이언트로부터 받은 키를 파일에 저장
        saveKeyToFile(clientIP, receivedMessage);

        send(clientSocket, buf, bytesReceived, 0);
    }
    close(clientSocket);
}


int main() {
    // 소켓 생성
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return -1;
    }

    // 서버 주소 설정
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); // 54000번 포트 사용
    hint.sin_addr.s_addr = INADDR_ANY; // 현재 호스트의 IP 주소 사용

    // 소켓과 서버 주소 바인딩
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't bind to IP/port! Quitting" << std::endl;
        return -2;
    }

    // 클라이언트로부터 연결 요청을 대기
    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "Can't listen! Quitting" << std::endl;
        return -3;
    }

    std::cout << "Waiting for a client to connect..." << std::endl;

    std::vector<std::thread> clientThreads;
    // 클라이언트 연결 수락
    while (true) {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        if (clientSocket == -1) {
            std::cerr << "Problem with client connecting! Quitting" << std::endl;
            return -4;
        }

        // 새 스레드 생성하여 클라이언트 처리
        clientThreads.emplace_back(handleClient, clientSocket);
    }

    // 스레드 종료 대기
    for (auto& t : clientThreads) {
        t.join();
    }

    // 소켓 닫기
    close(listening);

    return 0;
}
