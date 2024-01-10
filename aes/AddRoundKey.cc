#include "AddRoundKey.h"


// This function adds the round key to state.
// The round key is added to the state by an XOR function.
#pragma hls_top
static void AddRoundKey(uint8_t round, state_t &state, const uint8_t RoundKey[AES_keyExpSize], uint8_t debug[16])
{
  uint8_t i,j;
  uint8_t k = 0;

#pragma hls_unroll yes
  for (i = 0; i < 4; ++i)
  {
    #pragma hls_unroll yes
    for (j = 0; j < 4; ++j)
    {
	  state[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
	  debug[k++] = RoundKey[(round * Nb * 4) + (i * Nb) + j];
    }
  }
}
