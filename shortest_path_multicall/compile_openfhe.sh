#!/bin/bash
mkdir -p shortest_path/bin
for target in openfhe interpreted_openfhe yosys_interpreted_openfhe
do
	echo ===================================
	echo shortest_path_${target}_testbench
	bazel build //transpiler/examples/shortest_path:shortest_path_${target}_testbench
	cp -f ../../bazel-bin/transpiler/examples/shortest_path/shortest_path_${target}_testbench shortest_path/bin
done
