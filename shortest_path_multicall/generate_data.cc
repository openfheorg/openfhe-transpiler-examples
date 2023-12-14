#include "generate_data.h"

using namespace std;

void generate_data(const bool simpleCostFlag, const bool constrainedFlag, const bool verbose,
				   const unsigned int ignoreId, const unsigned int myId, unsigned int &myCost,
				   const unsigned int MAX_COST,
				   PathStruct paths[MAX_NEIGHBORS], unsigned int costVec[MAX_NEIGHBORS],
				   unsigned int minCost, unsigned char minCostIndex, PathStruct &goodOutput
				   ){

  // create RNG
  //std::random_device dev; //replace with these two lines is for randomness
  //std::mt19937 rng(dev()); 
  std::seed_seq seq{1,2,3,4,5};
  std::mt19937 rng{seq};  //same # every time
  // distribution in range [1, MyId- 1] for node ids (so we never generate myId)
  std::uniform_int_distribution<std::mt19937::result_type> dist100(1, myId-1); 

  /////////////////////////////////////////////////////////////////////////
  // create inputs.

  if (simpleCostFlag) {
    myCost = 1;
  }
  
  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
    int thisId = i+1;
    paths[i].pathLength = i+1;
    for (auto j = 0; j< MAX_PATH_LEN; j++) {
      paths[i].path[j] = 0;
    }

    unsigned int thisCost;

    if (simpleCostFlag) {
      thisCost = paths[i].pathLength;
    } else {
      thisCost = (i+1)*10;
    }
    paths[i].cost = thisCost;
    costVec[i] = thisCost; //to verify
    if (thisCost < minCost){
      minCost =thisCost;
      minCostIndex = i;
    }

    int thisPathLength = paths[i].pathLength;
    for (auto j = 0; j < thisPathLength-1; j++) {
      unsigned int tmp = dist100(rng);
      tmp = tmp == ignoreId?tmp+1:tmp; //don't use the ignorId
      paths[i].path[j] = dist100(rng);
    }
    paths[i].path[thisPathLength-1] = thisId; //this neighbor's  ID.
  }

  if (constrainedFlag){
    //set a path to contain the ignore ID
    unsigned char thisPath(MAX_NEIGHBORS-1);
    unsigned char thisPathLength(paths[thisPath].pathLength);
    paths[thisPath].path[thisPathLength-1] = ignoreId;
    paths[thisPath].cost = MAX_COST;

	if (verbose) {
	  cout << "constraining neighbor path "<< (unsigned int) thisPath+1 << " contains ID: " << ignoreId
		   << " by setting cost to MAX_COST"<<endl;
	}
    //recompute min cost
	minCost = MAX_COST;
    for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
      unsigned int thisCost = paths[i].cost; 
      costVec[i] = thisCost; //to verify
      if (thisCost < minCost){
        minCost =thisCost;
        minCostIndex = i;
      }
    }
  }

  if (verbose) {
	cout << "inputs are: " << endl;
	for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	  int thisId = i+1;
	  cout << " neighbor : "<< thisId
		   << " cost " << paths[i].cost
		   << " length " << (unsigned int) paths[i].pathLength << endl;
	  for (unsigned char j = 0; j< MAX_PATH_LEN; j++) {
		cout << " " << paths[i].path[j];
	  }
	  cout << endl;
	}
  }
  //cleartext generation of output

  goodOutput = paths[minCostIndex];
  goodOutput.cost += myCost;
  goodOutput.path[goodOutput.pathLength] = myId;
  goodOutput.pathLength++;
}

// returns true if two PathStructs are the same

bool verify_input(PathStruct input_round_trip[MAX_NEIGHBORS], PathStruct paths[MAX_NEIGHBORS]) {
					   
  bool good(true); 
  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	bool test = (input_round_trip[i].cost == paths[i].cost);
	good &= test;
	if (!test) {
	  cout << "cost i: " << (unsigned int) i << " decrypted: " << input_round_trip[i].cost
		   << " original: " << paths[i].cost << endl;
	}
	
	test =  (input_round_trip[i].pathLength  == paths[i].pathLength);
	good &= test;
	if (!test) {
	  cout << "pathlength i: " << (unsigned int) i << " decrypted: " << input_round_trip[i].pathLength
		   << " original: " << paths[i].pathLength << endl;
	}
	
	for (auto j = 0; j< MAX_PATH_LEN; j++) {
      test = (input_round_trip[i].path[j] == paths[i].path[j]);
	  if (!test) {
		cout << "path i,j: " << (unsigned int) i << ", "<< (unsigned int) j
			 << " decrypted: " << input_round_trip[i].path[j]
			 << " original: " << paths[i].path[j] << endl;
	  }
	  
	  good &= test;
	}
  }
  return good;
}


// returns true if two cost vectors are the same, prints error if not
bool verify_cost_vector(const unsigned int decCostVec[MAX_NEIGHBORS],
						const unsigned int costVec[MAX_NEIGHBORS]) {

  bool good = true;
  for (unsigned char i=0; i < MAX_NEIGHBORS; i++){
	bool test = (decCostVec[i] == costVec[i]);
    good &= test;
	if (!test) {
	  cout << "cost i: "<< (unsigned int) i << " decrypted: " << decCostVec[i] << " original: " << costVec[i] << endl;
	}
  }

  return good;
}

// returns true if both inputs are equal 
bool verify_min_index(const unsigned char res_index, const unsigned char minCostIndex) {
  
  bool good =  (res_index == minCostIndex);

  if (!good) {
	cout << "decrypted res_index " << (unsigned int)res_index << endl;
	cout << "correct minCostIndex "<< (unsigned int)minCostIndex << endl;
  }
  return good;
}

// returns true if both PathStructs are equal
bool verify_output(const PathStruct outPath, const PathStruct goodOutput) {
  bool good;

  good = true;
  bool test;
  test = (outPath.cost == goodOutput.cost);
  good &= test;
  if (!test) {
	cout << "outPath.cost " << "decrypted: " << outPath.cost
		 << " original: " << goodOutput.cost << endl;
  }
  test = (outPath.pathLength == goodOutput.pathLength); 
  good &= test; 
  if (!test) {
	cout << "outPath.pathLength " << "decrypted: " << outPath.pathLength
		 << " original: " << goodOutput.pathLength << endl;
  }
  for (unsigned char i=0; i < MAX_PATH_LEN; i++){
	bool test = (outPath.path[i] == goodOutput.path[i]);
    good &= test;
	if (!test) {
	  cout << "i: " << "decrypted: " << outPath.path[i] << " original: " << goodOutput.path[i] << endl;
	}
  }


  return good;
}
