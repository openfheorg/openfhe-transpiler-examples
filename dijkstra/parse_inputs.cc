
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
				  lbcrypto::BINFHE_METHOD &method,
                  std::string &param_set_name, std::string &method_name,
#endif
                  std::string &out_fname) {
  // manage the command line args
  int opt;  // option from command line parsing

  std::string usage_string =
    std::string("run ") + std::string(argv[0]) +
    std::string(
                " demo with settings (default value show in parenthesis):\n") +
#ifdef USE_OPENFHE
    std::string("-m method (AP|GINX) [AP] \n") +
#endif
    std::string("-o output file name (out.csv)\n") +
#ifdef USE_OPENFHE
    std::string("-p parameter security set (TOY|STD128_OPT|STD128Q_OPT|STD192_OPT|STD192Q_OPT|STD256_OPT|STD256Q_OPT) [TOY]\n") +
#endif
    std::string("-v verbose flag (false)\n") +
    std::string("\nh prints this message\n");

  out_fname = "out.csv";

#ifdef USE_OPENFHE
  param_set_name = "TOY"; //defaults
  method_name = "AP";
#endif    

  while ((opt = getopt(argc, argv, "m:o:p:vh")) != -1) {
    switch (opt) {
    case 'm':
#ifdef USE_OPENFHE
      method_name = optarg;
      if (method_name == "GINX") {
        method = lbcrypto::GINX;
        std::cout << "using GINX" << std::endl;
      } else if (method_name == "AP") {
        method = lbcrypto::AP;
        std::cout << "using AP" << std::endl;
      } else {
        std::cerr << "Error Bad Method chosen" << std::endl;
        exit(-1);
      }
#else
      std::cout << "BinFHE parameters are ignored for cleartext processing." << std::endl;
#endif
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
      } else if (param_set_name == "STD128_OPT") {
        param_set = lbcrypto::STD128_OPT;
        std::cout << "using STD128_OPT" << std::endl;
      } else if (param_set_name == "STD128Q_OPT") {
        param_set = lbcrypto::STD128Q_OPT;
        std::cout << "using STD128Q_OPT" << std::endl;
      } else if (param_set_name == "STD192_OPT") {
        param_set = lbcrypto::STD192_OPT;
        std::cout << "using STD192_OPT" << std::endl;
      } else if (param_set_name == "STD192Q_OPT") {
        param_set = lbcrypto::STD192Q_OPT;
        std::cout << "using STD192Q_OPT" << std::endl;
      } else if (param_set_name == "STD256_OPT") {
        param_set = lbcrypto::STD256_OPT;
        std::cout << "using STD256_OPT" << std::endl;
      } else if (param_set_name == "STD256Q_OPT") {
        param_set = lbcrypto::STD256Q_OPT;
        std::cout << "using STD256Q_OPT" << std::endl;
      } else {
        std::cerr << "Error Bad Set chosen: " << param_set_name << std::endl;
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

