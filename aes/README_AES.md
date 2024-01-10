# AES 
This is a subdirectory in the repository of code containing examples
of AES running under FHEW and the google transpiler . All work was
generated under DARPA funding for the Data Protection in Virtual
Environments (DPRIVE) program.

## Acknowledgements and Distribution 

Distribution Statement ‘A’ (Approved for Public Release, Distribution
Unlimited).  This work is supported in part by DARPA. The views,
opinions, and/or findings expressed are those of the author(s) and
should not be interpreted as representing the official views or
policies of the Department of Defense or the U.S. Government.

This work is supported by DARPA through contract number HR0011-21-9-0003. 
This work was conducted by Duality Technologies as the Prime contractor.

## Interacting with the Google Transpiler. 

The Google Transpiler is a valuable research tool, but requires using
a specific build structure, and requires all code to reside within
their build tree. Rather than do that, we developed an alternative
approach where we use a soft link of our repository code into their
build tree within a docker image.

The `aes` directory should be soft linked to the
`transpiler\experiments` directory.

Note `bazel` must be run above these directories, i.e. in the
experiments directory in order to correctly operate.

Note also that `bazel` can trigger a rebuild of the build system if
any of the upstream sources get updated -- and this can take a very
long time to complete.

`openfhe.BUILD.64` and `openfhe.BUILD.32` are optimized files for
building 64 bit and 32 bit versions of OpenFHE. One simply replaces
the `patches/openfhe.BUILD` file in the transpiler directory with one
of these files (changing the name to `openfhe.BUILD`).

## AES Google Transpiler (GT) Experiments

This is an initial baseline for converting AES to TFHE.  The original
code comes from the `tiny-AES-C` project
https://github.com/kokke/tiny-AES-c, which is an open-source library
of AES code that was optimized for small resource compute
platforms. Specifically, their website says, "This library is designed
for small code size and simplicity, intended for cases where small
binary size, low memory footprint and portability is more important
than high performance." This makes it an idea source for experimenting
with the transpiler as small code/memory footprint would hopefully
lead to smaller resource utilization in generated encrypted circuits.
`tiny-AES-C` is published in the public domain. For more information,
please refer to <http://unlicense.org/>.

Note tiny-AES-C does not support GCM mode, but there are references to
other sources [here](https://github.com/kokke/tiny-AES-c/issues/16
"Link to GCM discussion").

The GT is an open-source project from Google whose goal is to make FHE
more accessible to a wider community. Basically, it uses various open-
source tools to convert specific functions to Boolean circuits, and
their input/output to special data structures representing either
cleartext or encrypted single bits then executes those circuits in
their plaintext or encrypted form.  There are two main ways of writing
code that use different backends. The resulting code is similar in
structure, but different in the particular syntax required.

1) the cleartext backend -- This encodes C++ data structures and
variables into internal data structures composed exclusively  of Booleans
(bits) and evaluates all resulting circuits in the clear for rapid
development and debugging.

1) an encrypted backend, either TFHE or OpenFHE -- This encrypts C++
data structures and variables into internal data structures composed
exclusivly of Ciphertexts containing a Boolean (one bit) and evaluates
all resulting circuits using encrypted operations of the underlying
backend. We use the OpenFHE backend exclusively in this project.

Additionally, the same cleartext or openfhe source code can be
compiled in different ways:

1) cleartext or openfhe - circuits are generated with
Google's XLS hardware design language system. The executable runs on
one core only, executing one gate at a time. Running OpenFHE this way
is extremely slow.

1) interpreted\_openfhe - as above, circuits are generated with
Google's XLS hardware design language system.  The executable runs on
all cores available, executing one gate per core giving a faster
overall runtime. This option is not available for cleartext.

1) yosys\_interpreted\_cleartext or yosys\_interpreted\_openfhe -
circuits are generated with the open source Yosys tool chain. Google
states that Yosys is better at generating optimized circuits than
XLS. The executable runs on all cores available, executing one gate per core
giving a faster overall runtime. This is the preferred mode of operation.

The code from tiny-AES has been converted in several ways in order to
work with the GT:
	
* The GT supports only a subset of C++ code, changes have been made to
  compile the code with that system.

** The use of pointers is not allowed. When data structures need to be
passed to encrypted functions as arguments, C++ references are used
instead. 

** All encrypted arrays need to be of fixed length. 

** Because of the above two restrictions, pointers used as array
references have been replaced with references to fixed arrays,
sometimes requiring array dimensions to be passed into encrypted
functions as encrypted input variables.

** While loops and for loops with variable end conditions need to be
rewritten as loops which iterate over all variables but break once an
encrypted length is reached (see the transpiler documentation for
details).

* The GT has practical limitations as to the size of an encrypted
  function. As all loops are unrolled, the use of double loops can
  lead to large encrypted circuit sizes -- too large can crash the
  compiler.  Thus, instead of encrypting double loops, we encrypt the
  inner loop as a function, and run the outer loop in software, in a
  data oblivious manner that keeps the result from revealing any info
  about the encrypted data.

* Certain variable names cannot be used as they convert to reserved
  words in the underlying HDL that gets generated. For example, `flag`
  should not be used as a variable name.

### AES Versions and Various Transpiled Examples

There are currently three different flavors of AES supported: ECB, CBC
and CTR. All three can run on 128-, 192- and 256-bit message sizes. ECB
and CBC both have separate encryption and decryption example
programs. CTR uses one function for both encrypt and decrypt.

#### Transpiled functions

