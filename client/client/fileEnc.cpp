#include "fileEnc.hpp"


// ����� �̸� ��������
string getUserName() {
    DWORD username_len = UNLEN + 1;
    char userName[UNLEN + 1];

    GetUserNameA(userName, &username_len);
    return userName;
}


void EncryptDir(unsigned char* key, string& dirName)
{
    WIN32_FIND_DATAA data;
    HANDLE hFind = nullptr;
    string folderName;

    vector<string> folders;
    vector<string> files;

    cout << "\n\n ********** " + dirName + " **********" << endl;
    hFind = FindFirstFileA((dirName + "\\*").c_str(), &data);

    if (hFind == INVALID_HANDLE_VALUE){
        return;
    }

    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            folders.push_back(data.cFileName);
        }
        else
        {
            files.push_back(data.cFileName);
        }
    } while (FindNextFileA(hFind, &data) != 0);

    FindClose(hFind);


    // ���� ���
    for (string& file : files)
    {
        cout << "[File] " << file << endl;
        EncryptFileByAES(key, dirName, file);
    }

    // ���� ���
    for (string& folder : folders)
    {
        if (folder == "." || folder == "..") {
            continue;
        }
        folderName = dirName + "\\" + folder;
        cout << "[Folder] " << folderName << endl;
        EncryptDir(key, folderName);
    }
}


void EncryptFileByAES(unsigned char* key, string& dirName, string& fileName) {

    HANDLE handle1;
    HANDLE handle2;
    DWORD size;
    DWORD nRead;
    DWORD nWrite;
    string name = dirName + "\\" + fileName;
    unsigned char* plain_buf;
    unsigned char* cipher_buf;

    // ���� �̸� �� ������ .��ġ 
    size_t lastDot = name.find_last_of('.');
    // Ȯ���ڸ� �� ���� �̸�
    string fileNameStem;
    // ���� Ȯ����
    string fileExtension;

    if (lastDot != string::npos) {
        fileNameStem = name.substr(0, lastDot);
        fileExtension = name.substr(lastDot + 1);
        if (fileExtension == "sdev") return;
    }
    else {
        fileNameStem = name;
    }
    // ��ȣȭ �� ���� �ҷ�����
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    // ��ȣȭ �� �ۼ��� ���� �ҷ����� Ȯ���� .sdev �� ����
    handle2 = CreateFileA((fileNameStem + ".sdev").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (handle1 == INVALID_HANDLE_VALUE || handle2 == INVALID_HANDLE_VALUE) {
        cout << "���� ���� ����" << endl;
        return;
    }

    // ���� ������ Ȯ��
    size = GetFileSize(handle1, NULL);
    size += 16 - size % 16;

    // ���� ������ ��ŭ ���� �Ҵ�
    plain_buf = (unsigned char*)calloc(size, sizeof(char));
    cipher_buf = (unsigned char*)calloc(size, sizeof(char));

    // ��ȣȭ�� ���� �о����
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE)
    {
        cout << "fail to file open " << endl;
        return;
    }
    else
    {
        //// ��ȣȭ
        EncryptText(plain_buf, (unsigned int)size, key, cipher_buf);
        // ��ȣȭ�� ���� .sdev ���Ͽ� �ۼ�
        if (WriteFile(handle2, cipher_buf, nRead, &nWrite, NULL) != TRUE)
        {
            cout << "fail to file write" << endl;
        }
        else {
            cout << fileName << " file is encrypted" << endl;
        }
    }
    CloseHandle(handle1);
    CloseHandle(handle2);

    free(plain_buf);
    free(cipher_buf);

}


// ���� ��ȣȭ
void EncryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.EncryptECB(plain, plainLength, key), plainLength);
}






void DecryptDir(unsigned char* key, string& dirName)
{
    WIN32_FIND_DATAA data;
    HANDLE hFind = nullptr;
    string folderName;

    vector<string> folders;
    vector<string> files;

    cout << "\n\n ********** " + dirName + " **********" << endl;
    hFind = FindFirstFileA((dirName + "\\*").c_str(), &data);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            folders.push_back(data.cFileName);
        }
        else
        {
            files.push_back(data.cFileName);
        }
    } while (FindNextFileA(hFind, &data) != 0);

    FindClose(hFind);


    // ���� ���
    for (string& file : files)
    {
        cout << "[File] " << file << endl;
        DecryptFileByAES(key, dirName, file);
    }

    // ���� ���
    for (string& folder : folders)
    {
        if (folder == "." || folder == "..") {
            continue;
        }
        folderName = dirName + "\\" + folder;
        cout << "[Folder] " << folderName << endl;
        DecryptDir(key, folderName);
    }
}


void DecryptFileByAES(unsigned char* key, string& dirName, string& fileName) {

    HANDLE handle1;
    HANDLE handle2;
    DWORD size;
    DWORD nRead;
    DWORD nWrite;
    string name = dirName + "\\" + fileName;
    unsigned char* plain_buf;
    unsigned char* cipher_buf;

    size_t lastDot = name.find_last_of('.');
    string fileNameStem;
    string fileExtension;

    if (lastDot != string::npos) {
        fileNameStem = name.substr(0, lastDot);
        fileExtension = name.substr(lastDot + 1);
        if (fileExtension != "sdev") return;
    }

    // ��ȣȭ �� ���� �ҷ�����
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    // TODO: ��ȣȭ �� �ۼ��� ���� �ҷ����� Ȯ���� �����ϴ� �Լ� �ʿ�
    handle2 = CreateFileA((fileNameStem + ".dec").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (handle1 == INVALID_HANDLE_VALUE || handle2 == INVALID_HANDLE_VALUE) {
        cout << "���� ���� ����" << endl;
        return;
    }

    // ���� ������ Ȯ��
    size = GetFileSize(handle1, NULL);
    size += 16 - size % 16;

    // ���� ������ ��ŭ ���� �Ҵ�
    plain_buf = (unsigned char*)calloc(size, sizeof(char));
    cipher_buf = (unsigned char*)calloc(size, sizeof(char));

    // ��ȣȭ�� ���� �о����
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE)
    {
        cout << "fail to file open " << endl;
        return;
    }
    else
    {
        // ��ȣȭ
        DecryptText(plain_buf, (unsigned int)size, key, cipher_buf);
        // ��ȣȭ�� ���� .sdev ���Ͽ� �ۼ�
        if (WriteFile(handle2, cipher_buf, nRead, &nWrite, NULL) != TRUE)
        {
            cout << "fail to file write" << endl;
        }
        else {
            cout << fileName << " file is encrypted" << endl;
        }
    }
    CloseHandle(handle1);
    CloseHandle(handle2);

    free(plain_buf);
    free(cipher_buf);

}


// ���� ��ȣȭ
void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.DecryptECB(plain, plainLength, key), plainLength);
}