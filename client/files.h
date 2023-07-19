#include <stdio.h>
#include <string.h>
#include <Windows.h>

#define BUFFER_SIZE      512
#define DESKTOP_DIR     L"\\Desktop"
#define DOWNLOADS_DIR   L"\\Downloads"
#define DOCUMENTS_DIR   L"\\Documents"
#define APPDATA_DIR     L"\\AppData\\Local\\Temp"

static char JPG_SIGN[4] = {0xFF, 0xD8, 0xFF, 0xE0};  // E1, #8
static char PNG_SIGN[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static char DOC_SIGN[4] = {0X0D, 0x44, 0x4F, 0x43};
static char DOCX_SIGN[8] = {0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00};  // zip과 앞의 4byte가 동일

struct FILEDIR {
    wchar_t desktop[100][100];
    wchar_t downloads[100][100];
    wchar_t documents[100][100];
    wchar_t appData[500][100];
};

struct FILES {
    int png_index;
    int jpg_index;
    int doc_index;
    int docx_index;
    int txt_index;
    wchar_t png[100][100];
    wchar_t jpg[100][100];
    wchar_t doc[100][100];
    wchar_t docx[100][100];
    wchar_t txt[100][100];
};


wchar_t *getUserName();

void getDirectoryFileList(wchar_t *dirName, wchar_t fileList[100][100] );

void classifyFiles(struct FILES *files, wchar_t fileList[100][100]);
