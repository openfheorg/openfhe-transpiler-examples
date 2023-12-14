# Duality Labs Transpiler Experiments 

This body of code contains examples of encrypted network measurement
and control fuctions developed under DARPA funding for the I2O Open,
Programmable, Secure 5G (OPS5G) program.  They represent distributed
set operations that would be performed among a group of nodes in a
path, a simple BGP-like shortest path calculation, and a dijkstra
shortest path computation for a small network.

## Acknowledgements and Distribution 

Distribution Statement ‘A’ (Approved for Public Release, Distribution
Unlimited).  This work is supported in part by DARPA. The views,
opinions, and/or findings expressed are those of the author(s) and
should not be interpreted as representing the official views or
policies of the Department of Defense or the U.S. Government.

This work is supported by DARPA through contract number HR001120C0157. 
This work was conducted by Duality Technologies as a subcontractor to
USC Information Sciences Institute.

## Work in Progress Warning.

This repostiory is being migrated from a private repo, an we are step
by step verifying correct operation. each example will not if it has
not been verified yet. You may try these but be warned that they may
not work yet.

## Interacting with the Google Transpiler. 

Note all this code has only been run under linux. Windows is
unsupported at this time.

The Google Transpiler can be found at
https://github.com/google/fully-homomorphic-encryption. It runs within
its own bazel build system. To simplify things we downnload and build
the transplier in one directory, and this repository in another, then
softlink the directories together. We then build in the transpiler
repo. Our build scripts will move the resulting executables out of the bazel cache back into a bin directory in our repo. 

All these projects should be soft linked to the
`transpiler\experiments` directory. 

Note `bazel` must be run above these directories, i.e. in the
`experiments` directory in order to correctly operate.

Note also that `bazel` can trigger a rebuild of the build system if
any of the upstream sources get updated -- and this can take a very
long time to complete.

`openfhe.BUILD.64` and `openfhe.BUILD.32` are optimized files for
building 64 bit and 32 bit versions of OpenFHE. One simply replaces
the `patches/openfhe.BUILD` file in the transpiler directory with one
of these files (changing the name to `openfhe.BUILD`). For almost all parameter sets we recommend using 32 bit OpenFHE (see the OpenFHE documentation for which parameter sets require 64bit arithmetic).

## Using the transpiler docker image

While cloning the transpiler works well, some prefer using docker. 
You can build the docker image

debian:
`docker build -t google-fhe-transpiler -f docker/debian-bullseye.Dockerfile .`

Ubuntu: 
`docker build -t google-fhe-transpiler -f docker/ubuntu-jammy.Dockerfile  .`

Which builds the (very large) docker file.

You can then run the docker as follows, linking in this code as a
mounted directory (which gets rsynched automatically). Assuming you install this repo in `/home/{yournamehere}/Documents/openfhe-transpiler-experiments` :

`docker run --rm -i -v /home/{yournamehere}/Documents/openfhe-transpiler-experiments:/home/{yournamehere}/Documents/openfhe-transpiler-experiments -t google-fhe-transpiler bash`

note that when running the docker image, you have to make new soft
links to the right project directories (i.e. `shortest_path`) within
the `examples` subdirectory of the docker image of the transpiler
tree, every time you run the docker image (as it resets).

# Network Measurement/Control Sub-projects

In general each of these subprojects can generate cleartext (with and
without yosys interpreted mode) and openfhe executables (with and
without yosys interpeted mode and straight XLS interpreted mode ). In
general we suggest always using yosys interpeted mode because a) yosys
generally makes smaller circuits and b) yosys interpreted will execute
gates in parallel across all available cores, for a much faster run
time.

## shortest_path [not yet verified]

This is an implementation of a simplified BGP protocol. There is no
I/O, only computation is benchmarked.

