
#include <getopt.h>
#include <string.h>
#include "dijkstra.h" 
#include "parse_inputs.h"

#ifdef USE_OPENFHE
using namespace lbcrypto;
#endif

void parse_inputs(int argc, char **argv, bool &verbose,

#ifdef USE_OPENFHE
                  lbcrypto::BINFHE_PARAMSET &param_set,
                  std::string &param_set_name,
#endif
                  std::string &out_fname) {
  // manage the command line args
  int opt;  // option from command line parsing

  std::string usage_string =
    std::string("run ") + std::string(argv[0]) +
    std::string(
                " demo with settings (default value show in parenthesis):\n") +
    std::string("-o output file name (out.csv)\n") +
#ifdef USE_OPENFHE
    std::string("-p parameter security set (TOY|STD128_LMKCDEY|STD128Q_LMKCDEY) [TOY]\n") +
#endif
    std::string("-v verbose flag (false)\n") +
    std::string("\nh prints this message\n");

  out_fname = "out.csv";

#ifdef USE_OPENFHE
  param_set_name = "TOY"; //defaults
#endif    

  while ((opt = getopt(argc, argv, "o:p:vh")) != -1) {
    switch (opt) {

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

