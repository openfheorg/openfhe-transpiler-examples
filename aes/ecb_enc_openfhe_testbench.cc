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


#define PROFILE // need to define this for TIC,TOC to work
#include "openfhe/binfhe/binfhecontext.h"
#include "binfhecontext-ser.h"

//#include "palisade/utils/debug.h"
#include "transpiler/data/openfhe_data.h"
#include "xls/common/logging/logging.h"

#include "aes.h"


#ifdef USE_INTERPRETED_OPENFHE
#include "transpiler/examples/aes/AES_init_ctx_interpreted_openfhe.h"
#include "transpiler/examples/aes/AddRoundKey_interpreted_openfhe.h"
#include "transpiler/examples/aes/SubBytes_interpreted_openfhe.h"
#include "transpiler/examples/aes/ShiftRows_interpreted_openfhe.h"
#include "transpiler/examples/aes/MixColumns_interpreted_openfhe.h"

#elif defined (USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/aes/AES_init_ctx_yosys_interpreted_openfhe.h"
#include "transpiler/examples/aes/AddRoundKey_yosys_interpreted_openfhe.h"
#include "transpiler/examples/aes/SubBytes_yosys_interpreted_openfhe.h"
#include "transpiler/examples/aes/ShiftRows_yosys_interpreted_openfhe.h"
#include "transpiler/examples/aes/MixColumns_yosys_interpreted_openfhe.h"
#else

#include "transpiler/examples/aes/AES_init_ctx_openfhe.h"
#include "transpiler/examples/aes/AddRoundKey_openfhe.h"
#include "transpiler/examples/aes/SubBytes_openfhe.h"
#include "transpiler/examples/aes/ShiftRows_openfhe.h"
#include "transpiler/examples/aes/MixColumns_openfhe.h"

#endif

using namespace lbcrypto;
using namespace std;

BinFHEContext cc; //used everywhere so global
LWEPrivateKey sk; //used everywhere so global

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


// Cipher is the main function that encrypts the PlainText.
void Cipher(OpenFheArrayRef<uint8_t, SDIMX, SDIMY> CT_state, const OpenFheArrayRef<uint8_t, AES_keyExpSize> CT_RoundKey)
{
  DEBUG_FLAG(false);
  uint8_t round = 0;
  OpenFhe<uint8_t> CT_round(cc);
  
  OpenFheArray<uint8_t, 16> CT_Debug(cc); 
  TimeVar t;
  double execTime(0.0);
  CT_round.SetEncrypted(round, sk);
  // Add the First round key to the state before starting the rounds.
  DEBUG("Round 0   AddRoundKey");
  
  TIC(t);
  XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug, cc));
  execTime = TOC_MS(t);
  cout << "AddRoundKey "<<(unsigned int) round<< " time: " << execTime/1000.0 << " s" << std::endl;
  debug_phex(CT_state, dbg_flag);
  DEBUG("key ");
  debug_phex16(CT_Debug, dbg_flag);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without MixColumns()
  //for (round = 1; round < Nr+2; ++round)

  for (round = 1; ; ++round)
  {
	dbg_flag = false;
	DEBUG("Round "<< (unsigned int)round);

	DEBUG("SubBytes " );
	TIC(t);
	XLS_CHECK_OK(SubBytes(CT_state, cc));
	execTime = TOC_MS(t);
	cout << "SubBytes "<<(unsigned int) round<< " time: " << execTime/1000.0 << " s" << std::endl;
	debug_phex(CT_state, dbg_flag);
	DEBUG("ShiftRows ");
	TIC(t);
	XLS_CHECK_OK(ShiftRows(CT_state, cc));
	execTime = TOC_MS(t);
	cout << "ShiftRows "<<(unsigned int)round<< " time: " << execTime/1000.0 << " s" << std::endl;
	debug_phex(CT_state, dbg_flag);
    if (round == Nr) {
      break;
    }
	DEBUG("MixColumns ");

	TIC(t);
	XLS_CHECK_OK(MixColumns(CT_state, cc));
	execTime = TOC_MS(t);
	cout << "MixColumns "<<(unsigned int) round<< " time: " << execTime/1000.0 << " s" << std::endl;
	debug_phex(CT_state, dbg_flag);
	CT_round.SetEncrypted(round, sk);
	DEBUG("round again " <<(unsigned int)CT_round.Decrypt(sk));
	DEBUG("AddRoundKey ");
	TIC(t);
	XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug, cc));
	execTime = TOC_MS(t);
	cout << "AddRoundKey "<<(unsigned int)round<< " time: " << execTime/1000.0 << " s" << std::endl;
	debug_phex(CT_state, dbg_flag);
	//printf("key ");debug_phex16(CT_Debug);
  }
  // Add round key to last round
  CT_round.SetEncrypted(Nr, sk);
  DEBUG("AddRoundKey last round");
  TIC(t);
  XLS_CHECK_OK(AddRoundKey(CT_round, CT_state, CT_RoundKey, CT_Debug, cc));
  execTime = TOC_MS(t);
  debug_phex(CT_state, dbg_flag);
  cout << "AddRoundKey "<<(unsigned int)Nr<< " time: " << execTime/1000.0 << " s" << std::endl;
}

void AES_ECB_encrypt(OpenFheArrayRef<uint8_t, AES_keyExpSize> CT_RoundKey, OpenFheArrayRef<uint8_t, SDIMX, SDIMY> CT_buffer)
{
  // The next function call encrypts the PlainText with the Key using AES algorithm.
   Cipher(CT_buffer, CT_RoundKey);
}

