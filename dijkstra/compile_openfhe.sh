#!/bin/bash

for target in openfhe yosys_interpreted_openfhe
do
	echo ===================================
	echo dijkstra_${target}_testbench
	bazel build //transpiler/examples/dijkstra:dijkstra_${target}_testbench
	cp ../../bazel-bin/transpiler/examples/dijkstra/dijkstra_${target}_testbench dijkstra/bin
done
