#ifndef AES_INIT_CTX_H
#define AES_INIT_CTX_H

#include "aes.h"

#if 0
void AES_init_ctx(struct AES_ctx& ctx, const uint8_t key[AES_KEYLEN]);
#else
void AES_init_ctx(uint8_t RoundKey[AES_keyExpSize], const uint8_t key[AES_KEYLEN]);
#endif

#endif //AES_INIT_CTX_H
