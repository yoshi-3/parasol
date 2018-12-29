#!/bin/bash

MEMORY=512m
java -Xmx$MEMORY -cp "../lib/*" -javaagent:../lib/sizeofag-1.0.0.jar moa.DoTask "LearnModel -m 100000 -l (IncMine -w 20 -m 5 -s 0.05 -r 0.4 -l 5000) -s (ZakiFileStream -f ../examples/test2.txt) -O test2.out"