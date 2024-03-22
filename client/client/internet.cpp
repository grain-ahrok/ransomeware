#include "internet.hpp"


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