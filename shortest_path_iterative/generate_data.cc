#include "generate_data.h"

using namespace std;

void generate_data(const unsigned int n_neighbors,
				   const bool simpleCostFlag, const bool constrainedFlag,
				   const bool verbose,
				   const unsigned int ignoreId, const unsigned int myId,
				   unsigned int &myCost,
				   PathStruct *paths,
				   unsigned int minCost, unsigned char minCostIndex,
				   PathStruct &goodOutput
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
  
  for (unsigned char i=0; i < n_neighbors; i++){
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
    unsigned char thisPath(n_neighbors-1);
    unsigned char thisPathLength(paths[thisPath].pathLength);
    paths[thisPath].path[thisPathLength-1] = ignoreId;
    paths[thisPath].cost = MAX_COST;

	if (verbose) {
	  cout << "constraining neighbor path "<< (unsigned int) thisPath+1 << " contains ID: " << ignoreId
		   << " by setting cost to MAX_COST"<<endl;
	}
    //recompute min cost
	minCost = MAX_COST;
    for (unsigned char i=0; i < n_neighbors; i++){
      unsigned int thisCost = paths[i].cost; 
      if (thisCost < minCost){
        minCost =thisCost;
        minCostIndex = i;
      }
    }
  }

  if (verbose) {
	cout << "inputs are: " << endl;
	for (unsigned char i=0; i < n_neighbors; i++){
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
bool verify_path(const PathStruct inPath, const PathStruct goodPath) {
  bool good;

  good = true;
  bool test;
  test = (inPath.cost == goodPath.cost);
  good &= test;
  if (!test) {
	cout << "inPath.cost " << "decrypted: " << inPath.cost
		 << " original: " << goodPath.cost << endl;
  }
  test = (inPath.pathLength == goodPath.pathLength); 
  good &= test; 
  if (!test) {
	cout << "inPath.pathLength " << "decrypted: " << inPath.pathLength
		 << " original: " << goodPath.pathLength << endl;
  }
  for (unsigned char i=0; i < MAX_PATH_LEN; i++){
	bool test = (inPath.path[i] == goodPath.path[i]);
    good &= test;
	if (!test) {
	  cout << "i = "<<i<<": " << "decrypted: " << inPath.path[i] << " original: " << goodPath.path[i] << endl;
	}
  }


  return good;
}
