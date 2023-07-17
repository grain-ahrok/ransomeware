#include "files.h"


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
    printf("================================================================================================= \n");

    wcscpy(szDir, dir);
    wcscat(szDir, L"\\*");
    printf("%S\n", szDir);
    hFind = FindFirstFileW(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {

        printf("\n error FindFirstFile \n");
        exit(0);
    }
    do{
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // printf("  %s   <DIR>\n", ffd.cFileName);
        }
        else {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            // printf("  %s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
        }
        wcscat(fileList[index], dir);
        wcscat(fileList[index], L"\\");
        wcscat(fileList[index], ffd.cFileName);
        index ++;
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