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

// Iterative version, hopefully more efficient


#include <stdint.h>
#include <iostream>
#include "shortest_path.h"
#include "parse_inputs.h"
#include "generate_data.h"

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/shortest_path/constrain_path_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/select_path_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/update_path_yosys_interpreted_cleartext.h"

#else
#include "transpiler/examples/shortest_path/constrain_path_cleartext.h"
#include "transpiler/examples/shortest_path/select_path_cleartext.h"
#include "transpiler/examples/shortest_path/update_path_cleartext.h"
#endif

using namespace std;

int main(int argc, char** argv) {

  bool simpleCostFlag(false);
  bool constrainedFlag(false);
  bool verbose(false);
  unsigned int n_neighbors(MAX_NEIGHBORS);
  std::string out_fname;
  
  parse_inputs(argc, argv, simpleCostFlag, constrainedFlag, verbose, n_neighbors, out_fname);

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

  Encoded<PathStruct> CT_oldpath; 
  Encoded<PathStruct> CT_newpath; 
  Encoded<unsigned int> CT_ignoreId(ignoreId);

  // Encode first path and save it as oldpath
  CT_oldpath.Encode(paths[0]);
  if (constrainedFlag){
	// if we are constraining, adjust this path's cost if needed
    XLS_CHECK_OK(constrain_path(CT_oldpath, CT_ignoreId));
  }

  PathStruct input_round_trip;
  input_round_trip = CT_oldpath.Decode();

  bool good;
  // verify round trip of input
  good = verify_path(input_round_trip, paths[0]);
  cout << "Round trip of input path 0: "<< (good?"passed":"failed") << endl;

  //loop over the remaining paths
  for(auto ix = 1; ix < n_neighbors; ix++) {
	  
	// Encode data
	CT_newpath.Encode(paths[ix]);
	input_round_trip = CT_newpath.Decode();

	if (constrainedFlag){
	  XLS_CHECK_OK(constrain_path(CT_newpath, CT_ignoreId));
	}
	auto dec_newpath = CT_newpath.Decode();
	good = verify_path(dec_newpath, paths[ix]);
	cout << "constrain Path "<<ix << ": " << (good?"passed":"failed") << endl;


	// select path with lowest cost, Oldpath = min(Oldpath, NewPath)
	XLS_CHECK_OK(select_path(CT_oldpath, CT_newpath, CT_oldpath));
	auto dec_oldpath = CT_oldpath.Decode();	
	cout << "DEBUG: selected path cost is " << dec_oldpath.cost <<endl;
	cout << "DEBUG: selected path length is " << dec_oldpath.pathLength <<endl;
	for (auto j = 0; j< MAX_PATH_LEN; j++){
	  cout << dec_oldpath.path[j] << " ";
	}
	cout <<endl;
  }

  //oldpath now contains the shortest path
  // debug check PathStruct oldpath; should be the path with the minimum cost
  auto dec_oldpath = CT_oldpath.Decode();	  
  good = verify_path(dec_oldpath, paths[minCostIndex]);
  cout << "good minimum selected path " << (unsigned int) minCostIndex <<": "
	   << (good?"passed":"failed") << endl;
  
  // update the accumlated Path (which contains the shortest path).

  Encoded<unsigned int> CT_myId(myId);
  Encoded<unsigned int> CT_myCost(myCost);
  XLS_CHECK_OK(update_path(CT_oldpath, CT_myId, CT_myCost));
 
  // validate oldpath
  PathStruct oldpath = CT_oldpath.Decode();

  good = verify_path(oldpath, goodOutput);
  cout << "Validating computation output: " << (good?"passed":"failed") << endl;
  
  cout << "Done" << endl;


}
