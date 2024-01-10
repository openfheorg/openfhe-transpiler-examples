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
#else

#include "transpiler/examples/aes/AES_init_ctx_cleartext.h"
#include "transpiler/examples/aes/AddRoundKey_cleartext.h"
#include "transpiler/examples/aes/SubBytes_cleartext.h"
#include "transpiler/examples/aes/ShiftRows_cleartext.h"
#include "transpiler/examples/aes/MixColumns_cleartext.h"

#endif

using namespace std;


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
		
static void debug_phex(OpenFheArrayRef<uint8_t, SDIMX, SDIMY> CT_state,
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


static void debug_phex16(OpenFheArrayRef<uint8_t, 16> CT_debug, bool dbg_flag){

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

void AES_ECB_encrypt(EncodedArrayRef<uint8_t, AES_keyExpSize> CT_RoundKey, EncodedArrayRef<uint8_t, SDIMX, SDIMY> CT_buffer)
{
  // The next function call encrypts the PlainText with the Key using AES algorithm.
   Cipher(CT_buffer, CT_RoundKey);
}

static void test_encrypt_ecb(void)
{
  //    uint8_t i;

#if defined(AES256)
    uint8_t key[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                      0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t out[] = { 0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8 };
#elif defined(AES192)
    uint8_t key[] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                      0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t out[] = { 0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc };
#elif defined(AES128)
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t out[] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };
#endif

    uint8_t in[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };


	printf("Nb :%d \n", Nb);
	printf("Nr :%d \n", Nr);
	printf("Nk :%d \n", Nk);
	printf("AES_KEYLEN :%d \n", AES_KEYLEN);
	printf("AES_keyExpSize :%d \n", AES_keyExpSize);


	
    // print text to encrypt, key and IV
    printf("ECB encrypt:\n\n");
    printf("plain text:\n");
	phex16(in);
    printf("\n");

    printf("key:\n");
    phex(key);
    printf("\n");

#if 0 //cannot manipulate subfields of a CT struct right now   
	Encoded<AES_ctx> CT_ctx;
	EncodedArray<uint8_t, AES_KEYLEN> CT_key;
	CT_key.Encode(key);
	
    XLS_CHECK_OK(AES_init_ctx(CT_ctx, CT_key));
	struct AES_ctx out_ctx;
	out_ctx = CT_ctx.Decode();
    printf("round trip key:\n");
	phex(out_ctx.RoundKey);
    printf("\n");

	//we may need to rewrite init_ctx so it returns just the roundkey
	EncodedArray<uint8_t, AES_keyExpSize> CT_roundkey;
	CT_roundkey.Encode(out_ctx.RoundKey);

#else //so modified this to work directly with the roundkey rather than ctx

	EncodedArray<uint8_t, AES_KEYLEN> CT_key;
	CT_key.Encode(key);

	EncodedArray<uint8_t, AES_keyExpSize> CT_roundkey;
	
    XLS_CHECK_OK(AES_init_ctx(CT_roundkey, CT_key));

	uint8_t out_roundkey[AES_keyExpSize]={0};
	CT_roundkey.Decode(out_roundkey);
    printf("round trip roundkey:\n");
	phex(out_roundkey);
    printf("\n");
 

#endif	
    // print the resulting cipher as 4 x 16 byte strings
    printf("ciphertext:\n");
	
	state_t pt_in;
	// Encoded<state_t> CT_pt_in;	 // does not work..
	EncodedArray<uint8_t, SDIMX, SDIMY> CT_pt_in;
	auto k = 0;
	auto kk = 0;
	for (auto i = 0; i < 1; ++i)     { // only currently have one ciphertext
	  for (auto ii = 0; ii< SDIMX; ii++) {
		for (auto jj = 0; jj< SDIMY; jj++) {
		  pt_in[ii][jj] = in[k++];
		}
	  }
	  
	  CT_pt_in.Encode(pt_in);
	  
	  AES_ECB_encrypt(CT_roundkey, CT_pt_in);
	  
	  //out_ctx = CT_ctx.Decode();
	  
	  uint8_t dec_ct_out[SDIMX][SDIMY];
	  uint8_t dec_out[SDIMX*SDIMY];
	  
	  CT_pt_in.Decode(dec_ct_out);
	  for (auto ii = 0; ii< SDIMX; ii++) {
		for (auto jj = 0; jj< SDIMY; jj++) {
		  dec_out[kk++] = dec_ct_out[ii][jj];
		}
	  }
	  cout<< "out "<< ":" <<endl;
	  phex16(dec_out);
		

		if (0 == memcmp((char*) out, (char*) dec_ct_out, 16)) {
		  printf("SUCCESS!\n");
		} else {
		  printf("FAILURE!\n");
		}
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
  printf("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
  return 0;
#endif
  
  test_encrypt_ecb();
  
  //run ECB
  
  cout << "Done" << endl;
  
  return 1;
}
