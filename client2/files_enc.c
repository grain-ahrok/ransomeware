#include "files_enc.h"

void file_encrypt(struct AES_ctx *ctx, wchar_t srcName[100], wchar_t desName[100]) {
    unsigned char* buff_txt;
    buff_txt = calloc(16, sizeof(char));
    DWORD readn;
    DWORD writen;

    printf("================================================================================================= \n");
    printf("            Encrypt File start  %S                         \n", srcName);
    printf("================================================================================================= \n\n");

    // 파일 읽기
    HANDLE file1 = CreateFileW(srcName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_READ_ONLY, NULL);
    if (file1 == INVALID_HANDLE_VALUE) {
        printf("file1 open error\n");
        return ;
    }
    // 파일 읽기 및 암호화 하기
    HANDLE file2 = CreateFileW(desName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (file2 == INVALID_HANDLE_VALUE) {
        printf("file2 open error\n");
        return ;
    }

    while (ReadFile(file1, buff_txt, 16, &readn, NULL)) {
        if (readn == 0)
            break;
        for (readn; readn < 16; readn++) {
            buff_txt[readn] = 0x00;
        }

        AES_ECB_encrypt(ctx, buff_txt);
        WriteFile(file2, buff_txt, readn, &writen, NULL);
        buff_txt = calloc(16, sizeof(unsigned char));
    }
    CloseHandle(file1);
    CloseHandle(file2);
}


void file_decrypt(struct AES_ctx *ctx, wchar_t srcName[100], wchar_t desName[100]) {       
    unsigned char* buff_txt;
    buff_txt = calloc(16, sizeof(char));
    DWORD readn;
    DWORD writen;

    HANDLE file2 = CreateFileW(srcName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file2 == INVALID_HANDLE_VALUE) {
            printf("file2 open error\n");
            return ;
    }

    HANDLE file3 = CreateFileW(desName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (file3 == INVALID_HANDLE_VALUE) {
            printf("file3 open error\n");
            return ;
    }

    while (ReadFile(file2, buff_txt, 16, &readn, NULL)) {
            if (readn == 0)
                break;
        AES_ECB_decrypt(ctx, buff_txt);
        WriteFile(file3, buff_txt, readn, &writen, NULL);
        buff_txt = calloc(16, sizeof(unsigned char));
    }
    CloseHandle(file2);
    CloseHandle(file3);
}