static void test_encrypt_ecb(void)
{
    uint8_t i;

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
    
	OpenFhe<AES_ctx> CT_ctx(cc);
	OpenFheArray<uint8_t, AES_KEYLEN> CT_key(cc);
	CT_key.SetEncrypted(key, sk);

	cout << "AES_init_ctx start" << endl;
	TimeVar t;
	double execTime(0.0);
	TIC(t);
    XLS_CHECK_OK(AES_init_ctx(CT_ctx, CT_key, cc));
	execTime = TOC_MS(t);
	cout << "AES_init_ctx time: " << execTime/1000.0 << " s" << std::endl;

	struct AES_ctx out_ctx;
	out_ctx = CT_ctx.Decrypt(sk);
    printf("round trip key:\n");
	phex(out_ctx.RoundKey);
    printf("\n");

	//we may need to rewrite init_ctx so it returns just the roundkey
	OpenFheArray<uint8_t, AES_keyExpSize> CT_roundkey(cc);
	CT_roundkey.SetEncrypted(out_ctx.RoundKey, sk);

#else //so modified this to work directly with the roundkey rather than ctx

	OpenFheArray<uint8_t, AES_KEYLEN> CT_key(cc);
	CT_key.SetEncrypted(key, sk);

	OpenFheArray<uint8_t, AES_keyExpSize> CT_roundkey(cc);
	
	cout << "AES_init_ctx start" << endl;
	TimeVar t;
	double execTime(0.0);
	TIC(t);
        XLS_CHECK_OK(AES_init_ctx(CT_roundkey, CT_key, cc));
        execTime = TOC_MS(t);
	cout << "AES_init_ctx time: " << execTime/1000.0 << " s" << std::endl;

	uint8_t out_roundkey[AES_keyExpSize]={0};
	CT_roundkey.Decrypt(out_roundkey, sk);
    printf("round trip roundkey:\n");
	phex(out_roundkey);
    printf("\n");
 

#endif	

	
	state_t pt_in;
	// OpenFhe<state_t> CT_pt_in;	 // does not work..
	OpenFheArray<uint8_t, SDIMX, SDIMY> CT_pt_in(cc);
	auto k = 0;
	auto kk = 0;
	for (auto i = 0; i < 1; ++i)     { // only currently have one ciphertext
	  for (auto ii = 0; ii< SDIMX; ii++) {
		for (auto jj = 0; jj< SDIMY; jj++) {
		  pt_in[ii][jj] = in[k++];
		}
	  }
	  //cout<< "in "<< i<< ":" <<endl;
      //phex(plain_text + (i * 16));	  

	  CT_pt_in.SetEncrypted(pt_in, sk);	
	  TIC(t);	
	  AES_ECB_encrypt(CT_roundkey, CT_pt_in);
	  execTime = TOC_MS(t);
	  cout << "AES_ECB_encrypt pass "<<i<< " time: " << execTime/1000.0 << " s" << std::endl;

	  //out_ctx = CT_ctx.Decrypt(sk);

	  uint8_t dec_ct_out[SDIMX][SDIMY];
          uint8_t dec_out[SDIMX*SDIMY];

	  CT_pt_in.Decrypt(dec_ct_out, sk);
	  for (auto ii = 0; ii< SDIMX; ii++) {
		for (auto jj = 0; jj< SDIMY; jj++) {
		  dec_out[kk++] = dec_ct_out[ii][jj];
		}
	  }
	cout<< "out "<< i<< ":" <<endl;
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
  printf("You need to specify a compile time symbol in aes.h between AES128, AES192 or AES256. Exiting");
  return 0;
#endif

  cc = BinFHEContext(); //was auto used everywhere

  bool fail(false);
  if (argc > 1) {
	
	std::string input = argv[1];
	if (input.compare("-h") == 0){
	  fail = true;
	} else 	if (input.compare("TOY") == 0){
	  cc.GenerateBinFHEContext(TOY); //strictly for debugging
	  cout << "using unsecure TOY settings" <<endl;
	} else 	if (input.compare("MEDIUM") == 0){
	  //note MEDIUM s >= 100 bits security
	  cc.GenerateBinFHEContext(MEDIUM);
	  cout << "using MEDIUM settings" <<endl;
	} else if (input.compare("STD128_LMKCDEY") == 0){
	  cc.GenerateBinFHEContext(STD128_LMKCDEY);
	  cout << "using STD128_LMKCDEY settings" <<endl;
	} else if (input.compare("STD128Q_LMKCDEY") == 0){
	  //STD128Q is 128 bit quantum , 192 and 256 are supported.
	  //remove Q to limit to classical attacks.
	  cc.GenerateBinFHEContext(STD128Q_LMKCDEY); //may need 64 bit arch. 
	  cout << "using STD128Q_LMKCDEY settings" <<endl;
	} else {  
	  fail = true;
	}
  } else {
	//default to toy
	
	cc.GenerateBinFHEContext(TOY); //strictly for debugging
	cout << "using unsecure TOY settings" <<endl;
  }
  
  if (fail) {
	cerr<< "Usage: " << argv[0] << "TOY (defult) | MEDIUM | STD128_LMKCDEY | STD128Q_LMKCDEY"  << endl;
	return EXIT_FAILURE;
  }

  std::cout << "Generating the secret key..." << std::endl;

  // Generate the secret key
  sk = cc.KeyGen();

  std::cout << "Generating the bootstrapping keys..." << std::endl;

  // Generate the bootstrapping keys (refresh and switching keys)
  cc.BTKeyGen(sk);

  std::cout << "Completed key generation." << std::endl;

  
  //run ECB
  
  test_encrypt_ecb();
  cout << "Done" << endl;
  
  return 1;
}
