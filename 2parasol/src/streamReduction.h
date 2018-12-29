
#ifndef STREAMREDUCTION_H
#define STREAMREDUCTION_H

/**
 * headerItem.h 
 * header filers
 * mineItem.c
 */

#include"entryTable.h"


//Element in the hash map
typedef struct map{
	int pos; //Index of the corresponding entry in the table
	struct map *next; //next list element
}MAP;


//Entry
typedef struct i_entry{
	int item;	//item
	int e_count;	//estimated count
	int t_count;	//true count
 	int anchor;     //anchor index indicating ites previous index in the entry table
}I_ENTRY;

//Table of enties
typedef struct i_table{
	I_ENTRY** heap; //the heap array of entries
	int delta; //the maximal error count
	int last; //the index of last entry in the table

}I_TABLE;


/* Parameters for item mining */
typedef struct i_parameter{
	
	//float support;			//minimal support
	//float error;			//maximal error ratio	
	//int itemtype;			//number of item kinds  
	//int stream_len;			//stream length
	
	//char in_filename[FILE_LEN];  //input file name (that is, STREAM.seq)
	//char out_filename[FILE_LEN]; //output file name (that is, MINING_RESULT.ans)
	//char out_table[FILE_LEN];
	//char out_map[FILE_LEN];
	
	int table_cap;			//flag indicating if the entry table is full
	int table_size;			//maximal entry size to be registered  
	int table_max;			//maximal size of entries that have been registerd
	int hash_size;			//hash size
	
	int time;			//current time (the number of transactions that have been processed)

	clock_t time_start;		//start CPU time for executing the process
	clock_t time_end;		//end CPU time for executing the process

	//newly added parameters for item mining
	int T_max;	//maximal transaction length
	int item_cnt;	
	
}I_PARAMETER;

void PrintHelp();

#endif
