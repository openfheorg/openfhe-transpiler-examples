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
#include "parse_inputs.h"
#include "generate_data.h"

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/shortest_path/create_cost_vector_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/create_constrained_cost_vector_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/vector_min_index_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/select_path_yosys_interpreted_cleartext.h"
#include "transpiler/examples/shortest_path/update_path_yosys_interpreted_cleartext.h"

#else
#include "transpiler/examples/shortest_path/create_cost_vector_cleartext.h"
#include "transpiler/examples/shortest_path/create_constrained_cost_vector_cleartext.h"
#include "transpiler/examples/shortest_path/vector_min_index_cleartext.h"
#include "transpiler/examples/shortest_path/select_path_cleartext.h"
#include "transpiler/examples/shortest_path/update_path_cleartext.h"
#endif

using namespace std;

int main(int argc, char** argv) {

  bool simpleCostFlag;
  bool constrainedFlag;
  bool verbose;
  std::string out_fname;
  
  parse_inputs(argc, argv, simpleCostFlag, constrainedFlag, verbose, out_fname);

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
  
  // Encode data
  EncodedArray<PathStruct, MAX_NEIGHBORS> CT_paths; 
  CT_paths.Encode(paths);
    
  PathStruct input_round_trip[MAX_NEIGHBORS];
  CT_paths.Decode(input_round_trip);

  bool good;
  // verify round trip of input
  good = verify_input(input_round_trip, paths);
  cout << "Round trip of input: "<< (good?"passed":"failed") << endl;

  EncodedArray<unsigned int, MAX_NEIGHBORS> CT_costVec;
  unsigned int maxes[MAX_NEIGHBORS] = {MAX_COST};
  CT_costVec.Encode(maxes);
  Encoded<unsigned int> CT_ignoreId(ignoreId);

  if (constrainedFlag){
    XLS_CHECK_OK(create_constrained_cost_vector(CT_paths, CT_ignoreId, CT_costVec));
  } else {  
    XLS_CHECK_OK(create_cost_vector(CT_paths, CT_costVec));
  }              
  // use this absl class because I don't have an alternative example
  absl::FixedArray<unsigned int> tmp = CT_costVec.Decode();
    // so copy it into a normal data type for our use
  
  unsigned int decCostVec[MAX_NEIGHBORS];
  for (auto i = 0; i< MAX_NEIGHBORS; i++){
	decCostVec[i] = tmp[i];
  }

  good = verify_cost_vector(decCostVec, costVec);
  cout << "Cost Vector: " << (good?"passed":"failed") << endl;

  // find the index of the minimum

  Encoded<unsigned char> CT_index(0);
  XLS_CHECK_OK(vector_min_index(CT_costVec, CT_index));
  
  unsigned char res_index = CT_index.Decode();

  good = verify_min_index(res_index, minCostIndex);
  cout <<"min_index of x: " << (good?"passed":"failed") << endl;

  // select correct PathStruct
  // Encode data
  Encoded<PathStruct> CT_outPath;

  XLS_CHECK_OK(select_path(CT_outPath,  CT_paths, CT_index));

  // debug check
  PathStruct outPath;
  // update the accumlated Path (which contains the shortest path).


  Encoded<unsigned int> CT_myId(myId);
  Encoded<unsigned int> CT_myCost(myCost);

  XLS_CHECK_OK(update_path( CT_outPath, CT_myId, CT_myCost));
 
  // validate outPath
  outPath = CT_outPath.Decode();

  good = verify_output(outPath, goodOutput);
  cout << "Validating computation output: " << (good?"passed":"failed") << endl;
  
  cout << "Done" << endl;


}
