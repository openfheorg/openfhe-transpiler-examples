// C++ program for Dijkstra's single source shortest path
// algorithm. The program is for adjacency matrix
// representation of the graph
// this Transpiler example utilizes code originally published 
// from https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
// The orignal dijkstra code is contributed by shivanisinghss2110
// modified for transpiler use D, Cousins Duality Technologies inc. 

// Changes Copyright 2022 Duality Technologies Inc.
// D. Cousins
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

#include <stdint.h>
#include <iostream>
#include <limits.h>
#include "dijkstra.h"

#if 1

#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
#include "transpiler/examples/dijkstra/min_distance_yosys_interpreted_cleartext.h"
#include "transpiler/examples/dijkstra/set_src_distance_yosys_interpreted_cleartext.h"
#include "transpiler/examples/dijkstra/update_distance_yosys_interpreted_cleartext.h"

#else
#include "transpiler/examples/dijkstra/min_distance_cleartext.h"
#include "transpiler/examples/dijkstra/set_src_distance_cleartext.h"
#include "transpiler/examples/dijkstra/update_distance_cleartext.h"
#endif

#include "min_distance.h"
#include "update_distance.h"

#endif

using namespace std;
// A utility function to print the constructed distance
// array
void printSolution(int dist[])
{
	cout << "Vertex \t Distance from Source" << endl;
	for (int i = 0; i < MAX_VERTICIES; i++)
		cout << i << " \t" << dist[i] << endl;
}

// Function that implements Dijkstra's single source
// shortest path algorithm for a graph represented using
// adjacency matrix representation

void dijkstra(int graph[MAX_VERTICIES][MAX_VERTICIES], int src,
			  int dist[MAX_VERTICIES])
{
								//int dist[MAX_VERTICIES]; // The output array. dist[i] will hold the
								// shortest
								// distance from src to i

	bool sptSet[MAX_VERTICIES]; // sptSet[i] will be true if vertex i is
					// included in shortest
	// path tree or shortest distance from src to i is
	// finalized

	// Initialize all distances as INFINITE and stpSet[] as
	// false
	for (int i = 0; i < MAX_VERTICIES; i++)
		dist[i] = MAX_COST, sptSet[i] = false;

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < MAX_VERTICIES - 1; count++) {
		// Pick the minimum distance vertex from the set of
		// vertices not yet processed. u is always equal to
		// src in the first iteration.
	  std::cout << "count "<< count<<" dist ";
	  for (int j = 0; j < MAX_VERTICIES; j++) {
		std::cout << dist[j] << " ";
	  }
	  std::cout<<std::endl;
	  std::cout << "count "<< count<<" sptSet ";
	  for (int j = 0; j < MAX_VERTICIES; j++) {
		std::cout << sptSet[j] << " ";
	  }
	  std::cout<<std::endl;

		int u = minDistance(dist, sptSet);
		std::cout << "count "<< count <<" u "<< u<<std::endl;
		updateDistance(sptSet, graph, dist, u);

	}

}
// transpiler version of the dijkstra algorithm
void dijkstra_trans(EncodedArrayRef<int, MAX_VERTICIES, MAX_VERTICIES> CT_graph,
					EncodedRef<int> CT_src,
					EncodedArrayRef<int, MAX_VERTICIES> CT_dist)
{
  //int dist[MAX_VERTICIES]; // The output array. dist[i] will hold the
  // shortest
  // distance from src to i
  
  bool sptSet[MAX_VERTICIES]; // sptSet[i] will be true if vertex i is
  // included in shortest
  // path tree or shortest distance from src to i is
  // finalized
  
  // Initialize all distances as INFINITE and stpSet[] as
  // false
  for (int i = 0; i < MAX_VERTICIES; i++)
	sptSet[i] = false;
  
  EncodedArray<bool,MAX_VERTICIES> CT_sptSet;
  CT_sptSet.Encode(sptSet);

  std::cout << "here 1"<< std::endl;
  
  // Distance of source vertex from itself is always 0
  XLS_CHECK_OK(setSrcDistance(CT_dist, CT_src));
  std::cout << "here 2"<< std::endl;
  int u(0);
  Encoded<int> CT_u(u);
  // Find shortest path for all vertices
  for (int count = 0; count < MAX_VERTICIES - 1; count++) {
	// Pick the minimum distance vertex from the set of
	// vertices not yet processed. u is always equal to
	// src in the first iteration.
	
	std::cout<< "minDistance "<< count << std::endl;
	std::cout << "count "<< count<<" dist ";
	auto tmp1 = CT_dist.Decode();
	for (int j = 0; j < MAX_VERTICIES; j++) {
	  std::cout << tmp1[j] << " ";
	}
	std::cout<<std::endl;
	auto tmp2 = CT_sptSet.Decode();
	std::cout << "count "<< count<<" sptSet ";
	for (int j = 0; j < MAX_VERTICIES; j++) {
		std::cout << tmp2[j] << " ";
	}
	std::cout<<std::endl;
	XLS_CHECK_OK(minDistance(CT_u, CT_dist, CT_sptSet));
	std::cout << "count "<< count <<" u "<< CT_u.Decode() <<std::endl;
	std::cout<< "updateDistance "<< count << std::endl;
	XLS_CHECK_OK(updateDistance(CT_sptSet, CT_graph, CT_dist, CT_u));
  }
  std::cout<< "done" << std::endl;	
  
}

