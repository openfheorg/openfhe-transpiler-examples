#include "SubBytes.h"

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.

#pragma hls_top
void SubBytes(state_t &state)
{
  uint8_t i, j;
  state_t temp_state;
  #pragma hls_unroll yes
  for (i = 0; i < 4; ++i)
  {
    #pragma hls_unroll yes
    for (j = 0; j < 4; ++j)
    {
	  temp_state[i][j] = getSBoxValue(state[i][j]);
    }
  }

    #pragma hls_unroll yes
  for (i = 0; i < 4; ++i)
  {
    #pragma hls_unroll yes
    for (j = 0; j < 4; ++j)
    {
	  state[i][j] = temp_state[i][j];
    }
  }
}
