#include "registry.hpp"

const wchar_t* regKeyPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";


void SaveReg() {
    // ���α׷� ��� �ҷ�����
    TCHAR programPath[MAX_PATH];
    GetModuleFileName(NULL, programPath, MAX_PATH);

    HKEY hKey;
    LONG result;
    // TODO : Current User���� ������ �� ���� �� �ִ� ����� �ִ���
    // ������Ʈ�� Ű ����
    result = RegOpenKeyEx(HKEY_CURRENT_USER, regKeyPath, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        cerr << "Failed to open registry key\n";
        return;
    }
    // ������Ʈ�� Ű ���
    result = RegSetValueEx(hKey, L"Chrome", 0, REG_SZ, (BYTE*)programPath, (DWORD)(lstrlen(programPath) + 1) * sizeof(TCHAR));
    if (result == ERROR_SUCCESS)
        cout << "Program added to startup\n";
    else cerr << "Failed to add program to startup\n";
    RegCloseKey(hKey);
}


void DeleteReg() {
    HKEY hKey;
    LONG result;

    // ������Ʈ�� Ű ����
    result = RegOpenKeyEx(HKEY_CURRENT_USER, regKeyPath, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        cerr << "Error opening registry key." << endl;
        return;
    }
    // �� ����
    result = RegDeleteValue(hKey, L"Chrome");
    if (result == ERROR_SUCCESS)
        cout << "Registry key deleted successfully." << endl;
    else cerr << "Error deleting Registry key." << endl;
    RegCloseKey(hKey);
}