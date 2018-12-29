# Which compiler
CC=gcc
CPPFLAGS=
# Compiler Options for development
CFLAGS=-Wall -g -pg
LDFLAGS=-Wl,--stack,25600
# stack size 256MB
all:				main

main:		main.c mineSubset.o helpMineSubset.o getdelim.o getline.o
						$(CC) $(CFLAGS) -o main main.c mineSubset.o helpMineSubset.o getdelim.o getline.o
						
mineSubset.o:		mineSubset.c
						$(CC) $(CFLAGS) -c mineSubset.c	

helpMineSubset.o:	helpMineSubset.c
						$(CC) $(CFLAGS) -c helpMineSubset.c

getdelim.o:	getdelim.c
						$(CC) $(CFLAGS) -c getdelim.c

getline.o:	getline.c
						$(CC) $(CFLAGS) -c getline.c
						
clean:
						rm -f main *.o
