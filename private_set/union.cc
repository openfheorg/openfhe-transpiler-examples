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
#include "union.h"

// inner loop of the union function (to be encrypted)
#pragma hls_top
void union_inner(SetStruct &outSet,
				 const unsigned char k,
				 const SetStruct oldSet,
				 const SetStruct newSet,
				 unsigned char &fflag){

  const unsigned char oldLen = oldSet.setLength; //this does not ever change
  const unsigned char newLen = newSet.setLength;
  
  if ((k < newLen)&&(outSet.overflow == false)){ //do not bother to do this loop
	
    #pragma hls_unroll yes
	for(unsigned char m = 0; m < MAX_SET_LEN; m++) {
	  if (m == oldLen){
		break; //do not compute past old length
	  }
	  
	  //eliminating common elements among the given arrays
	  if(newSet.set[k] == outSet.set[m]) {
		break; //already common, skip
	  } else if (m == oldLen-1) {
		if (fflag == MAX_SET_LEN) {
		  // we just overflowed
		  outSet.overflow = true;
		  break; //jump out of this loop 
		}
		outSet.set[fflag] = newSet.set[k]; // incorporate and increment lenght
		outSet.setLength++;
		fflag = fflag+1;
	  }
	}
  } 
}
