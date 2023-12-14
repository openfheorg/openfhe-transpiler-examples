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

#include "create_constrained_cost_vector.h"
//#include <stdint.h>
//#define UINT_MAX   4294967295U  //because we cannot find <limits.h>
//#define UINT_MAX   ((unsigned int)(0xffffffffffffffffU))  //because we cannot find <limits.h> -- note fails yosys
#define UINT_MAX   99999999
#pragma hls_top
void create_constrained_cost_vector(const PathStruct paths[MAX_NEIGHBORS],
									const unsigned int ignoreId,
									unsigned int costVec[MAX_NEIGHBORS]){
  //if ID exists in a path, set that path cost to MAX (i.e. ignore it)
  bool omit(false);
#pragma hls_unroll yes
  for (unsigned char i = 0; i < MAX_NEIGHBORS; i++){
	omit = false;
#pragma hls_unroll yes
	for (unsigned char j = 0; j < MAX_PATH_LEN; j++) {
	  if (paths[i].path[j] == ignoreId){
		omit = true;
	  } 
	}
	//costVec[i] = omit?UINT_MAX:paths[i].cost; //faiing in yosys
	if (omit){
	  //costVec[i] = 65536*256; //failed
	  costVec[i] = 65536*256-1; //passed
	  //costVec[i] = UINT_MAX;
	}else{
	  costVec[i] = paths[i].cost;
	}
  }
  return;
}
