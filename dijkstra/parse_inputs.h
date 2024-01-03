#ifndef PARSE_INPUTS_H
#define PARSE_INPUTS_H

#include <getopt.h>
#include <string.h>
#include <iostream>
#include "dijkstra.h"

//note this comes from other code where we use the same file for both cleartext
//and openfhe. hence the use of the #define

#if defined(USE_INTERPRETED_OPENFHE) || defined(USE_YOSYS_INTERPRETED_OPENFHE)
  #define USE_OPENFHE
#endif

#ifdef USE_OPENFHE
#include "openfhe/binfhe/binfhecontext.h"

//using namespace lbcrypto;
#endif

void parse_inputs(int argc, char **argv, bool &verbose,
#ifdef USE_OPENFHE
                  lbcrypto::BINFHE_PARAMSET &param_set,
				  std::string &param_set_name,
#endif
                  std::string &out_fname);

#endif //PARSE_INPUTS_H
