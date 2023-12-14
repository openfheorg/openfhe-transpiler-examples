#!/bin/bash

for target in yosys_interpreted_openfhe interpreted_openfhe # note openfhe segfaults the compiler.
do
	echo ===================================
	echo private_set_${target}_testbench
	bazel build //transpiler/examples/private_set:private_set_${target}_testbench
	cp -f ../../bazel-bin/transpiler/examples/private_set/private_set_${target}_testbench private_set/bin
done
