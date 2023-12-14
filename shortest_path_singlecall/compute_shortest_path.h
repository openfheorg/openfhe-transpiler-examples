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

#ifndef FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_COMPUTE_SHORTEST_PATH_INDEX_H_
#define FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_COMPUTE_SHORTEST_PATH_INDEX_H_

#include "shortest_path.h"

void compute_shortest_path(const PathStruct paths[MAX_NEIGHBORS], const int myID, const int myCost,  PathStruct &outPath );

#endif  // FULLY_HOMOMORPHIC_ENCRYPTION_TRANSPILER_EXAMPLES_COMPUTE_SHORTEST_PATH_INDEX_H_



