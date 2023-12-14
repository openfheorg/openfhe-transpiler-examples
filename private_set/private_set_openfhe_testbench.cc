// Copyright 2022 Duality Technologies Inc.
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

// private set union and intersection. Simulates a chain of nodes conducting this
// operation  each node takes old and new sets as input outputs updated old

// intersection and union code is taken from https://www.tutorialspoint.com/cplusplus-program-to-find-union-and-intersection-of-two-unsorted-arrays


#include <stdint.h>
#include <iostream>
#include "private_set.h"

#define PROFILE // need to define this for TIC,TOC to work
#include "openfhe/binfhe/binfhecontext.h"
#include "binfhecontext-ser.h"

#include "transpiler/data/openfhe_data.h"
#include "xls/common/logging/logging.h"

#include "parse_inputs.h" // these must be after the openfhe includes to compile
#include "generate_data.h"


#ifdef USE_INTERPRETED_OPENFHE
#include "transpiler/examples/private_set/intersect_interpreted_openfhe.h"
#include "transpiler/examples/private_set/union_interpreted_openfhe.h"
#include "transpiler/examples/private_set/copy_outset_inc_flag_interpreted_openfhe.h"
#include "transpiler/examples/private_set/set_length_interpreted_openfhe.h"


#elif defined(USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/private_set/intersect_yosys_interpreted_openfhe.h"
#include "transpiler/examples/private_set/union_yosys_interpreted_openfhe.h"
#include "transpiler/examples/private_set/copy_outset_inc_flag_yosys_interpreted_openfhe.h"
#include "transpiler/examples/private_set/set_length_yosys_interpreted_openfhe.h"
#else
#include "transpiler/examples/private_set/intersect_openfhe.h"
#include "transpiler/examples/private_set/union_openfhe.h"
#include "transpiler/examples/private_set/copy_outset_inc_flag_openfhe.h"
#include "transpiler/examples/private_set/set_length_openfhe.h"
#endif

using namespace lbcrypto;
using namespace std;

BinFHEContext cc; //used everywhere so global
LWEPrivateKey sk; //used everywhere so global

/////////////////////////////////////////////////////////////////////////////
// utility function to decrypt, print SetStruct for debug
void print_CT(bool verbose, OpenFheRef<SetStruct> CT,
				  string msg, unsigned int count) {
  if (verbose) {
	auto test = CT.Decrypt(sk);
	cout << msg << " " << count
		 << " overflow " << test.overflow
		 << " setLength " << (unsigned int) test.setLength << endl;
	for (unsigned char j = 0; j< test.setLength; j++) {
	  cout << " " <<  (unsigned int) test.set[j];
	}
	cout << endl;
  }
}  

/////////////////////////////////////////////////////////////////////////////
// utility function to decrypt, print and verify SetStruct for debug
void print_CT_verify(bool verbose,
				  OpenFheRef<SetStruct> CT,
				  SetStruct validSet, string msg, unsigned int count) {
  if (verbose) {
	print_CT(verbose, CT, msg, count);

	bool good(false);
	auto testSet = CT.Decrypt(sk);
	// verify set
	good = verify_set(testSet, validSet);
	cout << msg <<" " <<count<<": "
		 << (good?"passed":"failed") << endl;
  }
}


