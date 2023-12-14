
#include <algorithm>
#include <functional>
#include "generate_data.h"

using namespace std;

//local double loop version of the intersection function for validation
SetStruct local_intersect(const SetStruct oldSet, const SetStruct newSet){
  SetStruct outSet;
  outSet.overflow = false;

  unsigned char flag(0);
  
  unsigned char oldLen = oldSet.setLength;
  unsigned char newLen = newSet.setLength;

  for(unsigned char q = 0; q < MAX_SET_LEN; q++) {
	if (q == oldLen){
	  break;
	}
	for(unsigned char w = 0; w < MAX_SET_LEN; w++) {
	  if (w == newLen){
		break;
	  }
	  
	  //checking if both arrays contain a particular element
	  if(oldSet.set[q] == newSet.set[w]) {
		if (flag == MAX_SET_LEN) {
		  // we just overflowed
		  outSet.overflow = true;
		  break; //jump out of this loop 
		}
		outSet.set[flag] = oldSet.set[q];
		flag = flag+1;
		break;
	  }
	}
	if (outSet.overflow == true){
	  break; //and jump out of this loop
	}
  }
  outSet.setLength = flag;
  return outSet;
}

//local double loop version of the union function for validation
SetStruct local_union(const SetStruct oldSet, const SetStruct newSet){
  unsigned char oldLen = oldSet.setLength;
  unsigned char newLen = newSet.setLength;

  SetStruct outSet {false, 0, 0};
  outSet.overflow = false;
  // copy outSet from oldSet

  for(unsigned char i = 0; i < MAX_SET_LEN; i++) {
	if (i == oldLen) {
	  break;
	}
	outSet.set[i] = oldSet.set[i];
  }
  outSet.setLength = oldSet.setLength;

  unsigned char flag(0);
  
  flag = oldLen;

  for(unsigned char k = 0; k < MAX_SET_LEN; k++) {
    if (k == newLen){
	  break;
    }

    for(unsigned char m = 0; m < MAX_SET_LEN; m++) {
	  if (m == oldLen){
	    break;
	  }
	  
	  //eliminating common elements among the given arrays
	  if(newSet.set[k] == outSet.set[m]) {
		break;
	  } else if (m == oldLen-1) {
		if (flag == MAX_SET_LEN) {
		  // we just overflowed
		  outSet.overflow = true;
		  break; //jump out of this loop 
		}
		outSet.set[flag] = newSet.set[k];
		flag = flag+1;
	  }
    }
    if (outSet.overflow == true){
      break; //and jump out of this loop
    }
  }
  outSet.setLength = flag;
  return outSet;
}

