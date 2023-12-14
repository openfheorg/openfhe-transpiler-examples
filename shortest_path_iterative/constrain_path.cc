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

#include "constrain_path.h"

#pragma hls_top
void constrain_path(PathStruct &path, const unsigned int ignoreId){

  //if ID exists in a path, set that path cost to MAX (i.e. ignore it)
  bool omit(false);
#pragma hls_unroll yes
  for (unsigned char i = 0; i < MAX_PATH_LEN; i++) {
	if (path.path[i] == ignoreId){
	  path.cost = MAX_COST;
	} 
  }
  return;
}
