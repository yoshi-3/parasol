

#ifndef HEADERSUBSET_H
#define HEADERSUBSET_H


/**
 * headerSubset.h 
 * header filers
 * mineSubset.c
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h> //to use the function: getopt
#include <time.h> //to measure the executing time
#include <malloc/malloc.h> //for compiling the program in mac OSX
//#include <malloc.h> //for compiling the program in Linux OS

#define SUBSEQ_LEN 120 //maximal number of items in each entry
//#define TABLE_SIZE 1000
#define TABLE_SIZE 1000000 //maximal size of entry table 
//#define TABLE_SIZE 50000000
#define HASH_SIZE 250000 //hash size
//#define HASH_SIZE 1500000
#define FILE_LEN 500 //maximal length of file names
#define ALLOCATE_SIZE 64


/*Initial values of the parameters*/
#define DEF_SUP 0.2 //minimal support
#define DEF_ERR 0.1 //maximal error parameter
#define DEF_WIN 1 //window size
#define DEF_STRLEN 10000 //transactional stream length 
#define DEF_INFILE "default.seq" //default name of input file
#define DEF_LCMFILE "lcm.dat"    //default name of LCM output file
#define DEF_OUTFILE "default" //default name of output file
#define DEF_ANS "Answer.ans"
#define DEF_REP "Replay.ans"

// return the parent index in heap (table)
#define parent(n) ((n)/2)
// return the child index in heap (table)
#define child(n)  ((n)*2)


//Element in the hash map (with chaining way) for accessing entries
typedef struct list{
	int pos; //Index of the corresponding entry in the table
	struct list *next; //next list element
	struct list *prev; //previous list element
}LIST;

//Entry
typedef struct entry{
	int subset[SUBSEQ_LEN]; //subset
	int e_count;	//estimated count
	int t_count;	//true count 
	int length;		//the length of this subset
	LIST* link;		//link to the corresponding element in the access map
	int min_flag;	//flag indicating if this entry has the minimal estimated count
}ENTRY;

//Table of entries
typedef struct table{
	ENTRY** heap; //the heap array of entries 
	int delta; //the maximal error count
	int last; //the index of last entry in the table
	int t_skip_num;	// the number of t_skip operation
	
	/* parameters for SKIP operations */
	int registNum;		//the number of candidate sets that have been registered
	int min;			//the estimated count of the minimal entry at this moment
	int n_min;			//the number of minimal entries at this moment	
	
}TABLE;

typedef struct stream{
	int num; //number of items 
	int *itemset; //item set
}STREAM;


typedef struct pair{
	int left;
	int right;
}PAIR;


typedef struct parameter{
	float support;			//minimal support
	float error;			//maximal error ratio
	
	int itemtype;			//number of item kinds  
	int stream_len;			//stream length
	
	char in_filename[FILE_LEN];  //input file name (that is, STREAM.seq)
	char out_filename[FILE_LEN]; //output file name (that is, MINING_RESULT.ans)
	char out_table[FILE_LEN];
	char out_map[FILE_LEN];
	
	int table_cap;			//flag indicating if the entry table is full
	int table_size;			//maximal entry size to be registered  
	int table_max;			//maximal size of entries that have been registerd
	int hash_size;			//hash size
	int collision;			//the number of collisions
	int lcm_check;			//flag to compute the recall and precision
	int match_freq_items;	//the frequent outputs in table
	int num_freq_items;		//the number of frequent itemsets
	int num_outputs;		//the number of outputs
	int max_stream_len;		//maximal length of each stream
	int total_stream_len;	//total length of every stream
	int skip_num;			//the number of skip operations
	int burst_num;			//the number of bursty transactions
	double update_time;		//the executing time for updating the current table
	double replace_time;	//the executing time for replacing the entries in the table
	
	int t_skip;				//flag for applying the t-skip operator
	int r_skip;				//flag for applying the r-skip operator
	int reduce;				//flag for reduing the transactions in advance
	
	int time;				//current time (the number of transactions that have been processed)
	clock_t time_start;		//start CPU time for executing the process
	clock_t time_end;		//end CPU time for executing the process
	
}PARAMETER;

void PrintHelp();

#endif

