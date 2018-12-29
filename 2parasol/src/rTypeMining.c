/*
 *  rTypeMining.c
 *  
 */

#include "rTypeMining.h"

/**
 * FUNCTION: MiningStream
 * + function for mining streams
 * OPTIONS:
 * + tree - pointer to the TREE value
 * + table - pointer to the TABLE value
 * + map - pointer to the access map
 * + stream - pointer to the STREAM value
 * + para - pointer to the PARAMETER value
 * + s_para - pointer to the SPARAMETER value
 * RETURN:
 * + void
 **/ 
void MiningStream(TREE *tree, TABLE *table, LIST** map, FILE* stream, PARAMETER *para, I_TABLE *i_table, MAP** i_map, I_PARAMETER *i_para)
{
		
	STREAM* curStream = (STREAM*)malloc(sizeof(STREAM) * 1);	//current transaction
	curStream->itemset = (int*)malloc(sizeof(int)*SUBSEQ_LEN); //Allocating the itemset
    
    char *tmp = NULL;
	size_t len = 0;
	ssize_t read;
	
	para->time_start = clock(); //obtaining the current time by clock function
	
	while((read = getline(&tmp, &len, stream)) != -1)
	{
        
        GetCurTrans(tmp, curStream, 0);
		
        if(curStream->num == 0){
			// skip the following process
			continue;
		}
		para->time++;
        //ShowStream(*curStream);
		
		//outputting the progressed transaction number
		fprintf(stderr, "processed trans. num: \t %d \r", para->time);
    
		//updating the item table (to obtain the total number of appeared item kinds)
		
		//reducing this stream
		if(para->reduce){
			i_para->item_cnt += curStream->num; // updating the total item appearances
			I_UpdateTable(*curStream, i_table, i_map, i_para);
			ReduceStream(curStream, i_table, i_map, i_para->hash_size, table->delta);
			
			// updating the delta
			if(i_table->delta > table->delta){
				table->delta = i_table->delta;
			}
            if(curStream->num == 0){
                // skip the following process
                continue;
            }
		}
		
		//updating the maximal and average stream lengthes
		UpdateTable(tree, *curStream, table, map, para); //updating the entry table
	}
	if( para->comp == 1){
        Compress(tree, table, map, 0); // pre-processing
        Compress(tree, table, map, 1); // brute-force pairwise checking
	}
    
	fprintf(stderr, "\n");
    fprintf(stderr, "final table size: %d\n", table->last);
	para->time_end=clock();
	fclose(stream);
	// free the allocated memory
	free(curStream->itemset);
	free(curStream);
}

