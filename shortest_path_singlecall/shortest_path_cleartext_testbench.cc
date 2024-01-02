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

#include <random>
#include <iostream>
#include "shortest_path.h"

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"
#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/shortest_path/compute_shortest_path_yosys_interpreted_cleartext.h"
#else
#include "transpiler/examples/shortest_path/compute_shortest_path_cleartext.h"
#endif

using namespace std;

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

  // create RNG
  //std::random_device dev;
  //std::mt19937 rng(dev()); //this is for randomness
  std::seed_seq seq{1,2,3,4,5};
  std::mt19937 rng{seq};  //same # every time
  // distribution in range [1, 100] for node ids
  std::uniform_int_distribution<std::mt19937::result_type> dist100(1,100); 
  // create inputs.

  int myId(99);
  unsigned int myCost(10);
  
  PathStruct paths[MAX_NEIGHBORS];
   __attribute__((unused)) int costVec[MAX_NEIGHBORS];
  //int zeros[MAX_NEIGHBORS] = {};
   __attribute__((unused)) int maxes[MAX_NEIGHBORS] = {INT_MAX};
  unsigned char i;
  unsigned int minCost(INT_MAX);

  unsigned char minCostIndex(0);
  
  for (i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	unsigned int thisCost = (i+1)*10;
	paths[i].cost = thisCost;
	costVec[i] = thisCost; //to verify
	if (thisCost < minCost){
	  minCost =thisCost;
	  minCostIndex = i;
	}
	paths[i].pathLength = i+1;
	for (auto j = 0; j< MAX_PATH_LEN; j++) {
	  paths[i].path[j] = 0;
	}
	int thisPathLength = paths[i].pathLength;
	for (auto j = 0; j < thisPathLength-1; j++) {
	  paths[i].path[j] = dist100(rng);
	}
	paths[i].path[thisPathLength-1] = thisId; //this neighbor's  ID.
  }

  cout << "inputs are: " << endl;
  for (i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	cout << " neighbor : "<< thisId
		 << " cost " << paths[i].cost
		 << " length " << paths[i].pathLength << endl;
	for (auto j = 0; j< MAX_PATH_LEN; j++) {
	  cout << " " << paths[i].path[j];
	}
	cout << endl;
  }

  //cleartext generation of output

  PathStruct goodOutput = paths[minCostIndex];
  goodOutput.cost += myCost;
  goodOutput.path[goodOutput.pathLength] = myId;
  goodOutput.pathLength++;

  // Encode data
  EncodedArray<PathStruct, MAX_NEIGHBORS> CT_paths; 
  CT_paths.Encode(paths);
	
  std::cout << "Initial round-trip check: " << std::endl;
  PathStruct round_trip[MAX_NEIGHBORS];
  CT_paths.Decode(round_trip);

  for (i=0; i < MAX_NEIGHBORS; i++){
	int thisId = i+1;
	cout << " neighbor : "<< thisId
		 << " cost " << round_trip[i].cost
		 << " length " << round_trip[i].pathLength << endl;
	for (auto j = 0; j< MAX_PATH_LEN; j++) {
	  cout << " " << round_trip[i].path[j];
	}
	cout << endl;
  }

  cout << "Encoding done" << endl;

  Encoded<PathStruct> CT_outPath;
  Encoded<int> CT_myId(myId);
  Encoded<unsigned int> CT_myCost(myCost);
  
  XLS_CHECK_OK(compute_shortest_path( CT_paths, CT_myId, CT_myCost, CT_outPath));

  // debug outPath
  std::cout << "debug outPath: " << std::endl;
  PathStruct outPath;
  outPath = CT_outPath.Decode();

  cout << " cost " << outPath.cost
	   << " length " << outPath.pathLength << endl;
  for (auto j = 0; j< MAX_PATH_LEN; j++) {
	cout << " " << outPath.path[j];
  }
  cout << endl;
  cout <<  " minCost " << minCost <<endl;

  cout << "Validating computation output: "<<endl;

  bool good(true);

  good &= (outPath.cost == goodOutput.cost); 
  good &= (outPath.pathLength == goodOutput.pathLength); 
  for (i=0; i < MAX_PATH_LEN; i++){
	good &= (outPath.path[i] == goodOutput.path[i]);
  }
  cout << (good?"passed":"failed") << endl;
  
  cout << "Done" << endl;
  

}
