#!/bin/bash

for target in yosys_interpreted_cleartext
do
	echo ===================================
	echo private_set_${target}_testbench
	bazel build //transpiler/examples/private_set:private_set_${target}_testbench
	cp ../../bazel-bin/transpiler/examples/private_set/private_set_${target}_testbench private_set/bin
done
