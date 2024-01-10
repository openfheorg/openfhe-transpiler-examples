#!/bin/bash

mkdir -p aes/bin

#there is no interpreted_cleartext
for target in cleartext yosys_interpreted_cleartext
do
	for mode in ecb cbc
	do			
		for func in enc dec
		do
			echo ===================================
			echo ${mode}_${func}_${target}_testbench
			bazel build //transpiler/examples/aes:${mode}_${func}_${target}_testbench
			cp -f ../../bazel-bin/transpiler/examples/aes/${mode}_${func}_${target}_testbench aes/bin
		done
	done
	for mode in ctr
	do			
		for func in xcr
		do
			echo ===================================
			echo ${mode}_${func}_${target}_testbench
			bazel build //transpiler/examples/aes:${mode}_${func}_${target}_testbench
			cp -f ../../bazel-bin/transpiler/examples/aes/${mode}_${func}_${target}_testbench aes/bin
		done
	done
done