Shortest path was the first project we built using the transpiler, and
we learned alot about what works and what does not. Initially we tried
putting the entire encrypted algorithm into one set of functions
(`shortest_path_singlecall`) which led to huge circuits that take a
long time to build and execute. We then split it into multiple calls
(`shortest_path_multicall`), which builds faster, but still exhibits
combinatorial explosions in the loop circuits. Our final version
iterates over encrypted function calls in a software loop (`shortest_path_iterative`)

`shortest_path_singlecall` is a single call version -- it generates
huge circuits that take a long time. Provided only for pedagogical purposes.

`shortest_path_multicall` is a multiple call version, which still has poor runtime performance. 

`shortest_path_iterative` is the final version which
combines a software control loop with encrypted function calls. It is the most efficient way to work. 

One of these directories should be linked to `shortest_path` in the transpiler
directory.

Then one can run from the `transpiler/examples` directory using commands such as 

`bazel run //transpiler/examples/shortest_path:shortest_path_yosys_interpreted_openfhe_testbench -- -v -m GINX -o /home/palisade/Documents/transpiler-experiments/shortest_path_iterative/TOY-GINX.csv`

Note running the following will list all possible command line arguments

`bazel run //transpiler/examples/shortest_path:shortest_path_yosys_interpreted_openfhe_testbench -- -h`


## Private Set Processing Along a Path [verified]

This is an implementation of private set processing along a chain of
network nodes. It performs intersection or union on a vector of data
coming into a node with another vector supplied by the node. The
result is passed on to the next neighboring node in a path. The number
of nodes simulated is a command line parameter.  There is no I/O, only
computation is benchmarked.

The maximum vector length `MAX_SET_LEN` is specified in
`private_set.h` and is currently set to 20. Reducing this will result
in faster compile and run times. Increasing it may create problems
with compilation. YMMV.

If an intersecton causes the resulting vector's length to exceed
`MAX_SET_LEN`, an overflow flag is set to true.

`private_set` uses a software control loop calling smaller encrypted
modules to avoid the circuit combinatorial overload. It supports
intersection and union. 

This directory should be linked to `private_set` in the transpiler
directory.

Then one can run from the `transpiler/examples` directory manually using
commands such as

`bazel run //transpiler/examples/private_set:private_set_yosys_interpreted_cleartext_testbench -- -n 3`

Note large numbers of n take a LONG time to run. 

The following script will generate all cleartext versions and place them in a `bin` directory in `private_set`.

> `private_set/compile_cleartext.sh`

and openfhe versions using 

> `private_set/compile_openfhe.sh`

Do not worry about warning messages for Slow Loop unrolling.

Note, for some unknown reason, this code compiles for
`private_set_yosys_interpreted_openfhe_testbench` but gcc segfaults
when compiling `private_set_openfhe_testbench`, so that target is
removed from the compile script.

Once compiled with the script the resutling binary is in the `private_set/bin` directory

run `bin/private_set_cleartext_testbench` demo with settings (default value show in parenthesis):

`-c force common element`

`-d debug flag (false)`

`-n # neighbors [20]`

`-o output file name (out.csv)`

`-p parameter security set (TOY|STD128_LMKCDEY|STD128Q_LMKCDEY) [TOY]  {not in cleartext}
`-s seed_integer [1]`

`-v verbose flag (false)`

`-h prints help message`



To run the openFHE versions you may need to copy/link the following libraries to `private_set\bin` 

```
libOPENFHEbinfhe.so
libOPENFHEbinfhe.so.1
libOPENFHEcore.so
libOPENFHEcore.so.1
libOPENFHEpke.so
libOPENFHEpke.so.1
```

or set `LD_LIBRARY_PATH` to the appropriate location. You may need to
install the equivalent version of openFHE on your system.


## dijkstra [not yet verified]

This is an implementation of the Dijkstra shortest path
algorithm. Note this code has a hardwired network, and hardwired
source node and is basically a proof of concept. Note in the future we will release a dijkstra version that can run with 100s of nodes.

# Todo

[See the Todo File for planned work ](Todo.md)
