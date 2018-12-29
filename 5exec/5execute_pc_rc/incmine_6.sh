#!/bin/sh
#$ -N incmine_6connect
#$ -q cpu.q
#$ -o ./result/stdout/
#$ -e ./result/stderr/
#$ -cwd

cd ../1moa-incmine/bin

P="../../benchmark/zaki/"

F="6connect.zaki"
M=67557
W=68

java -Xmx1000M -cp "../lib/*" -javaagent:../lib/sizeofag-1.0.0.jar moa.DoTask "LearnModel -l (IncMine -w "$W" -s 1.0 -r 0.1 -l 1000) -s (ZakiFileStream -f "$P$F")" -m "$M"
