
#include <getopt.h>
#include <string.h>
#include "shortest_path.h"  //defines MAX_NEIGHBORS
#include "parse_inputs.h"

#ifdef USE_OPENFHE
#pragma message "using openFHE"

using namespace lbcrypto;
#endif

void parse_inputs(int argc, char **argv, bool &simple_cost_flag,
                  bool &constrained_flag, bool &verbose,

#ifdef USE_OPENFHE
                  lbcrypto::BINFHE_PARAMSET &param_set, 
				  std::string &param_set_name, 
#endif
                  unsigned int &n_neighbors, std::string &out_fname) {
  // manage the command line args
  int opt;  // option from command line parsing

  std::string usage_string =
    std::string("run ") + std::string(argv[0]) +
    std::string(
                " demo with settings (default value show in parenthesis):\n") +
    std::string("-c constrained shortest path flag (false)\n") +
    std::string("-n # neighbors [") + std::to_string(MAX_NEIGHBORS) + std::string("]\n") +
    std::string("-o output file name (out.csv)\n") +
#ifdef USE_OPENFHE
    std::string("-p parameter security set (TOY|STD128_LMKCDEY|STD128Q_LMKCDEY) [TOY]\n") +
#endif
    std::string("-s simple cost flag (false)\n") +
    std::string("-v verbose flag (false)\n") +
    std::string("\nh prints this message\n");

  int n_neighbors_in = MAX_NEIGHBORS;
  out_fname = "out.csv";

#ifdef USE_OPENFHE
  param_set_name = "TOY"; //defaults
#endif    

  while ((opt = getopt(argc, argv, "cn:o:p:svh")) != -1) {
    switch (opt) {
    case 'c':
      constrained_flag = true;
      std::cout << "constrained shortest path on" << std::endl;
      break;
 
    case 'n':
      n_neighbors_in = atoi(optarg);
      if (n_neighbors_in < 0) {
        n_neighbors = 1;
      } else if (n_neighbors_in > MAX_NEIGHBORS) {
        n_neighbors = MAX_NEIGHBORS;
      } else {
        n_neighbors = n_neighbors_in;
      }
      std::cout << "n_neighbors set to " << n_neighbors << std::endl;
      break;
    case 'o':
      out_fname = optarg;
      std::cout << "out file name \"" << out_fname << "\"" << std::endl;
      break;
    case 'p':
#ifdef USE_OPENFHE
      param_set_name = optarg;
      if (param_set_name == "TOY") {
        param_set = lbcrypto::TOY;
        std::cout << "using TOY" << std::endl;
      } else if (param_set_name == "STD128_LMKCDEY") {
        param_set = lbcrypto::STD128_LMKCDEY;
        std::cout << "using STD128_LMKCDEY" << std::endl;
      } else if (param_set_name == "STD128Q_LMKCDEY") {
        param_set = lbcrypto::STD128Q_LMKCDEY;
        std::cout << "using STD128Q_LMKCDEY" << std::endl;
      } else {
        std::cerr << "Error Bad Set chosen: " << param_set_name
				  << " should be TOY (defult) | STD128_LMKCDEY | STD128Q_LMKCDEY" << std::endl;
        exit(-1);
      }
#else
      std::cout << "BinFHE parameters are ignored for cleartext processing." << std::endl;
#endif
      break;
    case 's':
      simple_cost_flag = true;
      std::cout << "simple cost on" << std::endl;
      break;
    case 'v':
      verbose = true;
      std::cout << "verbose on" << std::endl;
      break;
    case 'h':
    default: /* '?' */
      std::cout << usage_string << std::endl;
      exit(0);
    }
  }
}

