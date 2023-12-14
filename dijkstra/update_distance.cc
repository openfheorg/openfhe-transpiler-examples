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

#include "update_distance.h"

#pragma hls_top
void updateDistance(bool sptSet[MAX_VERTICIES],
					int graph[MAX_VERTICIES][MAX_VERTICIES],
					int dist[MAX_VERTICIES],
					int u){
  // Mark the picked vertex as processed
  sptSet[u] = true;
		
  // Update dist value of the adjacent vertices of the
  // picked vertex.
  #pragma hls_unroll yes
  for (int v = 0; v < MAX_VERTICIES; v++)

	// Update dist[v] only if is not in sptSet,
	// there is an edge from u to v, and total
	// weight of path from src to v through u is
	// smaller than current value of dist[v]
	if (!sptSet[v] && graph[u][v]
		&& dist[u] != MAX_COST
		&& dist[u] + graph[u][v] < dist[v])
	  dist[v] = dist[u] + graph[u][v];
}
