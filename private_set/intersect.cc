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

#include "private_set.h"
#include "intersect.h"

// inner loop of the intersect function (to be encrypted)
#pragma hls_top
void intersect_inner(SetStruct &outSet,
					 unsigned char q,
					 SetStruct oldSet,
					 SetStruct newSet,
					 unsigned char fflag){
	
  unsigned char newLen = newSet.setLength;

  // test to see if we need to do this inner loop and if not just copy data
  if ((q < oldSet.setLength)&&(oldSet.overflow == false)){
    #pragma hls_unroll yes
	for(unsigned char w = 0; w < MAX_SET_LEN; w++) {
	  if (w == newLen){
		//end of encrypted loop
		break;
	  }
	  
	  //checking if both arrays contain a particular element
	  if(oldSet.set[q] == newSet.set[w]) {
		if (fflag == MAX_SET_LEN) {
		  // we just overflowed
		  outSet.overflow = true;
		  break; //jump out of this loop 
		}
		outSet.set[fflag] = oldSet.set[q]; //save element, increment length
		outSet.setLength++;
		fflag = fflag+1;
		break;
	  }
	}
  } else { // just copy output
	outSet.setLength = oldSet.setLength;
	outSet.overflow = oldSet.overflow;
	#pragma hls_unroll yes
	for(unsigned char w = 0; w < MAX_SET_LEN; w++) {
	  outSet.set[w] = oldSet.set[w];
	}
  }
  return;
}
