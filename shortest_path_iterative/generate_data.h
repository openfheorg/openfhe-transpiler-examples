#ifndef GENERATE_DATA_H
#define GENERATE_DATA_H

#include <getopt.h>
#include <string.h>
#include <iostream>
#include <random>

#include "shortest_path.h"


// generate data
// parameters
// input const bool simpleCostFlag if true costs are == path length.,
// input const bool constrainedFlag, if true path wit ignoreId gets set to MAX_COST
// input const bool verbose, if true print resulting data structures
// input const unsigned int ignoreId, the Id of the node to trigger constrained path
// input const unsigned int myId, my node's ID
// input/output unsigned int myCost, my node's cost, set to 1 if simpleCostFlag=true
// input const unsigned int MAX_COST, current larest cost (limited by bug in transpiler)
// output PathStruct paths[MAX_NEIGHBORS],
// output unsigned int costVec[MAX_NEIGHBORS], resulting cost vector
// output unsigned int minCost, resulting minimum cost
// output unsigned char minCostIndex, index of minimum cost path
// output PathStruct &goodOutput, result of final advertised path (with my node a cost added)

void generate_data(const unsigned int n_neighbors,
				   const bool simpleCostFlag, const bool constrainedFlag,
				   const bool verbose,
				   const unsigned int ignoreId, const unsigned int myId,
				   unsigned int &myCost,
				   PathStruct *paths,
				   unsigned int minCost, unsigned char minCostIndex,
				   PathStruct &goodOutput
                   );


// returns true if two PathStructs are the same, prints error if not
bool verify_path(const PathStruct inPath, const PathStruct goodPath);

#endif //GENERATE_DATA_H
