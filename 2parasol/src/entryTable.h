/*
 *  entryTable.h
 */

#ifndef ENTRYTABLE_H
#define ENTRYTABLE_H

#include"help.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h> //to use the function: getopt
#include <time.h> //to measure the executing time
#include <string.h>

#include <malloc/malloc.h> //for compiling the program in mac OSX
//#include <malloc.h> //for compiling the program in Linux OS


//#define ITEM_KIND 10000000 //maximal number of item kinds
#define SUBSEQ_LEN 10000    //maximal number of items in each entry
#define BUF_SIZE 20000     //buffer size storing input stream
#define TABLE_SIZE 2000000 //maximal size of entry table
//#define TABLE_SIZE 50000000
#define HASH_SIZE 7500000 //hash size
//#define HASH_SIZE 10000000

#define FILE_LEN 1000 //maximal length of file names
#define ALLOCATE_SIZE 64


/*Initial values of the parameters*/
#define DEF_SUP 0.0 //minimal support
#define DEF_ERR 0.0 //maximal error parameter
#define DEF_STRLEN 10000 //default stream size
#define DEF_INFILE "default.seq" //default name of input file
#define DEF_OUTFILE "default" //default name of output file
//#define DEF_RESFILE "result" //default name of result file
#define DEF_ANS "Answer.ans"
#define DEF_REP "Replay.ans"

// return the parent index in heap (table)
#define parent(n) ((n)/2)
// return the child index in heap (table)
#define child(n)  ((n)*2)


typedef struct data{
	int num; // number of transactions
	FILE* input; // input file
}DATA;


typedef struct stream{
	int num; //number of items 
	int *itemset; //item set
}STREAM;


//Element in the hash map (with chaining way) for accessing entries
typedef struct list{
	int pos; //Index of the corresponding entry in the table
	struct list *next; //next list element
	struct list *prev; //previous list element
}LIST;


//Node
//constructing a tree of entries that captures the inclusion relation between two entrie
typedef struct node{
	int pos;         //index of this entry in table 0: if it is root, more than 0: otherwise
	int height;		 //the height of this node
	int childNum;	 //the number of its child nodes
	int hasRsibling; //1: if it has its right sibling, 0: otherwise
	int hasLsibling; //1: if it has its left sibling, 0: otherwise
	int hasChild;    //1: if it has child, 0: otherwise
	struct node *parent;	//its parent node
	struct node *rchild;	//its rightmost child node
	struct node *lchild;	//its leftmost child node
	struct node* rsibling;	//its right sibling node
	struct node* lsibling;  //its left sibling node
}NODE;


//Entry
typedef struct entry{
	int subset[SUBSEQ_LEN]; //subset
    int tmp;        //temporary used flag for inner processing
	int e_count;	//estimated count
	int t_count;	//true count 
	int length;		//the length of this subset
	NODE* n_link;	//link to the corresponding node
	LIST* link;		//link to the corresponding element in the access map
}ENTRY;


//Table of entries
typedef struct table{
	ENTRY** heap; //the heap array of entries 
	int delta;	//the maximal error count
	int last; //the index of last entry in the table	
}TABLE;


//Element to be managed in the stack table
typedef struct element{
	ENTRY* entry;
	LIST* list;
	NODE* node;
}ELEMENT;


//Tree of nodes (as well as the stack table)
typedef struct tree{
	NODE* root;
	ELEMENT** stack;
	int cur_index;
}TREE;


typedef struct parameter{
	float support;			    //minimal support
	float error;			    //error parameter
    int table_size;             //size constant
    int lc;                     //flag to exploit the lossy counting
    int ss;                     //flag to exploit the space saving
    int comp;                   //flag to exploit Delta-compression
    int reduce;                 //flag to exploit the stream reduction
    int disp_output;            //flag to write the output file
    int time;                   //current time (the number of transactions that have been processed)
    clock_t time_start;         //start CPU time for executing the process
    clock_t time_end;           //end CPU time for executing the process
    char in_filename[FILE_LEN];  //input file name (that is, STREAM.seq)
    char out_filename[FILE_LEN]; //output file name (that is, MINING_RESULT.ans)
    char out_table[FILE_LEN];    //output file name (that is, MINING_RESULT.table)
    char out_map[FILE_LEN];      //output file name (that is, MINING_RESULT.map)
    int num_outputs;        //the number of outputs
}PARAMETER;


#endif




