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

#include "vector_min_index.h"


//note since we cannot use std::pair as a return value, we need to pass back min and int
//by reference, and since we cannot use pointers we need to use single element arrays.


#pragma hls_top
void vector_min_index(unsigned int a[MAX_NEIGHBORS], unsigned char& index) {

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
