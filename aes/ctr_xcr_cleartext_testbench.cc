// Copyright 2023 Duality Technologies Inc.
// D. Cousins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <random>
#include <stdint.h>
#include <iostream>

#include "aes.h"

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/aes/AES_init_ctx_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/AddRoundKey_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/SubBytes_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/ShiftRows_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/MixColumns_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/XorWithBuff_yosys_interpreted_cleartext.h"
#include "transpiler/examples/aes/IncIvOF_yosys_interpreted_cleartext.h"
#else

#include "transpiler/examples/aes/AES_init_ctx_cleartext.h"
#include "transpiler/examples/aes/AddRoundKey_cleartext.h"
#include "transpiler/examples/aes/SubBytes_cleartext.h"
#include "transpiler/examples/aes/ShiftRows_cleartext.h"
#include "transpiler/examples/aes/MixColumns_cleartext.h"
#include "transpiler/examples/aes/XorWithBuff_cleartext.h"
#include "transpiler/examples/aes/IncIvOF_cleartext.h"

#endif

using namespace std;

#define MSG_SIZE 64

// prints string as hex
static void phex(uint8_t* str)
{

#if defined(AES256)
    uint8_t len = 32;
#elif defined(AES192)
    uint8_t len = 24;
#elif defined(AES128)
    uint8_t len = 16;
#endif

    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}



// prints string as hex //only first 16  bytes
static void phex16(uint8_t* str)
{

    uint8_t len = 16;

    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}


#if 0		
		
static void debug_phex(EncodedArrayRef<uint8_t, SDIMX, SDIMY> CT_state,
					   bool dbg_flag){

  if (dbg_flag) {
	uint8_t dec_state[SDIMX][SDIMY];
	uint8_t output[SDIMX*SDIMX];
	uint8_t kk = 0;
	CT_state.Decrypt(dec_state, sk);
	for (auto ii = 0; ii< SDIMX; ii++) {
	  for (auto jj = 0; jj< SDIMY; jj++) {
		output[kk++] = dec_state[ii][jj];
	  }
	}
	cout<< "Debug: "<< endl;
	phex(output);
  }
}


static void debug_phex16(EncodedFheArrayRef<uint8_t, 16> CT_debug, bool dbg_flag){

  if (dbg_flag) {
	uint8_t output[16];

	CT_debug.Decrypt(output, sk);
	cout<< "Debug16: "<< endl;
	phex(output);
  }
}

#endif

// Cipher is the main function that encrypts the PlainText.
void Cipher(EncodedArrayRef<uint8_t, SDIMX, SDIMY> CT_state, const EncodedArrayRef<uint8_t, AES_keyExpSize> CT_RoundKey)
{
  uint8_t round = 0;
  Encoded<uint8_t> CT_round;
  
  EncodedArray<uint8_t, 16> CT_Debug; 
  
  
  CT_round.Encode(round);
  // Add the First round key to the state before starting the rounds.
  //cout<<"Round 0 AddRoundKey"<<endl;
  XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug));
  //debug_phex(CT_state);
  //printf("key ");debug_phex16(CT_Debug);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without MixColumns()
  //for (round = 1; round < Nr+2; ++round)

  for (round = 1; ; ++round)
  {
	//cout<<"Round "<< (unsigned int)round <<endl;

	//cout<<"SubBytes " << endl;
	XLS_CHECK_OK(SubBytes(CT_state));
	//debug_phex(CT_state);
	//cout<<"ShiftRows " << endl;
	XLS_CHECK_OK(ShiftRows(CT_state));
	//debug_phex(CT_state);
    if (round == Nr) {
      break;
    }
	//cout<<"MixColumns " << endl;
	XLS_CHECK_OK(MixColumns(CT_state));
	//debug_phex(CT_state);
	CT_round.Encode(round);
	//cout << "round again " <<(unsigned int)CT_round.Decode();
	//cout<<"AddRoundKey " << endl;
	XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug));
	//debug_phex(CT_state);
	//printf("key ");debug_phex16(CT_Debug);
  }
  // Add round key to last round
  CT_round.Encode(Nr);
  //cout<<"AddRoundKey last round" << endl;
  XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug));
  //debug_phex(CT_state);
}

void AES_CTR_xcrypt_buffer(EncodedArrayRef<uint8_t, AES_keyExpSize> CT_RoundKey,
							EncodedArrayRef<uint8_t, AES_BLOCKLEN> CT_iv, EncodedArrayRef<uint8_t, MSG_SIZE> CT_InBuffer, unsigned int length)
{

  size_t i; // counter over blocks in inbuffer

  EncodedArray<uint8_t, AES_BLOCKLEN> CT_buffer;
  EncodedArray<uint8_t, AES_BLOCKLEN> CT_buffer2;
  EncodedArray<uint8_t, SDIMX, SDIMY> CT_state;
  
  for (i = 0; i < length; i += AES_BLOCKLEN) {

	auto kk = 0;
	for (auto ii = 0; ii< SDIMX; ii++) {
	  for (auto jj = 0; jj< SDIMY; jj++) {
		CT_state[ii][jj] = CT_iv[kk++];
	  }
	}
	
	Cipher(CT_state, CT_RoundKey);	
	size_t kout = 0; //count over chars to write back to buffer
	for (auto ii = 0; ii< SDIMX; ii++) {
	  for (auto jj = 0; jj< SDIMY; jj++) {
		CT_buffer[kout++] = CT_state[ii][jj];
	  }
	}
	// CT_buffer contains encrypted Iv
	// increment Iv and handle overflow
	
	XLS_CHECK_OK(IncIvOF(CT_iv)); 	

	//have to coppy a block for processing.
	for (auto ii = 0; ii < AES_BLOCKLEN; ii++){
	  CT_buffer2[ii] = CT_InBuffer[i+ii];
	}
	XLS_CHECK_OK(XorWithBuff(CT_buffer2, CT_buffer)); 
	for (auto ii = 0; ii < AES_BLOCKLEN; ii++){	
	  CT_InBuffer[i+ii] =   CT_buffer2[ii];
	}
  }
}



