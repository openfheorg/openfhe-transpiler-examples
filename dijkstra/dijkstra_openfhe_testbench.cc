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

#define PROFILE // need to define this for TIC,TOC to work
#include "openfhe/binfhe/binfhecontext.h"
#include "binfhecontext-ser.h"

#include "transpiler/data/openfhe_data.h"
#include "xls/common/logging/logging.h"
#include "parse_inputs.h" // these must be after the openfhe includes to compile

#ifdef USE_INTERPRETED_OPENFHE
#include "transpiler/examples/dijkstra/min_distance_interpreted_openfhe.h"
#include "transpiler/examples/dijkstra/set_src_distance_interpreted_openfhe.h"
#include "transpiler/examples/dijkstra/update_distance_interpreted_openfhe.h"
#elif defined(USE_YOSYS_INTERPRETED_OPENFHE)
#include "transpiler/examples/dijkstra/min_distance_yosys_interpreted_openfhe.h"
#include "transpiler/examples/dijkstra/set_src_distance_yosys_interpreted_openfhe.h"
#include "transpiler/examples/dijkstra/update_distance_yosys_interpreted_openfhe.h"

#else
#include "transpiler/examples/dijkstra/min_distance_openfhe.h"
#include "transpiler/examples/dijkstra/set_src_distance_openfhe.h"
#include "transpiler/examples/dijkstra/update_distance_openfhe.h"
#endif

using namespace lbcrypto;
using namespace std;
// A utility function to print the constructed distance
// array
void printSolution(int dist[])
{
  cout << "Vertex \t Distance from Source" << endl;
  for (int i = 0; i < MAX_VERTICIES; i++)
	cout << i << " \t" << dist[i] << endl;
}

// helper function for Dijkstra
// sets the initial minimum distance
// duplicate of minDistance transpiled function 
int minDistanceCleartext(int dist[MAX_VERTICIES],
						 bool sptSet[MAX_VERTICIES])
{
  
  // Initialize min value
	int min = MAX_COST, min_index = 0;
	for (int v = 0; v < MAX_VERTICIES; v++)
	  if (sptSet[v] == false && dist[v] <= min)
		min = dist[v], min_index = v;
	return min_index;
}

// helper function for Dijkstra
// updates minimum distance
// duplicate of updateDistance transpiled function 
void updateDistanceCleartext(bool sptSet[MAX_VERTICIES],
							 int graph[MAX_VERTICIES][MAX_VERTICIES],
							 int dist[MAX_VERTICIES],
							 int u){
  // Mark the picked vertex as processed
  sptSet[u] = true;
  
  // Update dist value of the adjacent vertices of the
  // picked vertex.
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


// Function that implements Dijkstra's single source
// shortest path algorithm for a graph represented using
// adjacency matrix representation


void dijkstraCleartext(int graph[MAX_VERTICIES][MAX_VERTICIES], int src,
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

	int u = minDistanceCleartext(dist, sptSet);
	std::cout << "count "<< count <<" u "<< u<<std::endl;
	updateDistanceCleartext(sptSet, graph, dist, u);

  }

}

