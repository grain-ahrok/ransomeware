#include <stdio.h>
#include <Windows.h>
#include "aes.h"

#define AES_BLOCK_SIZE  16


void file_encrypt(struct AES_ctx *ctx, wchar_t srcName[100], wchar_t desName[100]);

void file_decrypt(struct AES_ctx *ctx, wchar_t srcName[100], wchar_t desName[100]);