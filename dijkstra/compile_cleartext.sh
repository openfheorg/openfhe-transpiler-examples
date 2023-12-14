#!/bin/bash

for target in yosys_interpreted_cleartext
do
	echo ===================================
	echo dijkstra_${target}_testbench
	bazel build //transpiler/examples/dijkstra:dijkstra_${target}_testbench
	cp ../../bazel-bin/transpiler/examples/dijkstra/dijkstra_${target}_testbench dijkstra/bin
done
