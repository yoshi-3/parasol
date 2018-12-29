# Which compiler
CC=gcc
CPPFLAGS=
# Compiler Options for development
CFLAGS=-Wall
LDFLAGS=-Wl,--stack,1024000
# stack size 1024MB
all:				main

main:		main.c rTypeMining.o param.o help.o stream.o streamRedFunc.o tableFunc.o treeFunc.o getline.o getdelim.o
						$(CC) $(CFLAGS) -o main main.c rTypeMining.o param.o help.o stream.o streamRedFunc.o tableFunc.o treeFunc.o getline.o getdelim.o

rTypeMining.o:	rTypeMining.c
						$(CC) $(CFLAGS) -c rTypeMining.c
						
getline.o:		getline.c
						$(CC) $(CFLAGS) -c getline.c	

getdelim.o:		getdelim.c
						$(CC) $(CFLAGS) -c getdelim.c	

param.o:	param.c
						$(CC) $(CFLAGS) -c param.c

help.o:	help.c
						$(CC) $(CFLAGS) -c help.c

stream.o: stream.c
						$(CC) $(CFLAGS) -c stream.c

streamRedFunc.o:	streamRedFunc.c
						$(CC) $(CFLAGS) -c streamRedFunc.c

tableFunc.o:	tableFunc.c
						$(CC) $(CFLAGS) -c tableFunc.c

treeFunc.o:	treeFunc.c
						$(CC) $(CFLAGS) -c treeFunc.c

clean:
						rm -f main *.o
						rm -f result.txt
						rm -f default.*