static void test_xcrypt_ctr(const string xcrypt)
{

const unsigned int NBUF = 4;


 uint8_t *in;
 uint8_t *out;
	
#if defined(AES256)
    uint8_t key[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t good_in[64]  = { 0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5, 0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28, 
                        0xf4, 0x43, 0xe3, 0xca, 0x4d, 0x62, 0xb5, 0x9a, 0xca, 0x84, 0xe9, 0x90, 0xca, 0xca, 0xf5, 0xc5, 
                        0x2b, 0x09, 0x30, 0xda, 0xa2, 0x3d, 0xe9, 0x4c, 0xe8, 0x70, 0x17, 0xba, 0x2d, 0x84, 0x98, 0x8d, 
                        0xdf, 0xc9, 0xc5, 0x8d, 0xb6, 0x7a, 0xad, 0xa6, 0x13, 0xc2, 0xdd, 0x08, 0x45, 0x79, 0x41, 0xa6 };
#elif defined(AES192)
    uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 
                        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t good_in[64]  = { 0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2, 0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b, 
                        0x09, 0x03, 0x39, 0xec, 0x0a, 0xa6, 0xfa, 0xef, 0xd5, 0xcc, 0xc2, 0xc6, 0xf4, 0xce, 0x8e, 0x94, 
                        0x1e, 0x36, 0xb2, 0x6b, 0xd1, 0xeb, 0xc6, 0x70, 0xd1, 0xbd, 0x1d, 0x66, 0x56, 0x20, 0xab, 0xf7, 
                        0x4f, 0x78, 0xa7, 0xf6, 0xd2, 0x98, 0x09, 0x58, 0x5a, 0x97, 0xda, 0xec, 0x58, 0xc6, 0xb0, 0x50 };
#elif defined(AES128)
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t good_in[64]  = { 0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26, 0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
                        0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff, 0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
                        0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e, 0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
                        0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1, 0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee };
#endif
    uint8_t iv[16]  = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
    uint8_t good_out[64] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };

	printf("Nb :%d \n", Nb);
	printf("Nr :%d \n", Nr);
	printf("Nk :%d \n", Nk);
	printf("AES_KEYLEN :%d \n", AES_KEYLEN);
	printf("AES_keyExpSize :%d \n", AES_keyExpSize);

	if (xcrypt.compare("encrypt") == 0){
	  in = good_in;
	  out = good_out;
	} else {
	  in = good_out;
	  out = good_in;
	}		
    // print text to encrypt, key and IV
    cout << "CTR " << xcrypt << endl<< endl;

    printf("in:\n");
	unsigned char j=0;
	
	for (unsigned char i = 0; i < NBUF; i++) {
	  phex16(&in[j]);
	  j+=AES_BLOCKLEN;
	}
	
    printf("\n");

    printf("key:\n");
    phex(key);
    printf("\n");

    printf("iv:\n");
    phex16(iv);
    printf("\n");

	//cannot manipulate subfields of a CT struct right now
	//so instead of using AES_ctx we need to work direct with it's subfields

	EncodedArray<uint8_t, AES_KEYLEN> CT_key;
	CT_key.Encode(key);

	EncodedArray<uint8_t, AES_BLOCKLEN> CT_iv;
	CT_iv.Encode(iv);

	EncodedArray<uint8_t, AES_keyExpSize> CT_roundkey;
	
	//note we do not need to use AES_init_ctx_iv, as iv is kept separate (no CTX)
    XLS_CHECK_OK(AES_init_ctx(CT_roundkey, CT_key));

	uint8_t out_roundkey[AES_keyExpSize]={0};
	CT_roundkey.Decode(out_roundkey);
    printf("round trip roundkey:\n");
	phex(out_roundkey);
    printf("\n");

	uint8_t out_iv[AES_keyExpSize]={0};
	CT_iv.Decode(out_iv);
    printf("round trip iv:\n");
	phex16(out_iv);
    printf("\n");


	EncodedArray<uint8_t, MSG_SIZE> CT_inbuffer;
	
	CT_inbuffer.Encode(in);

	AES_CTR_xcrypt_buffer(CT_roundkey, CT_iv, CT_inbuffer, MSG_SIZE);
	

	uint8_t dec_ct_out[MSG_SIZE];
	
	CT_inbuffer.Decode(dec_ct_out);
	cout<< "out "<< ":" <<endl;

	j=0;
	for (unsigned char i = 0; i < NBUF; i++) {
	  phex16(&dec_ct_out[j]);
	  j+=AES_BLOCKLEN;
	}
	
	if (0 == memcmp((char*) out, (char*) dec_ct_out, 16)) {
	  printf("SUCCESS!\n");
	} else {
	  printf("FAILURE!\n");
	}
	return;
}

int main(int argc, char** argv) {


#if defined(AES256)
  printf("\nTesting AES256\n\n");
#elif defined(AES192)
  printf("\nTesting AES192\n\n");
#elif defined(AES128)
  printf("\nTesting AES128\n\n");
#else
  printf("You need to specify a compile time symbol in aes.h between AES128, AES192 or AES256. Exiting");
  return 0;
#endif
  
  //run CTR
  test_xcrypt_ctr("encrypt");

  test_xcrypt_ctr("decrypt");
  
  cout << "Done" << endl;
  
  return 1;
}
