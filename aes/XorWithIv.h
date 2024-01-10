#ifndef XORWITHIV_H
#define XORWITHIV_H

#include "aes.h"


void XorWithIv(state_t &state, const uint8_t Iv[AES_BLOCKLEN]);

#endif //XORWITHIV_H
