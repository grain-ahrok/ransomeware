#include <stdio.h>
#include <string.h>
#include <Windows.h>

#define BUFFER_SIZE      512
#define DESKTOP_DIR     L"\\Desktop"
#define DOWNLOADS_DIR   L"\\Downloads"
#define DOCUMENTS_DIR   L"\\Documents"
#define APPDATA_DIR     L"\\AppData\\Local\\Temp"


struct FILES {
    wchar_t desktop[100][100];
    wchar_t downloads[100][100];
    wchar_t documents[100][100];
    wchar_t appData[500][100];
};

wchar_t *getUserName();

void getDirectoryFileList(wchar_t *dirName, wchar_t fileList[100][100] );
