#!/bin/bash
mkdir -p dijkstra/bin
for target in cleartext yosys_interpreted_cleartext
do
	echo ===================================
	echo dijkstra_${target}_testbench
	bazel build //transpiler/examples/dijkstra:dijkstra_${target}_testbench
	cp -f ../../bazel-bin/transpiler/examples/dijkstra/dijkstra_${target}_testbench dijkstra/bin
done
