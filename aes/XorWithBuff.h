#ifndef XORWITHBUFF_H
#define XORWITHBUFF_H

#include "aes.h"


void XorWithBuff(uint8_t buffer[AES_BLOCKLEN], const uint8_t Iv[AES_BLOCKLEN]);

#endif //XORWITHBUFF_H
