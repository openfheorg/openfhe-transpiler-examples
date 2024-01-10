#include "XorWithIv.h"



#pragma hls_top
void XorWithIv(state_t &state, const uint8_t Iv[AES_BLOCKLEN])
{
  uint8_t i, j;
  uint8_t k = 0;
  #pragma hls_unroll yes
  for (i = 0; i < 4; ++i)
  {
    #pragma hls_unroll yes
    for (j = 0; j < 4; ++j)
    {
	  state[i][j]  ^= Iv[k++];
    }
  }
}
