/*
 *  stream.c
 *  
 *
 *
 */

#include "stream.h"


void insert(STREAM* stream, int id);
int comp( const void *c1, const void *c2 );


void GetCurTrans(char* stream, STREAM* trans, int offset)
{
	// we do not register the first offset items that obtained by splitting the transactin
	// this is used for coping with the output error by IBM generator
	char delimiter[] = " ";
	char* tk;
	int cnt = 0;
	int item;
	trans->num = 0;
	tk = strtok(stream, delimiter);
	while(tk != NULL && cnt < SUBSEQ_LEN){
		//fprintf(stderr, "tk: %s\n", tk);
		if(offset > 0){
			offset--;
			tk = strtok(NULL, delimiter);
		}
		else{
			if( (item = atoi(tk)) != 0){
				trans->itemset[cnt] = item;
				cnt++;
			}
			tk = strtok(NULL, delimiter);
		}
	}
	qsort(trans->itemset, cnt, sizeof(int), comp);
	trans->num = cnt;
}

/* Comparison function */
int comp( const void *c1, const void *c2 )
{
	int tmp1 = *(int *)c1;
	int tmp2 = *(int *)c2;
	
	if( tmp1 < tmp2 )  return -1;
	else if( tmp1 == tmp2 ) return  0;
	else return  1;
}


void insert(STREAM* stream, int id)
{
	int i,j;
	if(stream->num == 0){
		stream->itemset[0] = id;
	}
	else{		
		for(i = 0; i < stream->num; i++){
			if(stream->itemset[i] > id){
				break;
			}
		}
		for(j = stream->num; j > i; j--){
			stream->itemset[j] = stream->itemset[j-1];
		}
		stream->itemset[i] = id; // insertion
	}
	stream->num++;
}

/*
 FUNCTION: ShowStream
 + outputting the current stream (for debagging)
 OPTIONS:
 + stream - the STREAME data
 RETURNS:
 + void
 */ 
void ShowStream(STREAM stream)
{
	int i;
	fprintf(stderr, "itemset %d\n",stream.num);
	for(i = 0; i < stream.num; i++)
		fprintf(stderr, "%d ",stream.itemset[i]);
	fprintf(stderr, "\n");  
}

void PrintZakiStream(STREAM stream, PARAMETER* para, FILE* zakiFile){
    
    fprintf(zakiFile, "%d %d %d ", para->time, para->time, stream.num);
    for(int i = 0; i < stream.num; i++)
        fprintf(zakiFile, "%d ",stream.itemset[i]);
    fprintf(zakiFile, "\n");
}

/**
 * FUNCTION: FreeStream
 *	+ opening the access map
 * OPTIONS:
 *	+ stream - pointer to STREAM
 *	+ stream_len - stream length
 *	+ phase - flag to display this progress
 * RETURN:
 *	+ void
 **/
void FreeStream(STREAM *stream, int stream_len, int phase)
{
	int i;
	
	if(phase == 1)
		fprintf(stderr,"FreeStream\t");
	
	for(i = 0; i < stream_len; i++){
		free(stream[i].itemset);
		stream[i].itemset = NULL;
	}
	
	free(stream);
	stream = NULL;
	
	if(phase == 1)
		fprintf(stderr,"OK!\n");
	
}

