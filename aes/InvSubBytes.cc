#include "InvSubBytes.h"


// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box. Inverse versoin

#pragma hls_top
void InvSubBytes(state_t &state)
{
  uint8_t i, j;
  #pragma hls_unroll yes
  for (i = 0; i < 4; ++i)
  {
    #pragma hls_unroll yes
	for (j = 0; j < 4; ++j)
    {
      state[i][j] = getSBoxInvert(state[i][j]);
    }
  }
}
