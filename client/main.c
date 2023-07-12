#include <stdio.h>
#include <Windows.h>
#include <winbase.h>

#define BUFFER_SIZE 512

// 공격한다는 의미가 무엇인지??
// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.
// 서버용 , 클라이언트 
// 타깃을 구조체로 묶어 재분류 하라는 것이 
// Desktop, Downloads, Documents ...Temp를 한 구조체 안에서 다루도록

int main() {

    // 이름 불러오기
    char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    GetUserName(acUserName, &nUserName);
    printf("%s \n", acUserName);
    // TODO : 키생성

    // Temp 파일에 접근
    char buf[BUFFER_SIZE] = {0,};
    DWORD readn;
    DWORD writen;

    char srcName[100];
    char desName[100];
    sprintf(srcName, "C:\\Users\\%s%s", acUserName,"\\source.txt");
    sprintf(desName, "C:\\Users\\%s%s", acUserName,"\\AppData\\Local\\Temp\\destination.txt");

    // 읽기 파일
    HANDLE file1 = CreateFile(srcName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file1 == INVALID_HANDLE_VALUE) {
        printf("file1 open error\n");
        return 0;
    }

    // 쓰기 파일
    HANDLE file2 = CreateFile(desName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if(file2 == INVALID_HANDLE_VALUE) {
        printf("file2 open error\n");
        return 0;
    }
    while (ReadFile(file1, buf, BUFFER_SIZE-1, &readn, NULL)) {
        if(readn == 0){
            break;
        }
        // TODO : 암호화 필요
        WriteFile(file2, buf, readn, &writen, NULL);
    }


}

