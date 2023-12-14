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

#ifndef FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_PRIVATE_SET_H_
#define FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_PRIVATE_SET_H_

// 20 works for cleartext,  
// segfaults on 64G machine for private_set_openfhe_testbench but is ok for
// private_set_yosys_interpreted_openfhe_testbench


const unsigned char MAX_SET_LEN = 20; 

// const unsigned char MAX_SET_LEN = 5;  // for fast debug

const unsigned char MAX_NEIGHBORS = 20;

struct SetStruct{
  bool overflow;
  unsigned char setLength;
  unsigned int set[MAX_SET_LEN];
};

#endif  // FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_PRIVATE_SET_H_