// driver code
int main()
{

#ifdef USE_YOSYS_INTERPRETED_CLEARTEXT
  std::cout << "Probem, this does not run under yosys interpreted mode" << std::endl;
#endif

	/* Let us create the example graph discussed above */
	int graph[MAX_VERTICIES][MAX_VERTICIES] = {
	  { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
	  { 4, 0, 8, 0, 0, 0, 0, 11, 0 },
	  { 0, 8, 0, 7, 0, 4, 0, 0, 2 },
	  { 0, 0, 7, 0, 9, 14, 0, 0, 0 },
	  { 0, 0, 0, 9, 0, 10, 0, 0, 0 },
	  { 0, 0, 4, 14, 10, 0, 2, 0, 0 },
	  { 0, 0, 0, 0, 0, 2, 0, 1, 6 },
	  { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
	  { 0, 0, 2, 0, 0, 0, 6, 7, 0 }
	};

	int true_distance[MAX_VERTICIES] = { 0, 4, 12, 19, 21, 11, 9, 8, 14};

	std::cout<< "Plaintext version "<<  std::endl;
	
	int test_distance[MAX_VERTICIES]={0};

	for (int i = 0; i < MAX_VERTICIES; i++)
	  test_distance[i] = MAX_COST;
	
	dijkstra(graph, 0, test_distance);

	// print the constructed distance array

    printSolution(test_distance);

    bool good = true;
	for (int i = 0; i < MAX_VERTICIES; i++){
	  good &= (test_distance[i] == true_distance[i]);
	}
	if (good) {
	  cout << "test passed" <<endl;
	} else {
	  cout << "test failed" <<endl;
	  cout << "vertex\ttrue\toutput"<<endl;
	  
	  for (int i = 0; i < MAX_VERTICIES; i++){
		cout << i <<"\t"<< true_distance[i] <<"\t"<< test_distance[i] <<endl;
	  }
	}


	int enc_test_distance[MAX_VERTICIES]={0};
	
	for (int i = 0; i < MAX_VERTICIES; i++)
	  enc_test_distance[i] = MAX_COST;
	
	EncodedArray<int, MAX_VERTICIES> CT_test_distance;
	CT_test_distance.Encode(enc_test_distance);

	EncodedArray<int, MAX_VERTICIES, MAX_VERTICIES> CT_graph;
	CT_graph.Encode(graph);
	Encoded<int> CT_src(0);
	
	dijkstra_trans(CT_graph, CT_src, CT_test_distance);

	auto tmp = CT_test_distance.Decode();
	// print the constructed distance array

	int out_test_distance[MAX_VERTICIES]={0};
	for (int i = 0; i < MAX_VERTICIES; i++)
	  out_test_distance[i] = tmp[i];

    printSolution(out_test_distance);

    good = true;
	for (int i = 0; i < MAX_VERTICIES; i++){
	  good &= (out_test_distance[i] == true_distance[i]);
	}
	if (good) {
	  cout << "test passed" <<endl;
	} else {
	  cout << "test failed" <<endl;
	  cout << "vertex\ttrue\toutput"<<endl;
	  
	  for (int i = 0; i < MAX_VERTICIES; i++){
		cout << i <<"\t"<< true_distance[i] <<"\t"<< out_test_distance[i] <<endl;
	  }
	}



	return 0;
}

