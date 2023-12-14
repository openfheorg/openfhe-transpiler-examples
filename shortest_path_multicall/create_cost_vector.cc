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

#include "create_cost_vector.h"

#pragma hls_top
void create_cost_vector(const PathStruct paths[MAX_NEIGHBORS], unsigned int costVec[MAX_NEIGHBORS]){
  #pragma hls_unroll yes    
  for (unsigned char i = 0; i < MAX_NEIGHBORS; i++){
	costVec[i] = paths[i].cost;
  }
  return;
}
