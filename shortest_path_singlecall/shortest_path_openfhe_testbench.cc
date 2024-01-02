// Copyright 2022 Duality Technologies Inc.
// D. Cousins
//
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
#include <iostream>
#include "shortest_path.h"

#include "openfhe/binfhe/binfhecontext.h"
#include "transpiler/data/openfhe_data.h"
#include "xls/common/logging/logging.h"


#ifdef USE_INTERPRETED_OPENFHE
#include "transpiler/examples/shortest_path/compute_shortest_path_interpreted_openfhe.h"
#elif defined(USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/shortest_path/compute_shortest_path_yosys_interpreted_openfhe.h"
#else
#include "transpiler/examples/shortest_path/compute_shortest_path_openfhe.h"
#endif

using namespace lbcrypto;
using namespace std;

absl::Time start_time;
double cpu_start_time;
void tic(){
  std::cout << "Starting!" << std::endl;
  start_time = absl::Now();
  cpu_start_time = clock();
}

void toc(){
  double cpu_end_time = clock();
  absl::Time end_time = absl::Now();
  std::cout << "\t\t\t\t\tTotal time: "
            << absl::ToDoubleSeconds(end_time - start_time) << " secs"
            << std::endl;
  std::cout << "\t\t\t\t\t  CPU time: "
            << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs"
            << std::endl;
}


int main(int argc, char** argv) {
  int nNeighbors(2);
  bool fail(false);
  if (argc > 1) {

	std::string input = argv[1];
	if (input.compare("-h") == 0){
	  fail = true;
	} else {
	  nNeighbors = std::stoi(input);
	  if ((nNeighbors < 1)||(nNeighbors>MAX_NEIGHBORS)) {
		fail = true;
	  }
	}
  }
  if (fail) {
	cerr<< "Usage: shortest_path_cleartext_testbench nNeighbors(1:" << MAX_NEIGHBORS <<endl;
	return EXIT_FAILURE;
  }

  auto cc = BinFHEContext();
  cc.GenerateBinFHEContext(TOY); //strictly for debugging
  //note MEDIUM s >= 100 bits security
  //STD128Q is 128 bit quantum , 192 and 256 are supported.
  //remove Q to limit to classical attacks.
  //cc.GenerateBinFHEContext(MEDIUM);
  //cc.GenerateBinFHEContext(STD128Q);
  //cc.GenerateBinFHEContext(STD128Q_OPT); //may need 64 bit arch. 

  std::cout << "Generating the secret key..." << std::endl;

  // Generate the secret key
  auto sk = cc.KeyGen();

  std::cout << "Generating the bootstrapping keys..." << std::endl;

  // Generate the bootstrapping keys (refresh and switching keys)
  cc.BTKeyGen(sk);

  std::cout << "Completed key generation." << std::endl;

  // create inputs.

  // create RNG
  //std::random_device dev;
  //std::mt19937 rng(dev()); //this is for randomness
  std::seed_seq seq{1,2,3,4,5};
  std::mt19937 rng{seq};  //same # every time
  // distribution in range [1, 100] for node ids
  std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100); 
  // create inputs.

  int myId(99);
  int myCost(10);
  
  PathStruct paths[MAX_NEIGHBORS];
   __attribute__((unused)) int costVec[MAX_NEIGHBORS];
  //int zeros[MAX_NEIGHBORS] = {};
   __attribute__((unused)) int maxes[MAX_NEIGHBORS] = {INT_MAX};

  unsigned int minCost(INT_MAX);

  unsigned char minCostIndex(0);
  
  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	unsigned int thisCost = (i+1)*10;
	paths[i].cost = thisCost;
	costVec[i] = thisCost; //to verify
	if (thisCost < minCost){
	  minCost =thisCost;
	  minCostIndex = i;
	}
	paths[i].pathLength = i+1;
	for (unsigned char j = 0; j< MAX_PATH_LEN; j++) {
	  paths[i].path[j] = 0;
	}
	int thisPathLength = paths[i].pathLength;
	for (unsigned char j = 0; j < thisPathLength-1; j++) {
	  paths[i].path[j] = dist100(rng);
	}
	paths[i].path[thisPathLength-1] = thisId; //this neighbor's  ID.
  }

  cout << "inputs are: " << endl;
  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	cout << " neighbor : "<< thisId
		 << " cost " << paths[i].cost
		 << " length " << paths[i].pathLength << endl;
	for (unsigned char  j = 0; j< MAX_PATH_LEN; j++) {
	  cout << " " << paths[i].path[j];
	}
	cout << endl;
  }

  //cleartext generation of output

  PathStruct goodOutput = paths[minCostIndex];
  goodOutput.cost += myCost;
  goodOutput.path[goodOutput.pathLength] = myId;
  goodOutput.pathLength++;
  
  // encrypt data

  OpenFheArray<PathStruct, MAX_NEIGHBORS> CT_paths(cc); 
  CT_paths.SetEncrypted(paths, sk);
	
  std::cout << "Initial round-trip check: " << std::endl;
  PathStruct round_trip[MAX_NEIGHBORS];
  CT_paths.Decrypt(round_trip, sk);

  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	cout << " neighbor : "<< thisId
		 << " cost " << round_trip[i].cost
		 << " length " << round_trip[i].pathLength << endl;
	for (unsigned char j = 0; j< MAX_PATH_LEN; j++) {
	  cout << " " << round_trip[i].path[j];
	}
	cout << endl;
  }

  cout << "Encoding done" << endl;

  OpenFhe<PathStruct> CT_outPath(cc);
  auto CT_myId = OpenFhe<int>::Encrypt(myId, cc, sk);
  auto CT_myCost = OpenFhe<unsigned int>::Encrypt(myCost, cc, sk);
  
  cout << "Encryption done" << endl;
  cout << "\t\t\t\t\tcompute_shortest_path:" << endl;
  
  tic();
  XLS_CHECK_OK(compute_shortest_path( CT_paths, CT_myId, CT_myCost, CT_outPath, cc));
  toc();
  cout << "\t\t\t\t\tComputation done" << endl;
  
	  // debug outPath
  std::cout << "debug outPath: " << std::endl;
  PathStruct outPath = CT_outPath.Decrypt(sk);

  cout << " cost " << outPath.cost
	   << " length " << outPath.pathLength << endl;
  for (unsigned char j = 0; j< MAX_PATH_LEN; j++) {
	cout << " " << outPath.path[j];
  }
  cout << endl;
  cout <<  " minCost " << minCost <<endl;

  cout << "Validating computation output: "<<endl;

  bool good(true);

  good &= (outPath.cost == goodOutput.cost); 
  good &= (outPath.pathLength == goodOutput.pathLength); 
  for (unsigned char i=0; i < MAX_PATH_LEN; i++){
	good &= (outPath.path[i] == goodOutput.path[i]);
  }
  cout << (good?"passed":"failed") << endl;
  
  cout << "Done" << endl;

}
