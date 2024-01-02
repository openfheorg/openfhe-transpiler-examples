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


#include <stdint.h>
#include <iostream>
#include "shortest_path.h"

#define PROFILE // need to define this for TIC,TOC to work
#include "openfhe/binfhe/binfhecontext.h"
#include "binfhecontext-ser.h"

#include "transpiler/data/openfhe_data.h"
#include "xls/common/logging/logging.h"

#include "parse_inputs.h" // these must be after the openfhe includes to compile
#include "generate_data.h"


#ifdef USE_INTERPRETED_OPENFHE
#include "transpiler/examples/shortest_path/constrain_path_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_interpreted_openfhe.h"
#elif defined(USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/shortest_path/constrain_path_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_yosys_interpreted_openfhe.h"
#else
#include "transpiler/examples/shortest_path/constrain_path_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_openfhe.h"
#endif

using namespace lbcrypto;
using namespace std;

int main(int argc, char** argv) {

  bool simpleCostFlag(false);
  bool constrainedFlag(false);
  bool verbose(false);
  unsigned int n_neighbors(MAX_NEIGHBORS);
  std::string out_fname;
  lbcrypto::BINFHE_PARAMSET param_set(TOY);
  std::string param_set_name("TOY");
  
  TimeVar t;					// for timing
  TimeVar tTot;					// for timing
  double execTimeConstrain(0.0);
  double execTimeSelect(0.0);
  double execTime(0.0);
  unsigned int n_select(0);
  unsigned int n_constrain(0);
  
  parse_inputs(argc, argv, simpleCostFlag, constrainedFlag, verbose,
               param_set, param_set_name, 
               n_neighbors, out_fname);
  
  
  ofstream outfile; //file for logging output
  ofstream sizeoutfile; //file for logging size output
  try {
	outfile.open(out_fname, ios::out | ios::trunc );
  } catch (const std::exception& e) {
    cerr << out_fname << " file open exception was caught, with message '" << e.what() << "'\n";
  }
  try {
	sizeoutfile.open(out_fname+".siz", ios::out | ios::trunc );
  } catch (const std::exception& e) {
    cerr << out_fname+".siz" << " file open exception was caught, with message '" << e.what() << "'\n";
  }

  outfile << "\"param\",\"constrainedFlag\",\"simpleCostFlag\",\"n_neighbors\",\"constrain_path\",\"select_path\",\"Path advertisement\" " << endl;

  outfile << param_set_name << "," << "," << constrainedFlag << "," << simpleCostFlag << "," << n_neighbors << ",";

  sizeoutfile << "Using BinFHE "<<param_set_name << endl;
  //generate FHE context and keys 
  if (verbose) {
    cout<< "Using BinFHE "<<param_set_name  <<endl;
  }

  auto cc = BinFHEContext();
  cc.GenerateBinFHEContext(param_set); //strictly for debugging

  std::cout << "Generating the secret key..." << std::endl;
  auto sk = cc.KeyGen();		// Generate the secret key

  {
	// measure the size of the secret key
	std::ostringstream keystring;
	lbcrypto::Serial::Serialize(sk, keystring, SerType::BINARY);
	if (!keystring.str().size())
	  std:: cout << "Serialized eval sum key is empty" << endl;
    sizeoutfile << "Secret key size is " << keystring.str().size()  << " bytes" << std::endl;

  }

  std::cout << "Generating the bootstrapping keys..." << std::endl;
  cc.BTKeyGen(sk); // Generate the bootstrapping keys (refresh and switching keys)
  std::cout << "Completed key generation." << std::endl;

  {
	// measure the size of a switching key
	std::ostringstream switchstring;
	auto switchkey= cc.GetSwitchKey();
  
	lbcrypto::Serial::Serialize(switchkey, switchstring, SerType::BINARY);
	if (!switchstring.str().size())
	  std:: cout << "Serialized switchkey is empty" << endl;
    sizeoutfile << "switchkey size is " << switchstring.str().size()  << " bytes" << std::endl;
  }
  {
	// measure the size of a refresh key
	std::ostringstream refreshstring;
	auto refreshkey= cc.GetRefreshKey();
  
	lbcrypto::Serial::Serialize(refreshkey, refreshstring, SerType::BINARY);
	if (!refreshstring.str().size())
	  std:: cout << "Serialized refreshkey is empty" << endl;
    sizeoutfile << "refreshkey size is " << refreshstring.str().size()  << " bytes" << std::endl;
  }
   
  size_t CTsize;
  {
	// measure the size of a ciphertext
	std::ostringstream CTstring;
	auto ct = cc.Encrypt(sk, 1);
  
	lbcrypto::Serial::Serialize(ct, CTstring, SerType::BINARY);
	CTsize = CTstring.str().size();
	if (!CTsize)
	  std:: cout << "Serialized ciphertext is empty" << endl;
    sizeoutfile << "1 bit CT size is " << CTsize  << " bytes" << std::endl;
  }

  // create inputs.
  unsigned int ignoreId(85); // ID to be ignored in the constrained case

  unsigned int myId(99);
  unsigned int myCost(10);

  PathStruct paths[n_neighbors];

  unsigned int minCost(MAX_COST);
  unsigned char minCostIndex(0);

  PathStruct goodOutput;

  generate_data(n_neighbors, simpleCostFlag, constrainedFlag, verbose,
				ignoreId, myId, myCost, 
				paths, minCost, minCostIndex, goodOutput);
  
  // encrypt data

  OpenFhe<PathStruct> CT_oldpath(cc); 
  OpenFhe<PathStruct> CT_newpath(cc); 
  auto CT_ignoreId = OpenFhe<unsigned int>::Encrypt(ignoreId, cc, sk);

  // Encode first path and save it as oldpath

  sizeoutfile << "bit_width of input CT_oldpath is "<< CT_oldpath.bit_width() <<endl;
  sizeoutfile << "bytes: "<< CT_oldpath.bit_width() * CTsize <<endl;
  CT_oldpath.SetEncrypted(paths[0], sk);
  if (constrainedFlag){

	// if we are constraining, adjust this path's cost if needed
	TIC(t);
    XLS_CHECK_OK(constrain_path(CT_oldpath, CT_ignoreId, cc));
	execTime = TOC_MS(t);;
	execTimeConstrain += execTime;
	n_constrain++;
  }
  
  PathStruct input_round_trip;
  input_round_trip = CT_oldpath.Decrypt(sk);
  bool good;
  // verify round trip of input
  good = verify_path(input_round_trip, paths[0]);
  cout << "Round trip of input path 0: "<< (good?"passed":"failed") << endl;

  //loop over the remaining paths
  for(auto ix = 1; ix < n_neighbors; ix++) {


	// Encode data
	CT_newpath.SetEncrypted(paths[ix], sk);
	input_round_trip = CT_newpath.Decrypt(sk);

	if (constrainedFlag){
      cout << "constrain_path" << endl;
	  TIC(t);
	  XLS_CHECK_OK(constrain_path(CT_newpath, CT_ignoreId, cc));
      execTime = TOC_MS(t);;
      execTimeConstrain += execTime;
      n_constrain++;
    }
	auto dec_newpath = CT_newpath.Decrypt(sk);
	good = verify_path(dec_newpath, paths[ix]);
	cout << "constrain Path "<<ix << ": " << (good?"passed":"failed") << endl;


	// select path with lowest cost, Oldpath = min(Oldpath, NewPath)
	cout << "select_path" << endl;
	TIC(t);
	XLS_CHECK_OK(select_path(CT_oldpath, CT_newpath, CT_oldpath, cc));
	execTime = TOC_MS(t);;
    execTimeSelect += execTime;
    n_select++;

	cout << " time: " << execTime/1000.0 << " s" << std::endl;
	auto dec_oldpath = CT_oldpath.Decrypt(sk);	
	if (verbose) {
	  cout << "selected path cost is " << dec_oldpath.cost <<endl;
	  cout << "selected path length is " << (unsigned int) dec_oldpath.pathLength <<endl;
	}
	for (auto j = 0; j< MAX_PATH_LEN; j++){
	  cout << dec_oldpath.path[j] << " ";
	}
	cout <<endl;
  }

  //compute average constrain and select times
  if (n_constrain) {
	execTimeConstrain /= (float)n_constrain;
  }
  cout << "average constrain time: " << execTimeConstrain/1000.0 << " s" << std::endl;
  if (n_select) {
	execTimeSelect /= (float)n_select;  
  }
  cout << "average select time: " << execTimeSelect/1000.0 << " s" << std::endl;
  outfile << execTimeConstrain << "," << execTimeSelect << ",";

  //oldpath now contains the shortest path
  // debug check PathStruct oldpath; should be the path with the minimum cost
  auto dec_oldpath = CT_oldpath.Decrypt(sk);	  
  good = verify_path(dec_oldpath, paths[minCostIndex]);
  cout << "good minimum selected path " << (unsigned int) minCostIndex <<": "
	   << (good?"passed":"failed") << endl;
  
  // update the accumlated Path (which contains the shortest path).


  auto CT_myId = OpenFhe<unsigned int>::Encrypt(myId, cc, sk);
  auto CT_myCost = OpenFhe<unsigned int>::Encrypt(myCost, cc, sk);
  cout << "update_path" << endl;
  TIC(t);
  XLS_CHECK_OK(update_path( CT_oldpath, CT_myId, CT_myCost, cc));
  execTime = TOC_MS(t);;
  outfile << execTime << "," ;
  cout << " time: " << execTime/1000.0 << " s" << std::endl;

  
  // validate oldpath
  PathStruct oldpath = CT_oldpath.Decrypt(sk);

  good = verify_path(oldpath, goodOutput);
  cout << "Validating computation output: "<< (good?"passed":"failed") << endl;


  cout << "Done" << endl;
  outfile <<endl;
  
  outfile.close();// close the output file
  
  sizeoutfile.close();// close the size output file
  
}
