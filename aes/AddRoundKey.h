#ifndef ADDROUNDKEY_H
#define ADDROUNDKEY_H

#include "aes.h"


static void AddRoundKey(uint8_t round, state_t &state, const uint8_t RoundKey[AES_KEYLEN], uint8_t debug[16]);

#endif //ADDROUNDKEY_H
