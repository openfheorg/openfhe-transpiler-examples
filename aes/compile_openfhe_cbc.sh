#!/bin/bash
$echo "This file must be run in the transpiler/examples directory"

mkdir -p aes/bin

for target in openfhe interpreted_openfhe yosys_interpreted_openfhe
do
	for mode in cbc
	do			
		for func in enc dec
		do
			echo ===================================
			echo ${mode}_${func}_${target}_testbench
			bazel build //transpiler/examples/aes:${mode}_${func}_${target}_testbench
			cp -f ../../bazel-bin/transpiler/examples/aes/${mode}_${func}_${target}_testbench aes/bin
		done
	done
done
