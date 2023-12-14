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

#include "compute_shortest_path.h"

#if 1
void create_cost_vector(const PathStruct paths[MAX_NEIGHBORS], unsigned int costVec[MAX_NEIGHBORS]){
  #pragma hls_unroll yes    
  for (unsigned char i = 0; i < MAX_NEIGHBORS; i++){
	costVec[i] = paths[i].cost;
  }
  return;
}

void vector_min_index(const unsigned int a[MAX_NEIGHBORS], unsigned int& min, unsigned char& index) {

  min = a[0];
  index = 0;
  #pragma hls_unroll yes    
  for (unsigned char i = 1; i < MAX_NEIGHBORS; i++){
	if (a[i] < min) {
	  min = a[i];
	  index = i;
	}
  }
  return;
}

void select_path(const PathStruct inPaths[MAX_NEIGHBORS],
				const unsigned char index, PathStruct& out){
  #pragma hls_unroll yes    
  for ( int i = 0; i < MAX_NEIGHBORS; i++){
	if (i == index){
	  out.cost = inPaths[i].cost;
	  out.pathLength = inPaths[i].pathLength;
      #pragma hls_unroll yes    
	  for (int j = 0; j < MAX_PATH_LEN; j++){
		out.path[j] = inPaths[i].path[j];
	  }
	}
  }
  return;
}

void update_path(PathStruct &path,  const int myID, const unsigned int myCost) {
  path.cost += myCost;
  path.path[path.pathLength] = myID;
  path.pathLength++;
}

#pragma hls_top
void compute_shortest_path(const PathStruct paths[MAX_NEIGHBORS], const int myID, const unsigned int myCost,  PathStruct &outPath ){
  unsigned int costVec[MAX_NEIGHBORS];

  create_cost_vector(paths, costVec);
  
  unsigned int min;
  unsigned char index;
  vector_min_index(costVec, min,  index);

  select_path(paths, index, outPath);

  update_path(outPath, myID, myCost);

  return;
}
#else
void create_cost_vector(const PathStruct paths[MAX_NEIGHBORS], unsigned int costVec[MAX_NEIGHBORS]){
  #pragma hls_unroll yes    
  for (unsigned char i = 0; i < MAX_NEIGHBORS; i++){
	costVec[i] = paths[i].cost;
  }
  return;
}

void vector_min_index(const unsigned int a[MAX_NEIGHBORS], unsigned char& index) {

  unsigned int min = a[0];
  index = 0;
  #pragma hls_unroll yes    
  for (unsigned char i = 1; i < MAX_NEIGHBORS; i++){
	if (a[i] < min) {
	  min = a[i];
	  index = i;
	}
  }
  return;
}

PathStruct select_path(const PathStruct inPaths[MAX_NEIGHBORS],
				const unsigned char index){
  return inPaths[index];
}

void update_path(PathStruct &path,  const int myID, const unsigned int myCost) {
  path.cost += myCost;
  path.path[path.pathLength] = myID;
  path.pathLength++;
}

#pragma hls_top
void compute_shortest_path(const PathStruct paths[MAX_NEIGHBORS], const int myID, const unsigned int myCost,  PathStruct &outPath ){
  unsigned int costVec[MAX_NEIGHBORS];

  create_cost_vector(paths, costVec);
  
  unsigned char index;
  vector_min_index(costVec, index);
  //cannot use this approach generates
  // UNIMPLEMENTED: Function __builtin_memcpy used but has no body
  
  outPath = select_path(paths, index);

  update_path(outPath, myID, myCost);

  return;
}

#endif