/////////////////////////////////////////////////////////////////////////////
//outer loop of intersection calls encrypted inner loop
OpenFheRef<SetStruct> intersect_outer(OpenFheRef<SetStruct> CT_oldset,
									  OpenFheRef<SetStruct> CT_newset,
									  bool debug) {

  if (debug) {
	cout << "=========="<<endl;
  }
  unsigned char flag(0);

  OpenFhe <unsigned char> CT_flag(cc);
  CT_flag.SetEncrypted(flag, sk);
  
  //////////////////////////////
  if (debug) {
	print_CT(debug, CT_oldset, "E initial oldset", 0);
	cout << " input flag " << (unsigned int ) flag << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  /////////////////////
  
  for(unsigned char q = 0; q < MAX_SET_LEN; q++) {

	OpenFhe <unsigned char> CT_q(cc);
	CT_q.SetEncrypted(q, sk);

	//need to pass in outSet not return it from this
	SetStruct outSet {false, 0, {0}};
	outSet.overflow = false;
	OpenFhe <SetStruct> CT_outset(cc);	
	CT_outset.SetEncrypted(outSet, sk);

	XLS_CHECK_OK(intersect_inner(CT_outset, CT_q, CT_oldset, CT_newset, CT_flag, cc));
	CT_oldset = CT_outset;
	
	///////////////////////////
	if (debug) {
	  print_CT(debug, CT_outset, "E loop q ", (unsigned int) q);
	  cout << " flag " << (unsigned int ) CT_flag.Decrypt(sk) << endl;
	}
	/////////////////////////////
  }
  if (debug) {
	print_CT(debug, CT_oldset, "E exiting oldSet ", 0);
	cout << " CT flag " << (unsigned int ) CT_flag.Decrypt(sk) << endl;
  }
  /////////////////////////////

  return CT_oldset;
}
/////////////////////////////////////////////////////////
//outer loop of union calls encrypted inner loop
OpenFheRef<SetStruct> union_outer(OpenFheRef<SetStruct> CT_outset,
								  OpenFheRef<SetStruct> CT_oldset,
								  OpenFheRef<SetStruct> CT_newset,
								  bool debug){

  if (debug) {
	cout << "=========="<<endl;
  }
  unsigned char flag(0);
  OpenFhe <unsigned char> CT_flag(cc);
  CT_flag.SetEncrypted(flag, sk);


  //////////////////////////////
 if (debug) {
	print_CT(debug, CT_oldset, "E initial oldset", 0);
	cout << " input flag " << (unsigned int ) flag << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  
  // copy outSet from oldSet and increment flag
  XLS_CHECK_OK(copy_outset_inc_flag(CT_outset, CT_oldset, CT_flag, cc));

  //////////////////////////////
  if (debug) {
	print_CT(debug, CT_outset, "E initial outset", 0);
	cout << " flag " << (unsigned int ) CT_flag.Decrypt(sk) << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  /////////////////////
  
  for(unsigned char k = 0; k < MAX_SET_LEN; k++) {
	OpenFhe <unsigned char> CT_k(cc);
	CT_k.SetEncrypted(k, sk);

	XLS_CHECK_OK(union_inner(CT_outset, CT_k, CT_oldset, CT_newset, CT_flag, cc));

	///////////////////////////
	if (debug) {
	  print_CT(debug, CT_outset, "E loop outset ", (unsigned int) k);
	  cout << " flag " << (unsigned int ) CT_flag.Decrypt(sk) << endl;
	}
	/////////////////////////////
  }

  XLS_CHECK_OK(set_length(CT_outset, CT_flag, cc));
  ///////////////////////////
  if (debug) {
	print_CT(debug, CT_outset, "E final outset ", 0);
	cout << " flag " << (unsigned int ) CT_flag.Decrypt(sk) << endl;
	cout << "=========="<<endl;
  }
  /////////////////////////////
  return CT_outset;
}
/////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

  // default values of command line parameters
  bool verbose(false);
  bool debug(false);
  bool common_flag(false);
  unsigned int n_neighbors(MAX_NEIGHBORS);
  string out_fname;
  unsigned int seed(1);

  lbcrypto::BINFHE_PARAMSET param_set(lbcrypto::TOY);
  string param_set_name("TOY");
  
  TimeVar t;					// for timing
  TimeVar tTot;					// for timing

  cout << "Private Set running maximum "
	   << (unsigned int) MAX_NEIGHBORS << "neighbors" <<endl;

  parse_inputs(argc, argv, verbose, debug, common_flag, seed,
               param_set, param_set_name,
               n_neighbors, out_fname);
  
  ofstream outfile; //file for logging output
  ofstream sizeoutfile; //file for logging size output
  try {
	outfile.open(out_fname, ios::out | ios::trunc );
  } catch (const exception& e) {
    cerr << out_fname << " file open exception was caught, with message '"
		 << e.what() << "'\n";
  }
  try {
	sizeoutfile.open(out_fname+".siz", ios::out | ios::trunc );
  } catch (const exception& e) {
    cerr << out_fname+".siz"
		 << " file open exception was caught, with message '"
		 << e.what() << "'\n";
  }

  outfile << "\"param\",\"n_neighbors\",\"MAX_SET_LEN\",\"intersect ave. time (ms)\",\"intersect s.d. time (ms)\",\"union ave. time (ms)\",\"union s.d. time (ms)\", " << endl;

  outfile << param_set_name << "," << n_neighbors << ","
		  << (unsigned int) MAX_SET_LEN << ",";

  sizeoutfile << "Using BinFHE "<<param_set_name << endl;
  //generate FHE context and keys 
  if (verbose) {
    cout<< "Using BinFHE "<<param_set_name << endl;
  }

  cc = BinFHEContext();
  cc.GenerateBinFHEContext(param_set);

  cout << "Generating the secret key..." << endl;

  sk = cc.KeyGen();		// Generate the secret key

  {
	// measure the size of the secret key
	ostringstream keystring;
	lbcrypto::Serial::Serialize(sk, keystring, SerType::BINARY);
	if (!keystring.str().size())
	   cout << "Serialized eval sum key is empty" << endl;
    sizeoutfile << "Secret key size is " << keystring.str().size()
				<< " bytes" << endl;

  }

  cout << "Generating the bootstrapping keys..." << endl;
  cc.BTKeyGen(sk);				// Generate the bootstrapping keys
								//(refresh and switching keys)
  cout << "Completed key generation." << endl;

  // measure the size of a switching key
  ostringstream switchstring;
  auto switchkey= cc.GetSwitchKey();
  
  lbcrypto::Serial::Serialize(switchkey, switchstring, SerType::BINARY);
  if (!switchstring.str().size()) {
	 cout << "Serialized switchkey is empty" << endl;
  } else {
	sizeoutfile << "switchkey size is " << switchstring.str().size()
				<< " bytes" << endl;
  }
  // measure the size of a refresh key
  ostringstream refreshstring;
  auto refreshkey= cc.GetRefreshKey();
  
  lbcrypto::Serial::Serialize(refreshkey, refreshstring, SerType::BINARY);
  if (!refreshstring.str().size()) {
	 cout << "Serialized refreshkey is empty" << endl;
  } else {
	sizeoutfile << "refreshkey size is " << refreshstring.str().size()
				<< " bytes" << endl;
  }
  size_t CTsize;

  // measure the size of a one bit ciphertext
  ostringstream CTstring;
  auto ct = cc.Encrypt(sk, 1);
  
  lbcrypto::Serial::Serialize(ct, CTstring, SerType::BINARY);
  CTsize = CTstring.str().size();
  if (!CTsize) {
	 cout << "Serialized ciphertext is empty" << endl;
  }
  sizeoutfile << "1 bit CT size is " << CTsize  << " bytes" << endl;


  ////////////////////////////////////////////////////////////////////
  // create inputs.
  SetStruct sets[MAX_NEIGHBORS]; // max storage

  SetStruct goodIntersectOutput = {false, 0, {0}};
  SetStruct goodUnionOutput = {false, 0, {0}};

  generate_data(n_neighbors, verbose, common_flag, seed,
				sets, goodIntersectOutput, goodUnionOutput);

  // loop over two passes, the first for intersection, the second for union
  for (auto pass = 0; pass < 2; pass++){

	double execTime(0.0);
	double execTimeTot(0.0);
	double execTimeSqTot(0.0);
	unsigned int n_iter(0);
	
	if (pass == 0) {
	  cout << "Computing intersection" << endl;
	} else {
	  cout << "Computing union" << endl;
	}

	OpenFhe<SetStruct> CT_oldset(cc); 
	OpenFhe<SetStruct> CT_newset(cc); 

	SetStruct oldset;
	SetStruct newset;

	// Encrypt first set and save it as oldset
	CT_oldset.SetEncrypted(sets[0], sk);

	sizeoutfile << "bit_width of input CT_oldset is "
				<< CT_oldset.bit_width() <<endl;
	sizeoutfile << "bytes: "<< CT_oldset.bit_width() * CTsize <<endl;

	oldset = sets[0];
	
	if (verbose) { //verify initial encryption is valid
	  bool good(false);
	  SetStruct input_round_trip;
	  input_round_trip = CT_oldset.Decrypt(sk);
	  
	  // verify round trip of input
	  good = verify_set(input_round_trip, sets[0]);
	  cout << "Round trip of input set 0: "<< (good?"passed":"failed") << endl;
	}

	//loop over the remaining sets
	for(auto ix = 1; ix < n_neighbors; ix++) {
	  
	  // Encode data
	  CT_newset.SetEncrypted(sets[ix], sk);
	  newset = sets[ix];

	  //verify initial encryption is valid
	  print_CT_verify(verbose, CT_newset, sets[ix], "Round trip of input set", ix );

	  OpenFhe <SetStruct> CT_outset(cc);

	  // operate on sets: Oldset = OP(Oldset, NewSet)
	  if (pass == 0) {
		TIC(t);
		CT_oldset = intersect_outer(CT_oldset, CT_newset, debug);
		execTime = TOC_MS(t);;
		execTimeTot += execTime;
		execTimeSqTot += execTime*execTime;
		n_iter++;
	  } else {
		TIC(t);
		CT_oldset = union_outer(CT_outset, CT_oldset, CT_newset, debug);
		execTime = TOC_MS(t);;
		execTimeTot += execTime;
		execTimeSqTot += execTime*execTime;
		n_iter++;
	  }
	  cout << "iteration " << ix << " time: " << execTime << " mSec"<<endl;
	  print_CT(verbose, CT_oldset, "resulting set pass ", ix);
	}

	//oldset now contains the result set
	auto dec_oldset = CT_oldset.Decrypt(sk);	  
	bool good(false);
	if (pass == 0) {
	  cout << "verifying intersection: "; 		
	  good = verify_set(dec_oldset, goodIntersectOutput);
	} else {
	  cout << "verifying union: "; 		
	  good = verify_set(dec_oldset, goodUnionOutput);
	}
	cout << (good?"passed":"failed") << endl;
  
	//compute average constrain and select times
	if (n_iter) {
	  execTimeTot /= (float)n_iter;
	  execTimeSqTot /= (float)n_iter;
	}
	cout << "average time: " << execTimeTot/1000.0 << " s" << endl;
	
	auto sdTime = sqrt( execTimeSqTot - execTimeTot*execTimeTot);
	cout << "s.d.: " << sdTime/1000.0 << " s" << endl;  
	outfile << execTimeTot << "," << sdTime << ",";	
  }

  outfile <<endl;
  
  outfile.close();// close the output file
  
  sizeoutfile.close();// close the size output file
  cout << "Done" << endl;
  exit(0);
}
