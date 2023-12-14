#ifndef GENERATE_DATA_H
#define GENERATE_DATA_H

#include <getopt.h>
#include <string.h>
#include <iostream>
#include <random>

#include "private_set.h"


// generate data
// parameters
// input const unsigned int n_neighbors number of neigbors to process in a chain,
// input const bool verbose, if true print resulting data structures
// output SetStruct sets[n_neighbors],
// output SetStruct &goodIntersectOutput, result of intersection set
// output SetStruct &goodUnionOutput, result of union set

void generate_data(const unsigned int n_neighbors,
				   const bool verbose,
				   const bool common_flag,
				   const unsigned int seed,
				   SetStruct *sets,
				   SetStruct &goodIntersectOutput,
				   SetStruct &goodUnionOutput
                   );


// returns true if two SetStructs are the same, prints error if not
bool verify_set(const SetStruct inSet, const SetStruct goodSet);

#endif //GENERATE_DATA_H
