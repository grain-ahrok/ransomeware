#include "fileEnc.hpp"


// 사용자 이름 가져오기
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


    // 파일 출력
    for (string& file : files)
    {
        cout << "[File] " << file << endl;
        EncryptFileByAES(key, dirName, file);
    }

    // 폴더 출력
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

    // 파일 이름 중 마지막 .위치 
    size_t lastDot = name.find_last_of('.');
    // 확장자를 뺀 파일 이름
    string fileNameStem;
    // 파일 확장자
    string fileExtension;

    if (lastDot != string::npos) {
        fileNameStem = name.substr(0, lastDot);
        fileExtension = name.substr(lastDot + 1);
        if (fileExtension == "sdev") return;
    }
    else {
        fileNameStem = name;
    }
    // 암호화 할 파일 불러오기
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
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
    if (ReadFile(handle1, plain_buf, size, &nRead, NULL) != TRUE)
    {
        cout << "fail to file open " << endl;
        return;
    }
    else
    {
        //// 암호화
        EncryptText(plain_buf, (unsigned int)size, key, cipher_buf);
        // 암호화한 내용 .sdev 파일에 작성
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


// 파일 암호화
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


    // 파일 출력
    for (string& file : files)
    {
        cout << "[File] " << file << endl;
        DecryptFileByAES(key, dirName, file);
    }

    // 폴더 출력
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

    // 복호화 할 파일 불러오기
    handle1 = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    // TODO: 복호화 후 작성할 파일 불러오기 확장자 지정하는 함수 필요
    handle2 = CreateFileA((fileNameStem + ".dec").c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

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
        // 암호화한 내용 .sdev 파일에 작성
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


// 파일 암호화
void DecryptText(unsigned char* plain, unsigned int plainLength, unsigned char* key, unsigned char* cipher) {
    AES aes(AESKeyLength::AES_256);
    memcpy(cipher, aes.DecryptECB(plain, plainLength, key), plainLength);
}