// function to generate data for the test runs. 
// inputs:
// n_neighbors: number of sets to generate
// verbose: if true report progress
// common_flag: if true add common element to all sets (for nozero intersetion)
// output:
// sets ponter to array of sets to generate
// goodIntersectOutput: intersection of sets (for validation)
// goodUnionOutput: union of sets (for validation)
void generate_data(const unsigned int n_neighbors,
				   const bool verbose, 
				   const bool common_flag,
				   const unsigned int seed,
				   SetStruct *sets,
				   SetStruct &goodIntersectOutput,
				   SetStruct &goodUnionOutput
				   ){

  // create RNG
  //std::random_device dev; //replace with these two lines is for randomness
  //std::mt19937 rng(dev()); 
  std::seed_seq seq{1*seed,2*seed,3*seed,4*seed,5*seed};
  std::mt19937 rng{seq};  //same # every time
  // distribution in range [0, MAX_SET_LEN] for set contents
  std::uniform_int_distribution<std::mt19937::result_type> uni(0, MAX_SET_LEN); 

  /////////////////////////////////////////////////////////////////////////
  // create inputs.

  if (n_neighbors > MAX_SET_LEN) {

	cout << "Note n_neighbors set to "<< n_neighbors
		 << " and MAX_SET_LEN set to "<< (unsigned int) MAX_SET_LEN
		 << " cannot fill sets with unique values. Exiting" << endl;
	exit(-1);
  }
  const unsigned int common(9); // a common element for intersection
  // clearall input data
  for (unsigned char i=0; i < n_neighbors; i++){
    unsigned char thisLength = i+1;
    sets[i].setLength = thisLength;
    for (unsigned char j = 0; j< MAX_SET_LEN; j++) {
      sets[i].set[j] = 0U;
    }

    sets[i].overflow = false;
	//loop over length to fill set with Unique values
	unsigned int tmp(0);
	for (unsigned char j = 0; j< thisLength; j++) {
	  bool done(false);
	  while (!done){
		tmp = (unsigned int) uni(rng); //gen a random number
		done = true;
		for (unsigned char k = 0; k < j; k++){ //test if unique
		  // if there is a match, done is false
		  done &= (tmp != sets[i].set[k]);
		  if (tmp == common) done=false;
		}
	  } //now unique
	  sets[i].set[j] = tmp;
	  
    }
	
	if (common_flag) {
	  // provide a common element to guarentee nonzero intersection
	  sets[i].set[thisLength-(unsigned char)1] = common; 
	}
  }

  if (verbose) {
	cout << "inputs are: " << endl;
	for (unsigned char i=0; i < n_neighbors; i++){
	  int thisId = i+1;
	  cout << " neighbor : "<< thisId
		   << " overflow " << sets[i].overflow
		   << " length " << (unsigned int) sets[i].setLength << endl;
	  for (unsigned char j = 0; j< sets[i].setLength; j++) {
		cout << " " <<  (unsigned int) sets[i].set[j];
	  }
	  cout << endl;
	}
  }

  //generate intersection and union (with two passes)
  for (auto pass = 0; pass < 2; pass++){

	SetStruct oldset = sets[0];
	SetStruct newset = sets[0];
	
	//loop over the remaining sets
	for(auto ix = 1; ix < n_neighbors; ix++) {
	  
	  newset = sets[ix];
	  
	  // operate on sets: Oldset = OP(Oldset, NewSet)
	  if (pass == 0) {
		oldset = local_intersect(oldset, newset);
	  } else {
	    oldset = local_union(oldset, newset);
	  }
	}
	if (pass == 0) {
	  goodIntersectOutput = oldset;
	} else {
	  goodUnionOutput = oldset;
	}
  }

  if (verbose) {
	auto len = (unsigned int) goodIntersectOutput.setLength;
	cout << "Intersection is: " << endl;
	cout <<  " overflow " << goodIntersectOutput.overflow
		 << " length " << (unsigned int) len << endl;
	for (unsigned char j = 0; j< len; j++) {
	  cout << " " <<  goodIntersectOutput.set[j];
	}
	cout << endl;

  
	len = (unsigned int) goodUnionOutput.setLength;
	cout << "Union is: " << endl;
	cout <<  " overflow " << goodUnionOutput.overflow
		 << " length " << (unsigned int) len << endl;
	for (unsigned char j = 0; j< len; j++) {
	  cout << " " << goodUnionOutput.set[j];
	}
	cout << endl;
  }

}

// test function
// returns true if two SetStructs are the same
bool verify_set(const SetStruct inSet, const SetStruct goodSet) {
  bool good;

  good = true;
  bool test;
  test = (inSet.overflow == goodSet.overflow);
  good &= test;
  if (!test) {
	cout << "inSet.overflow " << "decrypted: " << inSet.overflow
		 << " original: " << goodSet.overflow << endl;
  }
  
  test = (inSet.setLength == goodSet.setLength); 
  good &= test; 
  if (!test) {
	cout << "inSet.setLength " << "decrypted: "
		 << (unsigned int) inSet.setLength
		 << " original: " << (unsigned int) goodSet.setLength << endl;
	return good;
  }

  //note the sets themselves may be out of order due to how things are
  //computed via the transpiler. I do not understand why.

  // so copy the valid array entries to vectors and sort both
  std::vector<unsigned int> sortIn;
  for (unsigned char i=0; i < inSet.setLength; i++){
	sortIn.push_back(inSet.set[i]);
  }
  std::vector<unsigned int> sortGood;
  for (unsigned char i=0; i < goodSet.setLength; i++){
	sortGood.push_back(goodSet.set[i]);
  }

  sort(sortGood.begin(), sortGood.end());
  sort(sortIn.begin(), sortIn.end());
  
  for (auto i=0; i < goodSet.setLength; i++){
	bool test = (sortIn[i] == sortGood[i]);
    good &= test;
  }

  if (!good) {
	cout << "Full set contents: " << endl;
	for (auto i=0; i < MAX_SET_LEN; i++){
	  cout << "i = "<<i<<": " << "decrypted: " << sortIn[i] << " original: "
		   << sortGood[i] << endl;
	}
  }
  return good;
}

