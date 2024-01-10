#include "IncIvOF.h"

#pragma hls_top
void IncIvOF(uint8_t Iv[AES_BLOCKLEN]){
  uint8_t bi;
#if 0 //old version that now crashes xls
  /* Increment Iv and handle overflow */
  #pragma hls_unroll yes
  for (bi = (AES_BLOCKLEN - 1); bi >= 0; --bi) {
	/* inc will overflow */
	if (Iv[bi] == 255) {
	  Iv[bi] = 0;
	  continue;
	} 
	Iv[bi] += 1;
	break;   
  }
  bi = 0;
 #else
#pragma hls_unroll yes
  for (bi = 0; bi < AES_BLOCKLEN; ++bi) {
    Iv[AES_BLOCKLEN - 1 - bi] += 1;
    if (Iv[AES_BLOCKLEN - 1 - bi] == 0) {
      break;
    }
  }
#endif
}

