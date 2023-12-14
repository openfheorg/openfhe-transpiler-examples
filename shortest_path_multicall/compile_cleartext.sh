#!/bin/bash
mkdir -p shortest_path/bin
for target in cleartext yosys_interpreted_cleartext 
do
	echo ===================================
	echo shortest_path_${target}_testbench
	bazel build //transpiler/examples/shortest_path:shortest_path_${target}_testbench
	cp -f ../../bazel-bin/transpiler/examples/shortest_path/shortest_path_${target}_testbench shortest_path/bin
done
