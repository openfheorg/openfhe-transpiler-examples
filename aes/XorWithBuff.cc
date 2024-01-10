#include "XorWithBuff.h"

#pragma hls_top
void XorWithBuff(uint8_t buffer[AES_BLOCKLEN], const uint8_t Iv[AES_BLOCKLEN]){
  uint8_t i;
  #pragma hls_unroll yes
  for (i = 0; i < AES_BLOCKLEN; ++i)
  {
	buffer[i]  ^= Iv[i];
  }
}
