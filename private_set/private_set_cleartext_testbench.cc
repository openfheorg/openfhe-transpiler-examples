
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
#include "parse_inputs.h"
#include "generate_data.h"

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/private_set/intersect_yosys_interpreted_cleartext.h"
#include "transpiler/examples/private_set/union_yosys_interpreted_cleartext.h"
#include "transpiler/examples/private_set/copy_outset_inc_flag_yosys_interpreted_cleartext.h"
#include "transpiler/examples/private_set/set_length_yosys_interpreted_cleartext.h"
#else
#include "transpiler/examples/private_set/intersect_cleartext.h"
#include "transpiler/examples/private_set/union_cleartext.h"
#include "transpiler/examples/private_set/copy_outset_inc_flag_cleartext.h"
#include "transpiler/examples/private_set/set_length_cleartext.h"

#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// utility function to decode, print SetStruct for debug
void print_CT(bool verbose, EncodedRef<SetStruct> CT,
				  std::string msg, unsigned int count) {
  if (verbose) {
	auto test = CT.Decode();
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
// utility function to decode, print and verify SetStruct for debug
void print_CT_verify(bool verbose,
				  EncodedRef<SetStruct> CT,
				  SetStruct validSet, std::string msg, unsigned int count) {
  if (verbose) {
	print_CT(verbose, CT, msg, count);

	bool good(false);
	auto testSet = CT.Decode();
	// verify set
	good = verify_set(testSet, validSet);
	cout << msg <<" " <<count<<": "
		 << (good?"passed":"failed") << endl;
  }
}

/////////////////////////////////////////////////////////////////////////////
//outer loop of intersection calls encrypted inner loop
EncodedRef<SetStruct> intersect_outer(EncodedRef<SetStruct> CT_oldset,
									  EncodedRef<SetStruct> CT_newset,
									  bool debug) {

  if (debug) {
	cout << "=========="<<endl;
  }
  unsigned char flag(0);
  Encoded <unsigned char> CT_flag;
  CT_flag.Encode(flag);
  
  //////////////////////////////
  if (debug) {
	print_CT(debug, CT_oldset, "E initial oldset", 0);
	cout << " input flag " << (unsigned int ) flag << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  /////////////////////
  
  for(unsigned char q = 0; q < MAX_SET_LEN; q++) {

	Encoded <unsigned char> CT_q;
	CT_q.Encode(q);

	//need to pass in outSet not return it from this
	SetStruct outSet {false, 0, {0}};
	outSet.overflow = false;
	Encoded <SetStruct> CT_outset;	
	CT_outset.Encode(outSet);

	XLS_CHECK_OK(intersect_inner(CT_outset, CT_q, CT_oldset, CT_newset, CT_flag));
	CT_oldset = CT_outset;
	
	///////////////////////////
	if (debug) {
	  print_CT(debug, CT_outset, "E loop q ", (unsigned int) q);
	  cout << " flag " << (unsigned int ) CT_flag.Decode() << endl;
	}
	/////////////////////////////
  }
  if (debug) {
	print_CT(debug, CT_oldset, "E exiting oldSet ", 0);
	cout << " CT flag " << (unsigned int ) CT_flag.Decode() << endl;
  }
  /////////////////////////////

  return CT_oldset;
}
/////////////////////////////////////////////////////////
//outer loop of union calls encrypted inner loop
EncodedRef<SetStruct> union_outer(EncodedRef<SetStruct> CT_outset,
								  EncodedRef<SetStruct> CT_oldset,
								  EncodedRef<SetStruct> CT_newset,
								  bool debug){

  if (debug) {
	cout << "=========="<<endl;
  }
  unsigned char flag(0);
  Encoded <unsigned char> CT_flag;
  CT_flag.Encode(flag);


  //////////////////////////////
  if (debug) {
	print_CT(debug, CT_oldset, "E initial oldset", 0);
	cout << " input flag " << (unsigned int ) flag << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  
  // copy outSet from oldSet and increment flag
  XLS_CHECK_OK(copy_outset_inc_flag(CT_outset, CT_oldset, CT_flag));

  //////////////////////////////
  if (debug) {
	print_CT(debug, CT_outset, "E initial outset", 0);
	cout << " flag " << (unsigned int ) CT_flag.Decode() << endl;
	print_CT(debug, CT_newset, "E initial newset", 0);
  }
  /////////////////////
  
  for(unsigned char k = 0; k < MAX_SET_LEN; k++) {
	Encoded <unsigned char> CT_k;
	CT_k.Encode(k);

	XLS_CHECK_OK(union_inner(CT_outset, CT_k, CT_oldset, CT_newset, CT_flag));

	///////////////////////////
	if (debug) {
	  print_CT(debug, CT_outset, "E loop outset ", (unsigned int) k);
	  cout << " flag " << (unsigned int ) CT_flag.Decode() << endl;
	}
	/////////////////////////////
  }

  XLS_CHECK_OK(set_length(CT_outset, CT_flag));
  ///////////////////////////
  if (debug) {
	print_CT(debug, CT_outset, "E final outset ", 0);
	cout << " flag " << (unsigned int ) CT_flag.Decode() << endl;
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
  std::string out_fname;
  unsigned int seed(1);
  
  // parse command line iputs
  parse_inputs(argc, argv, verbose, debug, common_flag, seed, n_neighbors, out_fname);

  // create inputs.
  SetStruct sets[MAX_NEIGHBORS]; // max storage

  SetStruct goodIntersectOutput = {false, 0, {0}};
  SetStruct goodUnionOutput = {false, 0, {0}};

  generate_data(n_neighbors, verbose, common_flag, seed,
				sets, goodIntersectOutput, goodUnionOutput);

  
  // loop over two passes, the first for intersection, the second for union
  for (auto pass = 0; pass < 2; pass++){
	if (pass == 0) {
	  cout << "Computing intersection" << endl;
	} else {
	  cout << "Computing union" << endl;
	}
	
	Encoded<SetStruct> CT_oldset; 
	Encoded<SetStruct> CT_newset; 

	SetStruct oldset;
	SetStruct newset;

	// Encode first set and save it as oldset
	CT_oldset.Encode(sets[0]);
	oldset = sets[0];
	
	if (verbose) { //verify initial encryption is valid
	  bool good(false);
	  SetStruct input_round_trip;
	  input_round_trip = CT_oldset.Decode();
	  
	  // verify round trip of input
	  good = verify_set(input_round_trip, sets[0]);
	  cout << "Round trip of input set 0: "<< (good?"passed":"failed") << endl;
	}

	//loop over the remaining sets
	for(auto ix = 1; ix < n_neighbors; ix++) {
	  
	  // Encode data
	  CT_newset.Encode(sets[ix]);
	  newset = sets[ix];

	  //verify initial encryption is valid
	  print_CT_verify(verbose, CT_newset, sets[ix], "Round trip of input set", ix );


	  Encoded <SetStruct> CT_outset;
	  // operate on sets: Oldset = OP(Oldset, NewSet)
	  if (pass == 0) {
		CT_oldset = intersect_outer(CT_oldset, CT_newset, debug);
	  } else {
		CT_oldset = union_outer(CT_outset, CT_oldset, CT_newset, debug);
	  }
	  cout << "iteration " << ix << endl;
	  print_CT(verbose, CT_oldset, "resulting set pass ", ix);
	}
	
	//oldset now contains the result set
	auto dec_oldset = CT_oldset.Decode();	  
	bool good(false);
	if (pass == 0) {
	  cout << "verifying intersection: "; 		
		good = verify_set(dec_oldset, goodIntersectOutput);
	} else {
	  cout << "verifying union: "; 		
	  good = verify_set(dec_oldset, goodUnionOutput);
	}
	
	cout << (good?"passed":"failed") << endl;
	
  }  
  cout << "Done" << endl;
  exit(0);

}
