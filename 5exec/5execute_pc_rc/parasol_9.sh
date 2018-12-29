#!/bin/sh
#$ -N parasol_9mushroom
#$ -q cpu.q
#$ -o ./result/stdout/
#$ -e ./result/stderr/
#$ -cwd

cd ../2parasol/src

P="../../benchmark/"
N="9mushroom"
F="$P$N.txt"

K=100000
E=0.1

./main -i "$F" -k "$K" -s 1.0 -e "$E" -m