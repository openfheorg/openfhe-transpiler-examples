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

#include "min_distance.h"

// A utility function to find the vertex with minimum
// distance value, from the set of vertices not yet included
// in shortest path tree
#pragma hls_top
int minDistance(int dist[MAX_VERTICIES], bool sptSet[MAX_VERTICIES])
{

	// Initialize min value
	int min = MAX_COST, min_index = 0;
    #pragma hls_unroll yes
	for (int v = 0; v < MAX_VERTICIES; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

