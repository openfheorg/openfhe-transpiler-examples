#!/bin/bash
mkdir -p private_set/bin
for target in cleartext yosys_interpreted_cleartext 
do
	echo ===================================
	echo private_set_${target}_testbench
	bazel build //transpiler/examples/private_set:private_set_${target}_testbench
	cp -f ../../bazel-bin/transpiler/examples/private_set/private_set_${target}_testbench private_set/bin
done
