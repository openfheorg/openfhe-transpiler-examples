#ifndef AES_ECB_DECRYPT_H
#define AES_ECB_DECRYPT_H


#include "aes.h"


// buffer size is exactly AES_BLOCKLEN bytes; 
// you need only AES_init_ctx as IV is not used in ECB 
// NB: ECB is considered insecure for most uses

void AES_ECB_decrypt(const struct AES_ctx* ctx, uint8_t* buf);

#endif //AES_ECB_DECRYPT_H
