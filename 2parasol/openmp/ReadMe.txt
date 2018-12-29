

ReadMe

miner.cpp: this is the openmp sourcefile for the baseline algorithm.

Using an intel compliler (we run it using Intel Parallel Studio XE 2017), 
please exploit the following command:
% icpc -o miner -xCORE-AVX2 -qopenmp -qopenmp-link static -std=c++0x miner.cpp

Then, it works with the following command:
% ./miner k < filename
where k is the maximum size of used entries and filename is the input file path

Example 
./miner 1000 < ../sample/input/t0.txt

If we set the thread number as 4, please use the following command:
% OMP_NUM_THREADS=4, ./minere 1000 < ../sample/input/t0.txt 

This source is used to create the result in Figure 12(c)