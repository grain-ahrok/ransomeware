#include "internet.hpp"

const string SERVER_IP = "43.203.179.45";
const int SERVER_PORT = 54000;


// Wi - Fi �������̽��� Mac �ּ� ��������
string getMacAddress() {

	ULONG ulOutBufLen = 0;
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	string macAddress = "";

	// GetAdaptersInfo ȣ���Ͽ� ������ ũ�� ��������
	if (GetAdaptersInfo(NULL, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		// ���� �Ҵ�
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			return macAddress;
		}

		// ����� ���� ��������
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
			// ù ��° ������� MAC �ּ� ��������
			if (pAdapterInfo->AddressLength > 0) {
				for (DWORD i = 0; i < pAdapterInfo->AddressLength; i++) {
					// MAC �ּҸ� 16���� ���ڿ��� ��ȯ�Ͽ� macAddress�� ����
					char temp[3];
					sprintf_s(temp, "%02X", (int)pAdapterInfo->Address[i]);
					macAddress += temp;

					// MAC �ּ� ������(:) �߰�
					if (i < pAdapterInfo->AddressLength - 1) {
						macAddress += ":";
					}
				}
			}
		}
		// �Ҵ�� �޸� ����
		free(pAdapterInfo);
	}
	return macAddress;
}



SOCKET StartSocket() {
	// ���� �ʱ�ȭ
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed!" << std::endl;
		return -1;
	}
	// ���� ����
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		WSACleanup();
		return -2;
	}
	// ���� ����
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

	return clientSocket;
}