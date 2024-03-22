#include "internet.hpp"


// Wi - Fi 인터페이스의 Mac 주소 가져오기
string getMacAddress() {

	ULONG ulOutBufLen = 0;
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	string macAddress = "";

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

					// MAC 주소 구분자(:) 추가
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