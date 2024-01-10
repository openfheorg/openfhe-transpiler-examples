#!/bin/bash
echo "This file must be run in the bin directory"

#set the size to the same setting as in aes.h
size=128
#size=192
#size=256
for target in openfhe interpreted_openfhe yosys_interpreted_openfhe
do
	for security in TOY MEDIUM STD128Q_OPT

	do
		for mode in ecb cbc
		do			
			for func in enc dec
			do
				echo ===================================
				echo ${mode}_${func}_${target}_testbench 
				./${mode}_${func}_${target}_testbench ${security} | tee ${size}_${mode}_${func}_${target}_${security}.txt

			done
		done
		for mode in ctr
		do			
			for func in xcr
			do
				echo ===================================
				echo ${mode}_${func}_${target}_testbench 
				./${mode}_${func}_${target}_testbench ${security} | tee ${size}_${mode}_${func}_${target}_${security}.txt
			done
		done
	done
done
