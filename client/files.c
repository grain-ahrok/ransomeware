#include "files.h"

#define SIGN_BUFF_SIZE  8


/**
 * get user name
*/
wchar_t *getUserName() {
    wchar_t *acUserName = malloc(sizeof(wchar_t*) * 100);
    memset(acUserName, 0, sizeof(wchar_t*) * 100);
    DWORD nUserName = sizeof(acUserName);
    GetUserNameW((LPWSTR)acUserName, &nUserName);
    return acUserName;
}


/**
 * directory 불러오기
*/
void getDirectoryFileList(wchar_t *dir, wchar_t fileList[100][100] ) {

    WIN32_FIND_DATAW ffd;
    LARGE_INTEGER filesize;
    wchar_t szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    int index = 0;

    printf("================================================================================================= \n");
    printf("                       Target directory is %S                         \n", dir);
    printf("================================================================================================= \n\n");

    wcscpy(szDir, dir);
    wcscat(szDir, L"\\*");
    hFind = FindFirstFileW(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {

        printf("\n error FindFirstFile \n");
        exit(0);
    }
    do{
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // printf("  %S   <DIR>\n", ffd.cFileName);
        }
        else {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            // printf("  %S   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
            wcscat(fileList[index], dir);
            wcscat(fileList[index], L"\\");
            wcscat(fileList[index], ffd.cFileName);
            index ++;
        }
        
        if(index > 100) {
            break;
        }
    } while (FindNextFileW(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        printf("\n error FindFirstFile\n");
    }
    FindClose(hFind);
}




void classifyFiles(struct FILES *files, wchar_t *dir, wchar_t fileList[100][100]) {

    char buff_sign[SIGN_BUFF_SIZE];
    DWORD readn;

    printf("================================================================================================= \n");
    printf("                      Classify Files start  %S                         \n", dir);
    printf("================================================================================================= \n\n");

    for (int i = 0; i < 100; i++) {
        if(fileList[i][0] == '\0') break;
        
        HANDLE file1 = CreateFileW(fileList[i], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file1 == INVALID_HANDLE_VALUE) {
            printf("file1 open error\n");
            return;
        }
        ReadFile(file1, buff_sign, SIGN_BUFF_SIZE, &readn, NULL);
        
        if(!strncmp(buff_sign, JPG_SIGN, 4)) {
            wcscpy(files->jpg[files->jpg_index], fileList[i]);
            files->jpg_index++;

        } else if(!strncmp(buff_sign, PNG_SIGN, 8)) {
            wcscpy(files->png[files->png_index], fileList[i]);
            files->png_index++;

        } else if(!strncmp(buff_sign, DOC_SIGN, 4)) {
            wcscpy(files->doc[files->doc_index], fileList[i]);
            files->doc_index++;

        } else if(!strncmp(buff_sign, DOCX_SIGN, 8)) {
            wcscpy(files->docx[files->docx_index], fileList[i]);
            files->docx_index++;

        } else if(wcsstr(fileList[i], L".txt") != NULL){
            wcscpy(files->txt[files->txt_index], fileList[i]);
            files->txt_index++;

        } else {
            
        }
    }
}