#include <stdio.h>
#include <Windows.h>
#include <winbase.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#define BUFFER_SIZE 512
char *getUserName();
void DisplayErrorBox(LPTSTR lpszFunction);

// 공격한다는 의미가 무엇인지??
// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.
// 서버용 , 클라이언트
// 타깃을 구조체로 묶어 재분류 하라는 것이
// Desktop, Downloads, Documents ...Temp를 한 구조체 안에서 다루도록

int main()
{

    char *userName = getUserName();
    // TODO : 키생성

    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    char dirName[100] = {0, };
    sprintf(dirName, "%s%s%s", "C:\\Users\\", userName, "\\Documents" );

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.

    StringCchLength(dirName, MAX_PATH, &length_of_arg);
    if (length_of_arg > (MAX_PATH - 3)) {
        printf(TEXT("\nDirectory path is too long.\n"));
        return (-1);
    }

    printf(TEXT("\nTarget directory is %s\n\n"), dirName);

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, dirName);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        DisplayErrorBox(TEXT("FindFirstFile"));
        return dwError;
    }

    // List all the files in the directory with some info about them.
    do{
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        }
        else {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            printf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }
    FindClose(hFind);
}






/**
 * get user name
*/
char *getUserName()
{
    char acUserName[100] = {
        0,
    };
    DWORD nUserName = sizeof(acUserName);
    GetUserName(acUserName, &nUserName);
    return acUserName;
}

/**
 * error box
*/
void DisplayErrorBox(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
