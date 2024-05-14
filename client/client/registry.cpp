#include "registry.hpp"

const wchar_t* regKeyPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";


void SaveReg() {
    // 프로그램 경로 불러오기
    TCHAR programPath[MAX_PATH];
    GetModuleFileName(NULL, programPath, MAX_PATH);

    HKEY hKey;
    LONG result;
    // TODO : Current User에서 권한을 더 높일 수 있는 방법이 있는지
    // 레지스트리 키 열기
    result = RegOpenKeyEx(HKEY_CURRENT_USER, regKeyPath, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        cerr << "Failed to open registry key\n";
        return;
    }
    // 레지스트리 키 등록
    result = RegSetValueEx(hKey, L"Chrome", 0, REG_SZ, (BYTE*)programPath, (DWORD)(lstrlen(programPath) + 1) * sizeof(TCHAR));
    if (result == ERROR_SUCCESS)
        cout << "Program added to startup\n";
    else cerr << "Failed to add program to startup\n";
    RegCloseKey(hKey);
}


void DeleteReg() {
    HKEY hKey;
    LONG result;

    // 레지스트리 키 열기
    result = RegOpenKeyEx(HKEY_CURRENT_USER, regKeyPath, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        cerr << "Error opening registry key." << endl;
        return;
    }
    // 값 삭제
    result = RegDeleteValue(hKey, L"Chrome");
    if (result == ERROR_SUCCESS)
        cout << "Registry key deleted successfully." << endl;
    else cerr << "Error deleting Registry key." << endl;
    RegCloseKey(hKey);
}