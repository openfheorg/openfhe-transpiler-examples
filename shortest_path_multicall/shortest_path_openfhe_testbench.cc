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
#include "transpiler/examples/shortest_path/create_cost_vector_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/create_constrained_cost_vector_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/vector_min_index_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_interpreted_openfhe.h"
#elif defined(USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/shortest_path/create_cost_vector_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/create_constrained_cost_vector_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/vector_min_index_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_yosys_interpreted_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_yosys_interpreted_openfhe.h"
#else
#include "transpiler/examples/shortest_path/create_cost_vector_openfhe.h"
#include "transpiler/examples/shortest_path/create_constrained_cost_vector_openfhe.h"
#include "transpiler/examples/shortest_path/vector_min_index_openfhe.h"
#include "transpiler/examples/shortest_path/select_path_openfhe.h"
#include "transpiler/examples/shortest_path/update_path_openfhe.h"
#endif

using namespace lbcrypto;
using namespace std;

int main(int argc, char** argv) {

  bool simpleCostFlag;
  bool constrainedFlag;
  bool verbose;
  unsigned int n_neighbors(MAX_NEIGHBORS);
  std::string out_fname;
  lbcrypto::BINFHE_PARAMSET param_set;
  lbcrypto::BINFHE_METHOD method;
  std::string param_set_name;
  std::string method_name;
  
  TimeVar t;					// for timing
  double execTime(0.0);

  parse_inputs(argc, argv, simpleCostFlag, constrainedFlag, verbose,
               param_set, method, param_set_name, method_name,
               n_neighbors, out_fname);
  
  //note currently n_neighbors is not used

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

  outfile << "\"param\",\"method\",\"constrainedFlag\",\"simpleCostFlag\",\"Create cost vector\",\"Create constrained cost vector\",\"Vector min\",\"Select path\",\"Path advertisement\" " << endl;

  outfile << param_set_name << "," << method_name << "," << constrainedFlag << "," << simpleCostFlag << ",";

  sizeoutfile << "Using BinFHE "<<param_set_name <<", " << method_name <<endl;
  //generate FHE context and keys 
  if (verbose) {
    cout<< "Using BinFHE "<<param_set_name <<", " << method_name <<endl;
  }

  auto cc = BinFHEContext();
  cc.GenerateBinFHEContext(param_set, method); //strictly for debugging

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
  const unsigned int MAX_COST(65536*256-1);   // apparently there is a
                                              // problem going bigger.
  PathStruct paths[MAX_NEIGHBORS];
  unsigned int costVec[MAX_NEIGHBORS];
  unsigned int minCost(MAX_COST);
  unsigned char minCostIndex(0);

  PathStruct goodOutput;

  generate_data(simpleCostFlag, constrainedFlag, verbose,
                ignoreId, myId, myCost, MAX_COST,
                paths, costVec, minCost, minCostIndex, goodOutput);
  
  // encrypt data

  OpenFheArray<PathStruct, MAX_NEIGHBORS> CT_paths(cc); 
  CT_paths.SetEncrypted(paths, sk);

  sizeoutfile << "bit_width of input CT_paths is "<< CT_paths.bit_width() <<endl;
  sizeoutfile << "bytes: "<< CT_paths.bit_width() * CTsize <<endl;
  sizeoutfile << "single path in bytes: "<< CT_paths.bit_width() * CTsize / (float)MAX_NEIGHBORS <<endl;
  
  PathStruct input_round_trip[MAX_NEIGHBORS];
  CT_paths.Decrypt(input_round_trip, sk);
  bool good;
  // verify round trip of input
  good = verify_input(input_round_trip, paths);
  cout << "Round trip of input: "<< (good?"passed":"failed") << endl;



  OpenFheArray<unsigned int, MAX_NEIGHBORS> CT_costVec(cc);
  unsigned int maxes[MAX_NEIGHBORS] = {MAX_COST};
  CT_costVec.SetEncrypted(maxes, sk); 

  auto CT_ignoreId = OpenFhe<unsigned int>::Encrypt(ignoreId, cc, sk);

  if (constrainedFlag){
    cout << "create_constrained cost_vector" << endl;        
    TIC(t);
    XLS_CHECK_OK(create_constrained_cost_vector(CT_paths, CT_ignoreId, CT_costVec, cc));
    execTime = TOC_MS(t);;
	outfile << " 0," << execTime << "," ;
	cout << " time: " << execTime/1000.0 << " s" << std::endl;
  } else {    
    cout << "create_cost_vector" << endl;
    TIC(t);
    XLS_CHECK_OK(create_cost_vector(CT_paths, CT_costVec, cc));
    execTime = TOC_MS(t);;
	outfile <<  execTime << ", 0," ;
	cout << " time: " << execTime/1000.0 << " s" << std::endl;
  }
  // use this absl class because I don't have an alternative example
  absl::FixedArray<unsigned int> tmp = CT_costVec.Decrypt(sk);
  // so copy it into a normal data type for our use
  
  unsigned int decCostVec[MAX_NEIGHBORS];
  for (auto i = 0; i< MAX_NEIGHBORS; i++){
    decCostVec[i] = tmp[i];
  }

  good = verify_cost_vector(decCostVec, costVec);
  cout << "Cost Vector: " << (good?"passed":"failed") << endl;

  // find the index of the minimum

  OpenFhe<unsigned char> CT_index(cc);
  cout << "vector_min_index" << endl;

  TIC(t);
  XLS_CHECK_OK(vector_min_index(CT_costVec, CT_index, cc));
  execTime = TOC_MS(t);;
  outfile << execTime << "," ;
  cout << " time: " << execTime/1000.0 << " s" << std::endl;  

  unsigned char res_index = CT_index.Decrypt(sk);
  good = verify_min_index(res_index, minCostIndex);
  cout <<"min_index of x: " << (good?"passed":"failed") << endl;
  // select correct PathStruct
  // Encode data
  OpenFhe<PathStruct> CT_outPath(cc);

  cout << "select_path" << endl;
  TIC(t);
  XLS_CHECK_OK(select_path(CT_outPath,  CT_paths, CT_index, cc));
  execTime = TOC_MS(t);;
  outfile << execTime << "," ;
  cout << " time: " << execTime/1000.0 << " s" << std::endl;
	
  // debug check
  PathStruct outPath;
  // update the accumlated Path (which contains the shortest path).


  auto CT_myId = OpenFhe<unsigned int>::Encrypt(myId, cc, sk);
  auto CT_myCost = OpenFhe<unsigned int>::Encrypt(myCost, cc, sk);
  cout << "update_path" << endl;
  TIC(t);
  XLS_CHECK_OK(update_path( CT_outPath, CT_myId, CT_myCost, cc));
  execTime = TOC_MS(t);;
  outfile << execTime << "," ;
  cout << " time: " << execTime/1000.0 << " s" << std::endl;

  sizeoutfile << "bit_width() of output CT_outpath is "<< CT_outPath.bit_width() <<endl;
  sizeoutfile << "size of output CT_outpath is "<< CT_outPath.bit_width()*CTsize <<" bytes" <<endl;

  // validate outPath
  outPath = CT_outPath.Decrypt(sk);

  good = verify_output(outPath, goodOutput);
  cout << "Validating computation output: "<< (good?"passed":"failed") << endl;


  cout << "Done" << endl;
  outfile <<endl;
  
  outfile.close();// close the output file
  
  sizeoutfile.close();// close the size output file
  
}