void dijkstraEnc(OpenFheArrayRef<int, MAX_VERTICIES, MAX_VERTICIES> CT_graph,
				 OpenFheRef<int> CT_src,
				 OpenFheArrayRef<int, MAX_VERTICIES> CT_dist,
				 BinFHEContext cc, LWEPrivateKey sk)
{
  // CT_dist[MAX_VERTICIES]; // The output array. dist[i] will hold the
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
  

  //TODO move this out of this function? no sk in this function?
  
  OpenFheArray<bool,MAX_VERTICIES> CT_sptSet(cc);
  CT_sptSet.SetEncrypted(sptSet, sk);
  
  // Distance of source vertex from itself is always 0
  XLS_CHECK_OK(setSrcDistance(CT_dist, CT_src, cc));

  int u(0);
  OpenFhe<int> CT_u(cc);
  CT_u.SetEncrypted(u, sk);
  // Find shortest path for all vertices
  for (int count = 0; count < MAX_VERTICIES - 1; count++) {
	// Pick the minimum distance vertex from the set of
	// vertices not yet processed. u is always equal to
	// src in the first iteration.
	
	std::cout<< "minDistance "<< count << std::endl;
	std::cout << "count "<< count<<" dist ";
	auto tmp1 = CT_dist.Decrypt(sk);
	for (int j = 0; j < MAX_VERTICIES; j++) {
	  std::cout << tmp1[j] << " ";
	}
	std::cout<<std::endl;
	auto tmp2 = CT_sptSet.Decrypt(sk);
	std::cout << "count "<< count<<" sptSet ";
	for (int j = 0; j < MAX_VERTICIES; j++) {
	  std::cout << tmp2[j] << " ";
	}
	std::cout<<std::endl;
	XLS_CHECK_OK(minDistance(CT_u, CT_dist, CT_sptSet, cc));
	std::cout << "count "<< count <<" u "<< CT_u.Decrypt(sk) <<std::endl;
	std::cout<< "updateDistance "<< count << std::endl;
	XLS_CHECK_OK(updateDistance(CT_sptSet, CT_graph, CT_dist, CT_u, cc));
  }
  std::cout<< "done" << std::endl;	
  
}
///////////////////////////////////////////////////////////////////
// main driver code
int main(int argc, char** argv)
{

#ifdef USE_YOSYS_INTERPRETED_OPENFHE
  std::cout << "Probem, this does not run under yosys interpreted mode" << std::endl;
#endif
  
  lbcrypto::BINFHE_PARAMSET param_set(TOY);
  lbcrypto::BINFHE_METHOD method(AP);
  std::string param_set_name("TOY");
  std::string method_name("AP");
  
  TimeVar t;					// for timing
  double execTime(0.0);
  string out_fname("");
  bool verbose(false);
  parse_inputs(argc, argv, verbose,
               param_set, method, param_set_name, method_name,
               out_fname);
  
  ofstream outfile; //file for logging output
  ofstream sizeoutfile; //file for logging size output
  try {
	outfile.open(out_fname, ios::out | ios::trunc );
  } catch (const std::exception& e) {
    cerr << out_fname << " file open exception was caught, with message '" << e.what() << "'\n";
  }
  try {
	sizeoutfile.open(out_fname+".siz", ios::out | ios::trunc );
  } catch (const std::exception& e) {
    cerr << out_fname+".siz" << " file open exception was caught, with message '" << e.what() << "'\n";
  }

  outfile << "\"param\",\"method\",\"Dijkstra\" " << endl;

  outfile << param_set_name << "," << method_name << ",";

  sizeoutfile << "Using BinFHE "<<param_set_name <<", " << method_name <<endl;
  //generate FHE context and keys 
  if (verbose) {
    cout<< "Using BinFHE "<<param_set_name <<", " << method_name <<endl;
  }

  BinFHEContext cc = BinFHEContext();
  cc.GenerateBinFHEContext(param_set, method); //strictly for debugging

  std::cout << "Generating the secret key..." << std::endl;
  LWEPrivateKey sk = cc.KeyGen();		// Generate the secret key

  {
	// measure the size of the secret key
	std::ostringstream keystring;
	lbcrypto::Serial::Serialize(sk, keystring, SerType::BINARY);
	if (!keystring.str().size())
	  std:: cout << "Serialized eval sum key is empty" << endl;
    sizeoutfile << "Secret key size is " << keystring.str().size()  << " bytes" << std::endl;

  }

  std::cout << "Generating the bootstrapping keys..." << std::endl;
  cc.BTKeyGen(sk); // Generate the bootstrapping keys (refresh and switching keys)
  std::cout << "Completed key generation." << std::endl;

  {
	// measure the size of a switching key
	std::ostringstream switchstring;
	auto switchkey= cc.GetSwitchKey();
  
	lbcrypto::Serial::Serialize(switchkey, switchstring, SerType::BINARY);
	if (!switchstring.str().size())
	  std:: cout << "Serialized switchkey is empty" << endl;
    sizeoutfile << "switchkey size is " << switchstring.str().size()  << " bytes" << std::endl;
  }
  {
	// measure the size of a refresh key
	std::ostringstream refreshstring;
	auto refreshkey= cc.GetRefreshKey();
  
	lbcrypto::Serial::Serialize(refreshkey, refreshstring, SerType::BINARY);
	if (!refreshstring.str().size())
	  std:: cout << "Serialized refreshkey is empty" << endl;
    sizeoutfile << "refreshkey size is " << refreshstring.str().size()  << " bytes" << std::endl;
  }
   
  size_t CTsize;
  {
	// measure the size of a ciphertext
	std::ostringstream CTstring;
	auto ct = cc.Encrypt(sk, 1);
  
	lbcrypto::Serial::Serialize(ct, CTstring, SerType::BINARY);
	CTsize = CTstring.str().size();
	if (!CTsize)
	  std:: cout << "Serialized ciphertext is empty" << endl;
    sizeoutfile << "1 bit CT size is " << CTsize  << " bytes" << std::endl;
  }

  cout << "got here"<<endl;

  // Generate data
  
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

  //compute the distance output vector in plaintext.
  
  dijkstraCleartext(graph, 0, test_distance);

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
  // Encrypt the input and run encrypted algorithm. 
  

  int enc_test_distance[MAX_VERTICIES]={0};
	
  for (int i = 0; i < MAX_VERTICIES; i++)
	enc_test_distance[i] = MAX_COST;
	
  OpenFheArray<int, MAX_VERTICIES> CT_test_distance(cc);
  CT_test_distance.SetEncrypted(enc_test_distance, sk);

  OpenFheArray<int, MAX_VERTICIES, MAX_VERTICIES> CT_graph(cc);
  CT_graph.SetEncrypted(graph, sk);
  
  OpenFhe<int> CT_src(cc);
  CT_src.SetEncrypted(0, sk);

  TIC (t);
  dijkstraEnc(CT_graph, CT_src, CT_test_distance, cc, sk);
  execTime = TOC_MS(t);

  cout << "average time: " << execTime/1000.0 << " s" << std::endl;
  outfile << execTime <<",";

  auto tmp = CT_test_distance.Decrypt(sk);
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

  cout << "Done" << endl;
  outfile <<endl;
  
  outfile.close();// close the output file
  
  sizeoutfile.close();// close the size output file
  
  return 0;
}

