#ifndef PARSE_INPUTS_H
#define PARSE_INPUTS_H

#include <getopt.h>
#include <string.h>
#include <iostream>
#include "shortest_path.h"  //defines MAX_NEIGHBORS

#if defined(USE_INTERPRETED_OPENFHE) || defined(USE_YOSYS_INTERPRETED_OPENFHE)
  #define USE_OPENFHE
#endif

#ifdef USE_OPENFHE
#pragma message "using openFHE"

#include "palisade/binfhe/binfhecontext.h"
#include "palisade/binfhe/lwecore.h"
//using namespace lbcrypto;
#endif

void parse_inputs(int argc, char **argv, bool &simple_cost_flag,
                  bool &constrained_flag, bool &verbose,
#ifdef USE_OPENFHE
                  lbcrypto::BINFHEPARAMSET &param_set, lbcrypto::BINFHEMETHOD &method,
				  std::string &param_set_name, std::string &method_name,
#endif
                  unsigned int n_neighbors, std::string &out_fname);

#endif //PARSE_INPUTS_H
