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

## shortest_path 

This is an implementation of a simplified BGP protocol. There is no
I/O, only computation is benchmarked.

The computing node receives information from a set of upstream
neighbor nodes containing path cost information. The computing node
selects the minimum cost vector, adds themselves to it, and then sends
the output downstream, all encrypted.

Pseudo code:

1. The compute node gets an encrypted cost and encrypted path-id structure from each upstream neighbor. The structure contains : path cost, path length, vector of node IDs with the neighbor at the last element of the vector.

2. Create new vector of encrypted costs of all neighbors.

3. Find index of the minimum cost vector.

4. Loop over all structures (loop variable is `this_index`):

4.1 Generate Boolean variable `test` such that `test = (this index == encrypted index)`. 

4.2 Multiply all values in the struct by `test` and add struct to an
accumulated updated version of the struct.  Thus, the accumulated
struct contains only the selected path cost, path length and vector
of IDs (note zero is considered an invalid null node).

5. Then the compute node adds their cost to the final cost, increments the vector length and inserts their id at the end of the vector. 

6. The compute node passes the new encrypted cost and path id struct downstream (this is implied).

the `-c` constraint flag when true, sets a particular node ID's cost
to maximum, preventing it from being included in the computation. This
node number is set using `ignoreId` in the main prgraam.

Shortest path was the first project we built using the transpiler, and
we learned alot about what works and what does not. Initially we tried
putting the entire encrypted algorithm into one set of functions
(`shortest_path_singlecall`) which led to huge circuits that take a
long time to build and execute. This is primarily due to double loop unrolling. 

We then split it into multiple calls, 
(`shortest_path_multicall`), which builds faster, but still exhibits
combinatorial explosions in the single loop unrolled circuits. Our final version
iterates over encrypted function calls in a software loop (`shortest_path_iterative`)

`shortest_path_singlecall` is a single call version where all
computation on the input data is done in one encrypted function. It
generates huge circuits that take a long time. Provided only for
pedagogical purposes.

There is one encrypted function `compute_shortest_path()`.

`shortest_path_multicall` is a multiple call version, which still has
poor runtime performance. Again provided only for pedagogical
purposes.

The following encrypted functions are used:

`create_cost_vector()` and 
`create_constrained_cost_vector()` build the initial encrypted input.

`vector_min_index()`: idenifies the index of the minimum element of a vector. 

Shortest path: `select_path()`: select lowest cost of two paths.

Path advertisement: `update_path()`: adds our nodeID and cost to selected path.


`shortest_path_iterative` is the final version developed which
combines a software control loop with encrypted function calls. It is
the most efficient way to work. The following descriptions refer only to this version. 

One of these directories should be linked to `shortest_path` in the transpiler
directory.

Then one can run from the `transpiler/examples` directory using commands such as 

`bazel run //transpiler/examples/shortest_path:shortest_path_yosys_interpreted_openfhe_testbench -- -v  -o {full path to output directory}/output.csv`

the `-c` constraint flag when true, sets the local node's path cost to maximum.

The output files log runtimes and sizes of ciphertexts of the OpenFHE version. 

Note running the following will list all possible command line
arguments

`bazel run //transpiler/examples/shortest_path:shortest_path_cleartext_testbench -- -h`
`bazel run //transpiler/examples/shortest_path:shortest_path_yosys_interpreted_openfhe_testbench -- -h`



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

The number of simulated nodes in the process for singlecall and multicall versoins is set in
`shortest_paths.h`. Increasing this will increase compile and runtime.

Note in the interative version, one can set the number of nodes at
runtime by using the `-n` flag. The maximum number of nodes is still
set in `shortest_paths.h`.


## Private Set Processing Along a Path

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


## Dijkstra

This is an implementation of the Dijkstra shortest path algorithm,
originally published // from
https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
. The orignal dijkstra code is contributed by shivanisinghss2110. Note
this code has a hardwired network, and hardwired source node and is
basically a proof of concept to show that network algorithms can be
executed in encrypted form using the transplier. Note in the future we
will release a version that can run with hundreds of nodes. 

This version contains three encrypted functions, two of which are run
in a software loop over all verticies. Those two have unrolled loops,
so that puts a practical limit on the size of the network that can be
processed.


# Todo

[See the Todo File for planned work ](Todo.md)