The following files contain single AES standard algorithmic
sub-functions that get transpiled into encrypted circuits:

```
AES_init_ctx.[h|cc] - Initialization function (key expansion) used in every AES mode
AddRoundKey.[h|cc] - used in every AES version
ShiftRows.[h|cc] - used in the encrypt operation of ECB, CBC and CTR encrypt/decrypt
SubBytes.[h|cc] - used in the encrypt operation of ECB, CBC and CTR encrypt/decrypt
MixColumns.[h|cc] - used in the encrypt operation of ECB, CBC and CTR encrypt/decrypt
InvShiftRows.[h|cc] - Used in decryption operation of ECB and CBC
InvSubBytes.[h|cc] - Used in decryption operation of ECB and CBC
InvMixColumns.[h|cc] - Used in decryption operation of ECB and CBC
XorWithIv.[h|cc] - used in the encrypt and decrypt operation of CBC
XorWithBuff.[h|cc] -  used in CTR encrypt/decrypt
IncIvOF.[h|cc] - used in CTR encrypt/decrypt
```

The following are testbench source codes, for compiling the various testbench programs: 

The naming convention is a concatenation of these three things: 
```
AES mode = ecb|cbc|ctr
operation = enc (encrypt) | dec (decrypt) | xcr (encrypt & decrypt)
backend = cleartext (unencrypted for testing) | openfhe (encrypted processing)
```
The following source testbenches are defined:
```
ecb_enc_cleartext_testbench.cc
ecb_enc_openfhe_testbench.cc

ecb_dec_cleartext_testbench.cc
ecb_dec_openfhe_testbench.cc

cbc_enc_cleartext_testbench.cc
cbc_enc_openfhe_testbench.cc

cbc_dec_cleartext_testbench.cc
cbc_dec_openfhe_testbench.cc
 
ctr_xcr_cleartext_testbench.cc
ctr_xcr_openfhe_testbench.cc

``` 
The following output executables are created.

```
ecb_enc_cleartext_testbench
ecb_enc_yosys_interpreted_cleartext_testbench

ecb_enc_openfhe_testbench
ecb_enc_interpreted_openfhe_testbench
ecb_enc_yosys_interpreted_openfhe_testbench

ecb_dec_cleartext_testbench
ecb_dec_yosys_interpreted_cleartext_testbench

ecb_dec_openfhe_testbench
ecb_dec_interpreted_openfhe_testbench
ecb_dec_yosys_interpreted_openfhe_testbench

cbc_enc_cleartext_testbench
cbc_enc_yosys_interpreted_cleartext_testbench

cbc_enc_openfhe_testbench
cbc_enc_interpreted_openfhe_testbench
cbc_enc_yosys_interpreted_openfhe_testbench

cbc_dec_cleartext_testbench
cbc_dec_yosys_interpreted_cleartext_testbench

cbc_dec_openfhe_testbench
cbc_dec_interpreted_openfhe_testbench
cbc_dec_yosys_interpreted_openfhe_testbench

ctr_xcr_cleartext_testbench
ctr_xcr_yosys_interpreted_cleartext_testbench

ctr_xcr_openfhe_testbench
ctr_xcr_interpreted_openfhe_testbench
ctr_xcr_yosys_interpreted_openfhe_testbench	
```	

To manually compile a program, switch to the `transpiler/examples`
directory and execute the following command (replacing
`ecb_enc_cleartext_testbench` with the executable you want to compile.

`bazel build //transpiler/examples/aes:ecb_enc_cleartext_testbench`

To manually run a program (building if necessary), switch to the
`transpiler/examples` directory and execute the following command
(replacing `ecb_enc_cleartext_testbench` with the executable you want
to run. note that adding -h after the double dash will display a help
menu.

For example, the following runs the executable in TOY security mode. 

`bazel run //transpiler/examples/aes:ecb_enc_cleartext_testbench -- TOY`

Note that all intermediate and executable files can be found in: 

`/usr/src/fhe/bazel-bin/transpiler/examples/aes`

(when using the docker approach). However these files are wiped when
the docker file is closed.



`compile_cleartext.sh` when run in the `transpiler/examples` directory
as `aes\compile_cleartext.sh` will compile all cleartext executables
and move them out of the transpiler's `bazel-bin` directory into the
`aes\bin` directory where they can be run outside of the transpiler
directory tree.

`compile_openfhe_all.sh` when run in the `transpiler/examples`
directory does the same for the openfhe code. Due to the long compile
times, shorter scripts are also provided for specific cases.

Note one needs to either define the path to the openfhe libraries or
provide softlinks in the `bin` directory for the runtime libraries:

```
libOPENFHEbinfhe.so
libOPENFHEbinfhe.so.1
libOPENFHEcore.so
libOPENFHEcore.so.1
libOPENFHEpke.so
libOPENFHEpke.so.1

```

## Viewing the transpiled circuits

*Note* this requires installing `yosys` on your system:

`sudo apt install yosys`

The transpiler outputs a .dot file in the bazel-bin directory
alongside the generated netlist this is the output of yosys's show
command. To view them run 

`dot -Tsvg bazel-bin/path/to/output.dot >
output.svg`

For an arbtrary netlist, you can load it into `yosys` and visualize it
with the system viewer as follows:

```
$ yosys
> read_verilog my_netlist.v
> show -format dot -prefix output -viewer <your_system_viewer>
```

the system viewerer `xdot` can work here.

The Yosys manual, Appendix C.126 has more details on `show`

