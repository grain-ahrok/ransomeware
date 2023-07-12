#include <stdio.h>
#include <string.h>
#include <Windows.h>

#define BUFFER_SIZE 512
char *getUserName();
void getDirectoryFileList(char *dirName, char fileList[100][100] );

// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.
// 서버용 , 클라이언트
// 타깃을 구조체로 묶어 재분류 하라는 것이
// Desktop, Downloads, Documents ...Temp를 한 구조체 안에서 다루도록
// TODO : 키생성, 암호화

#define DESKTOP_DIR "\\Desktop"
#define DOWNLOADS_DIR "\\Downloads"
#define DOCUMENTS_DIR "\\Documents"
#define APPDATA_DIR "\\AppData\\Local\\Temp"

int main() {

    char dirPrefix[100] = "C:\\Users\\";
    char *userName = getUserName();
    strcat(dirPrefix, userName);

    char* dir = (char*)calloc(100, sizeof(char));
    strcat(dir, dirPrefix);

    //[1] Desktop
    char desktop[100][100];
    strcat(dir, DESKTOP_DIR);
    getDirectoryFileList(dir, desktop);
    for(int i = 0; i < 100; i++) {
        if(desktop[i][0] == '\0') break;
        printf("%s\n", desktop[i]);
    }

    dir = (char*)calloc(100, sizeof(char));
    strcat(dir, dirPrefix);
    
    // [2]  Downloads
    char downloads[100][100];
    strcat(dir, DOWNLOADS_DIR);
    getDirectoryFileList(dir, downloads);
    for(int i = 0; i < 100; i++) {
        if(downloads[i][0] == '\0') break;
        printf("%s\n", downloads[i]);
    }

    dir = (char*)calloc(100, sizeof(char));
    strcat(dir, dirPrefix);

    // [3]  Documents
    char documents[100][100];
    strcat(dir, DOCUMENTS_DIR);
    getDirectoryFileList(dir, documents);
    for(int i = 0; i < 100; i++) {
        if(documents[i][0] == '\0') break;
        printf("%s\n", documents[i]);
    }

    dir = (char*)calloc(100, sizeof(char));
    strcat(dir, dirPrefix);

    // [4]  AppData/Local/Temp
    char appData[100][100];
    strcat(dir, APPDATA_DIR);
    getDirectoryFileList(dir, appData);
    for(int i = 0; i < 100; i++) {
        if(appData[i][0] == '\0') break;
        printf("%s\n", appData[i]);
    }

}


/**
 * get user name
*/
char *getUserName() {
    char *acUserName = malloc(sizeof(char) * 100);;
    DWORD nUserName = sizeof(acUserName);
    GetUserName(acUserName, &nUserName);
    return acUserName;
}


/**
 * directory 불러오기
*/
void getDirectoryFileList(char *dir, char fileList[100][100] ) {

    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    int index = 0;

    printf("================================================================================================= \n");
    printf("                       Target directory is %s                         \n", dir);
    printf("================================================================================================= \n");

    strcpy(szDir, dir);
    strcat(szDir, "\\*");
    hFind = FindFirstFileA((LPCSTR)szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {

        printf("\n error FindFirstFile \n");
        return;
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
        strcpy(fileList[index], ffd.cFileName);
        index ++;
        if(index > 100) {
            break;
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        printf("\n error FindFirstFile\n");
    }
    FindClose(hFind);

}



// /**
//  * error box
// */
// void DisplayErrorBox(LPTSTR lpszFunction) 
// { 
//     // Retrieve the system error message for the last-error code

//     LPVOID lpMsgBuf;
//     LPVOID lpDisplayBuf;
//     DWORD dw = GetLastError(); 

//     FormatMessage(
//         FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//         FORMAT_MESSAGE_FROM_SYSTEM |
//         FORMAT_MESSAGE_IGNORE_INSERTS,
//         NULL,
//         dw,
//         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//         (LPTSTR) &lpMsgBuf,
//         0, NULL );

//     // Display the error message and clean up

//     lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
//         (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
//     StringCchPrintf((LPTSTR)lpDisplayBuf, 
//         LocalSize(lpDisplayBuf) / sizeof(TCHAR),
//         TEXT("%s failed with error %d: %s"), 
//         lpszFunction, dw, lpMsgBuf); 
//     MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

//     LocalFree(lpMsgBuf);
//     LocalFree(lpDisplayBuf);
// }
