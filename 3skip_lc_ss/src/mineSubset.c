/*
 mineSubset.c
 
 Input: streams data (*.seq)
 Output: frequent itemsets
 
 Embedding the SKIP operation into the baseline implementation
 
 For the detail on the usage, please refer users to the option -h
  
 */

#ifndef HEADERSUBSET_C
#define HEADERSUBSET_C


#include"mineSubset.h" // Definition of data structures and constant symbols


/**
 * FUNCTION: MiningStream
 * + function for mining streams
 * OPTIONS:
 * + table - pointer to the TABLE value
 * + map - pointer to the access map
 * + stream - pointer to the STREAM value
 * + para - pointer to the PARAMETER value
 * + s_para - pointer to the SPARAMETER value
 * RETURN:
 * + void
 **/ 
void MiningStream(TABLE* table, LIST** map, FILE* stream, PARAMETER* para)
{
	
	float progress; //progress ratio
	FILE *fcsv;
	char outCSV[FILE_LEN];
	clock_t start, end;
	int replace_num_baseline;
	int replace_num_sss = para->table_size;
	
	int* itemKind = (int*)calloc(1000000, sizeof(int));      //item kinds 
	STREAM* curStream = (STREAM*)malloc(sizeof(STREAM) * 1);	//current transaction
	curStream->itemset = (int*)malloc(sizeof(int)*3000); //Allocating the itemset
	
	char *tmp = NULL;
	char k[64];
	
	size_t len = 0;
	ssize_t read;
	
	strcpy(outCSV,para->out_filename);
	sprintf(k, "%d", para->table_size);
	strcat(outCSV, k);
	strcat(outCSV,".csv");
	
	fcsv = fopen(outCSV,"w"); //opening the output file (csv)
	
	para->time_start = clock(); //obtaining the current time by clock function
	fprintf(fcsv,"#t,support,str_len,table_size,exec_time,update_time,replace_time,delta,min,n_m,r,t_skip,skip,burst_num,replace_num (baseline),replace_num (SSS)\n");
	
	int time = 0; //current time (from Time step 1)

	//repeating the following by finishing to read the input streams
	while((read = getline(&tmp, &len, stream)) != -1)
	{
		time++;
		para->stream_len++;
	
		GetCurTrans(tmp, curStream, 0, itemKind, para);
	
		if(curStream->num == 0){
			continue;
		}
		
		//ShowStream(curStream, para);
		
		//progress = (time*100) / para->stream_len; //updating the progress ratio
	
		//outputting the progress ratio with the recovery code r
		fprintf(stderr,"Mining Stream\t %d \t %d\t %d\r", (int)progress, time, para->stream_len);
		
		//STREAM curStream = stream[time-1];
		
		if(para->max_stream_len < curStream->num){
			para->max_stream_len = curStream->num;
		}
		para->total_stream_len += curStream->num;
		
		//updating the maximal and average stream lengthes

		
		start = clock();
		// t-skip
		if(para->t_skip){
			// applying the t-skip operation
			if( table->last == 0 || (log2(para->table_size) >= curStream->num - 1) ){
				UpdateTable(*curStream, table, map, para); //updating the entry table
			}
			else{
				// t-skip operation
				table->t_skip_num++;
				table->min = table->delta + 1;
				table->delta++;
				if(para->table_size == table->last){
					table->n_min = updateRSkip(table, 1); //the minimal entry will increase 
				}
				
				// incrementing the est_count of every entry in the table
				increment(table); 
				
			}
		}
		else {
			UpdateTable(*curStream, table, map, para); //updating the entry table
		}		
		end = clock();
		
		replace_num_baseline = (int)pow(2, curStream->num) - 1 - table->registNum;
		if( replace_num_sss > replace_num_baseline ){
			replace_num_sss = replace_num_baseline;
		}
		
		// outputing the current parameter values in csv file
		fprintf(fcsv,"%d,%.2f,%d,%d,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
				time, 
				time*para->support, 
				curStream->num,
				table->last,
				(double)1000*(end-start)/(double)CLOCKS_PER_SEC,
				para->update_time,
				para->replace_time,
				table->delta,
				table->min,
				table->n_min,
				table->registNum,
				table->t_skip_num,
				para->skip_num,
				para->burst_num,
				replace_num_baseline,
				replace_num_sss);
		
		// resetting registed ones
		table->registNum = 0;
		
		replace_num_sss = table->n_min;
	}
	
	para->time = time;
	para->time_end=clock();
	
	fclose(fcsv);
	
	fprintf(stderr,"MiningStream\tOK!    \t");
	fprintf(stderr,"(CPU: %.1fsec)    \n",
			(double)(para->time_end - para->time_start)/(double)CLOCKS_PER_SEC);
	
}


void GetCurTrans(char* stream, STREAM* trans, int offset, int* itemKind, PARAMETER* para)
{
	// we do not register the first offset items that obtained by splitting the transactin
	// this is used for coping with the output error by IBM generator
	char delimiter[] = " ";
	char* tk;
	int cnt = 0;
	int item;
	trans->num = 0;
	tk = strtok(stream, delimiter);
	while(tk != NULL && cnt < 3000){
		//fprintf(stderr, "tk: %s(J\(Bn", tk);
		if(offset > 0){
			offset--;
			tk = strtok(NULL, delimiter);
		}
		else{
			if( (item = atoi(tk)) != 0){
				if( item < 1000000 && itemKind[item] == 0){
					itemKind[item] = 1;
					para->itemtype++;
				}
				trans->itemset[cnt] = item;
				cnt++;
			}
			tk = strtok(NULL, delimiter);
		}
	}
	qsort(trans->itemset, cnt, sizeof(int), comp);
	trans->num = cnt;
	//if(cnt > 0){
	//	trans->num = cnt-1;
	//}
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

/**
 * FUNCTION: UpdateTable
 * + Extracting each subset from the current transaction and register it to the entry table
 * OPTIONS:
 * + stream - the stream data
 * + table - pointer to the entry table
 * + map - pointer to the access map
 * + para - pointer to the PARAMETER value
 * RETURN:
 * + void
 **/
void UpdateTable(STREAM curStream, TABLE *table, LIST** map, PARAMETER *para){
	
	int i;
	int num_replace_set = 0;	// number of the candidate sets to be replaced
	int update_case = 0;		// flag indicating in which case (0, 1, 23) we update the delta value
	
	clock_t start_update, end_update, start_replace, end_replace;
	
	// step 1. checking if the registered entry has been included in this transaction
	
	if(table->last == 0 || log2(table->last) < curStream.num){
		//printf("---scan_table---\n");
		
		if(log2(para->table_size) <= curStream.num - 1)
			para->burst_num++; //this transaction is burst
		
		int order[curStream.num];
		int reorder[curStream.num];
		
		// initializing the order array with zero
		for(i = 0; i < curStream.num; i++){
			order[i] = 0;
		}
		
		// step 1-a. updating each entry's count by scanning the table
		start_update = clock();
		UpdateByTableScan(curStream, table, map, para, order);
		end_update = clock(); 
				
		// step 2. skipping operations and setting the case flag for updating the delta value
		num_replace_set = table->n_min;
		float diff = log2(table->n_min + table->registNum + 1) - curStream.num;
		if( diff > 0 ){
			num_replace_set = (int)pow(2, curStream.num) - table->registNum - 1;
			update_case = 0;
		}
		else if( diff < 0 ){
			para->skip_num++; //increment the number of skip operations
			update_case = 1;
		}
		else{
			update_case = 2;
		}

		// step 3. replacing minimal entries with the candidate sets
		// continue to replace the ``num_replaced_set'' minimal entries from the order array
		start_replace = clock();
		int zeroNum = reordering(order, reorder, curStream.num); //the number of items whose frequences were zero
		if(para->r_skip){
			if(update_case != 1 || para->table_size > table->last){
				ReplaceByTableScan(curStream, table, map, para, reorder, zeroNum, num_replace_set);
			}
		}
		else{
			ReplaceByTableScan(curStream, table, map, para, reorder, zeroNum, num_replace_set);
		}
		end_replace = clock();
		
	}
	else{
		
		int order[(int)pow(2, curStream.num) - 1];
		
		// step 1-b. updating each entry's count by enumerating all the subsets from the stream
		start_update = clock();
		UpdateByTransEnum(curStream, table, map, para, order);
		end_update = clock();
		
		// step 2. skipping operations and setting the case flag for updating the delta value
		num_replace_set = table->n_min;
		float diff = log2(table->n_min + table->registNum + 1) - curStream.num;
		if( diff > 0 ){
			num_replace_set = (int)pow(2, curStream.num) - table->registNum - 1;
			update_case = 0;
		}
		else if( diff < 0 ){
			para->skip_num++; //increment the number of skip operations
			update_case = 1;
		}
		else{
			update_case = 2;
		}
		
		// step 3. replacing minimal entries with the candidate sets
		// continue to replace the ``num_replaced_set'' minimal entries from array
		start_replace = clock();
		if(para->r_skip){
			if(update_case != 1 || para->table_size > table->last){
				ReplaceByTransEnum(curStream, table, map, para, order, num_replace_set);
			}
		}
		else{
			ReplaceByTransEnum(curStream, table, map, para, order, num_replace_set);
		}
		end_replace = clock();
	}
	
	//step 4. updating the next min and delta from current min and delta
	if( update_case == 0 ){
		//printf("delta updating: case 0:\n");
		table->delta = table->min;
	}
	else if( update_case == 1 ){
		//printf("delta updating: case 1:\n");
		table->min = table->delta + 1;
		table->delta++;
		if(para->table_size == table->last){
			table->n_min = updateRSkip(table, 1); // the minimal entry will increase 
		}
	}
	else{
		//printf("delta updating: case 2:\n");
		int temp = table->min;
		table->min = table->delta + 1;
		table->delta = temp;		
	}
	
	//updating two kinds of the executing time for updating and replacing the table
	para->update_time = (double)1000*(end_update - start_update) / (double)CLOCKS_PER_SEC;
	para->replace_time = (double)1000*(end_replace - start_replace) / (double)CLOCKS_PER_SEC;
}


/**
 * FUNCTION: UpdateByTableScan
 *	+ updating the count of each entry with the current stream by scanning the entry table
 * OPTIONS:
 *	+ stream - the STREAM data
 *	+ table	 - pointer to the entry table
 *	+ map	 - pointer to the access map
 *	+ para	 - pointer to the parameter
 *	+ order  - pointer to the array showing the order of frequencies over the table for each item in the stream
 * RETURN:
 *	+ void
 **/
void UpdateByTableScan(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* order){
	
	//printf("table->last: %d\n", table->last);
	int i, j;
	// step 1. for each registered entry, we check if it is included in the stream
	// we temporary prepare one box for the pointers to entries
	ENTRY** box = (ENTRY**)malloc(sizeof(ENTRY*) * (table->last+1));
	if(box == NULL){
		fprintf(stderr, "cannot allocate memory to the new box (error in Update By Table Scan)\n");
		exit(0);
	}
	for(i = 1; i <= table->last; i++){
		box[i] = table->heap[i];
		//EntryPrint(table->heap[i]);
	}
	
	for(i = 1; i <= table->last; i++){
		if( IsIncluded(box[i], stream, order) ){
			// incrementing the estimated and true counts of this entry
			box[i]->e_count++;
			box[i]->t_count++;
			// incrementing the number of registered candidate sets 
			table->registNum++;
			// reordering this entry in the current table
			j = Reorder(table, box[i]->link->pos, map); // j is the modified index of this entry
			// updating the min and n_m values wrt the current table
			UpdateMinEntries(table, j);
		}
	}	
	free(box);
	box = NULL;
}

/**
 * FUNCTION: UpdateByTransEnum
 *	+ updating the count of each entry with the current stream by enumerating all the subsets in it
 * OPTIONS:
 *	+ stream - the STREAM data
 *	+ table	 - pointer to the entry table
 *	+ map	 - pointer to the access map
 *	+ para	 - pointer to the parameter
 *	+ n_reg_array  - pointer to the array showing every ID of non-registered candidate set from the stream
 * RETURN:
 *	+ void
 **/
void UpdateByTransEnum(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* n_reg_array){
	
	int i;
	int nonRegistNum = 0;
	// step 1. enumerating all the subsets from the current transaction
	int canSetNum = (int)pow(2, stream.num);
		
	for(i = 1; i < canSetNum; i++){
		// step 2. checking if the current candidate set is registered or not
		
		// create the current candidate set
		int subset[stream.num];
		int subsetLen = GetSubset(subset, i, stream);
				
		int hash_key = GetHashKey(subset, subsetLen, para->hash_size);
		
		ENTRY* rCanSet = GetRegistSet(subset, subsetLen, map, table, hash_key);
		
		
		if(rCanSet != NULL){
			//step 3-1. this subset has been already registered
			
			//incrementing the true and estimated counts and this entry
			rCanSet->t_count++; 
			rCanSet->e_count++; 
			
			// incrementing the number of registered candidate sets 
			table->registNum++;
			
			// reordering this entry in the current table
			int pos = rCanSet->link->pos; // index of this entry in the table
			
			int j = Reorder(table, pos, map); // j is the modified index of this entry
			
			// updating the min and n_m values wrt the current table
			UpdateMinEntries(table, j);
			
		}
		else{
			// step 3-2. this subset has not been already registered
			n_reg_array[nonRegistNum] = i; // register this index into the n_reg_array
			nonRegistNum++;	// incrementing the num of non registeded subsets
		}
		
	}
}


/**
 * FUNCTION: Reorder
 *	+ reordering the focused entry in the table
 * OPTIONS:
 *	+ table - pointer to the TABLE
 *	+ id	- index of the focused entry
 * RETURN
 *	int	- the modified index of the focused entry
 **/
int Reorder(TABLE* table, int id, LIST** map){
	
	int parent, child;
	
	// index focusing on the initial parent
	parent = id;
	
	// index for its left child
	child = child(parent);
	
	// continue to search for the relevant position of the new entry
	while(child <= table->last){
		// change the child index if the e_count of right child < e_count of left child
		
		if(child != table->last && table->heap[child+1]->e_count < table->heap[child]->e_count){
			child++;
		}
		// swap if the e_count of child < e_count of parent
		if(table->heap[child]->e_count < table->heap[parent]->e_count){
			Swap(table, parent, child, map);
		}
		else{
			break;
		}
		// updating the parent and child indexes
		parent = child;
		child = child(parent);
	}
	return parent;
}


/** 
 * FUNCTION: UpdateMinEntries
 *	+ updating the min and n_m values wrt the current table
 *	+ updating the minimal entry flag of the focused entry
 * OPTIONS: 
 *	+ table - pointer to the table
 *  + i		- the previous index of the focused entry
 *	+ j	    - the reordered index of the focused entry
 * RETURN:
 *	+ void
 **/
void UpdateMinEntries(TABLE* table, int j){
	if( table->heap[j]->min_flag == 1 ){
		// this was minimal entry
		if( table->heap[j]->e_count != table->heap[1]->e_count ){
			// this entry is no longer minimal
			table->heap[j]->min_flag = 0;
			table->n_min--;
		}
		else{
			// this entry is still minimal but the minimal value is modified
			table->min = table->heap[1]->e_count;
			// re-counting the minimal entries 
			table->n_min = updateMinEntries(table, 1);
		}
	}
}

/**
 * FUNCTION: updateMinEntries
 *	+ counting the minimal entries wrt the current table
 * OPTIONS: 
 *	+ table - pointer to the TABLE
 *	+ j - the current id
 * RETURN:
 *	+ int
 **/
int updateMinEntries(TABLE* table, int id){
	
	// if the id is beyond the last index, return 0
	if(id > table->last){
		return 0;
	}
	// if this entry is not minimal one return 0;
	else if(table->heap[id]->e_count != table->min){
		return 0;
	}
	// return the sum of the counts both in left and right parts
	else{
		// this is minimal entry
		table->heap[id]->min_flag = 1;
		return 1 + updateMinEntries(table, child(id)) + updateMinEntries(table, child(id) + 1);
	}
	
}

int updateRSkip(TABLE* table, int id){
	// if the id is beyond the last index, return 0
	if(id > table->last){
		return 0;
	}
	// if this entry is not minimal one return 0;
	else if(table->heap[id]->e_count > table->min){
		return 0;
	}
	// return the sum of the counts both in left and right parts
	else{
		// this is minimal entry
		table->heap[id]->min_flag = 1;
		if(table->heap[id]->e_count < table->min){
			table->heap[id]->e_count = table->min;
		}
		return 1 + updateRSkip(table, child(id)) + updateRSkip(table, child(id) + 1);
	}
	
}


/** 
 * FUNCTION: ReplaceByTableScan
 *	+ replacing the minimal entries with candidate sets wrt the order array
 * OPTIONS: 
 *	+ curStream - stream
 *	+ table - pointer to the entry table
 *	+ map   - pointer to the access map
 *	+ para  - pointer to the parameter
 *  + order - order array
 *	+ num_replace_set - the num of the subsets to be replaced
 * RETURN
 *	+ void
 **/
void ReplaceByTableScan(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* order, int zeroNum, int num_replace_set){
	
	int hash_key;
	int num = 0;			// number of the candidate sets that have been replaced
	int need_contained_test = 0;    // flag indicating whether or not the generated subset should be tested
	
	// step 3-a. creating the candidate sets wrt the order array
	// reconstructing the order array so that each element indicates the rank of item wrt its frequency
	
	// checking if the n_m subsets can be created only by using the items with the zero frequency
	int index[stream.num];
	SetBit(index, stream.num, zeroNum);
	int subset[stream.num];	
		
	// continue to replace each minimal entry ``num_replaced_set'' times
	while(num < num_replace_set){
		// updating the index if it has reached the last index
		if( (stream.num != zeroNum) && EqualBitZero(index, stream.num)  ){
			UpdateBit(index, stream.num);
			//there are some candidate sets that must be created in the manner of generating and testing
			need_contained_test = 1;
		}
		// create the current candidate set
		int subsetLen = GetSubsetFromArray(subset, index, stream, order);
		
		ENTRY* rCanSet = NULL;
		
		if(need_contained_test){
			hash_key = GetHashKey(subset, subsetLen, para->hash_size);
			// checking whether or not this cansidate set is included in the table
			rCanSet = GetRegistSet(subset, subsetLen, map, table, hash_key);
		}
				
		if(rCanSet == NULL){	
			
			// this is non-registered entry
			num++; 	// updating the number of newly registered set
			
			if(table->last < para->table_size){
				// step 3-a-1. 
				// this subset can be newly registered in both access map and table
				
				// we re-compute the hash key as this process is coming without so frequently  
				hash_key = GetHashKey(subset, subsetLen, para->hash_size);
				RegistSet(subset, subsetLen, map, table, hash_key);
				
				if(table->last == para->table_size){
					// now, the current table becomes full
					para->table_cap = 1;
	
				}
				if(table->last > para->table_max){
					para->table_max = table->last;
				}
				
			}
			else{
				
				// step 3-a-2. this subset should be replaced in the manner of space saving
				ReplaceSet(subset, subsetLen, map, table, para->hash_size); // select the minimal entry

			}
		}
		UpdateBit(index, stream.num);
	}
}


/**
 * FUNCTIOIN: SetBit
 *	+ set 1 bit at a specific position, and set 0 bit at the other positions;
 * OPTIONS: 
 *	+ index - array
 *	+ size - length of array
 *	+ bit - the position index (from the left hand size)
 * RETURNS:
 *	+ void
 **/
void SetBit(int* array, int size, int bit){
	
	int i;
	for(i = 0; i < size; i++){
		if( i == (size - bit)){
			array[i] = 1;
		}
		else {
			array[i] = 0;
		}
	}
}

/** 
 * FUNCTION: EqualBit
 *	+ check the equality of this array with zero
 * OPTIONS
 *	+ index - array
 *  + size - length of array
 *	+ value - int value
 * RETURNS:
 *	+ int
 **/
int EqualBitZero(int* index, int size){
	
	int i;
	for(i = 0; i < size; i++){
		if( index[i] != 0 ){
			return 0;
		}
	}
	return 1;
}

/**
 * FUNCTION: UpdateBit
 *	+ counting up the one bit
 * OPTIONS:
 *	+ array 
 *  + RETURNS:
 * RETURNS
 *	+ void
 **/
void UpdateBit(int* array, int size){
	
	//increments one
	int i = 0;
	int carrier;
	
	while(i < size){
		//printf("d");
		carrier = (array[i] + 1) / 2;
		
		if( carrier == 1){
			array[i] = 0;
			i++;
		}
		else {
			array[i] = 1;
			break;
		}
	}
}


/**
 * FUNCTION: ReplaceByTransEnum
 *  + replacing the minimal entries with candidate sets wrt n_reg_array 
 * OPTIONS:
 *  + curStream - stream
 *  + table - pointer to the entry table
 *	+ map - pointer to the access map 
 *  + para - pointer to the parameter
 *	+ n_reg_array - the array of indexes each of which corresponds to some non-registered set);
 *  + num_replace_set - the number of candidate sets to be regi
 * RETURN:
 *	+ void
 **/
void ReplaceByTransEnum(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* n_reg_array, int num_replace_set){
	
	// step 3-b. creating the candidate sets wrt n_reg_array
	int i, index;
	
	for(i = 0; i < num_replace_set; i++){
		
		// create the current candidate set
		index = n_reg_array[i]; // index of the current non-registered entry
		int subset[stream.num];
		int subsetLen = GetSubset(subset, index, stream);
		
		if(table->last < para->table_size){
			// step 3-b-1. 
			//this subset can be newly registered in both access map and table
			
			int hash_key = GetHashKey(subset, subsetLen, para->hash_size);
			
			RegistSet(subset, subsetLen, map, table, hash_key);
			
			if(table->last == para->table_size){
				// Now, table becomes full
				para->table_cap = 1;
			}
			if(table->last > para->table_max){
				para->table_max = table->last;
			}
			
		}
		else{
			// step 3-b-2. this subset should be replaced in the manner of space saving
			// select beta such that f(beta) is minimal over the table
			
			ReplaceSet(subset, subsetLen, map, table, para->hash_size);

		}
	}
	
}


/** 
 * FUNCTION: reordering
 *	+ reconstructing the order array into the reorder array
 *	ex. let order be <0, 2, 0, 1> then reorder should become <0, 2, 1, 4>
 * OPTIONS:
 *	+ pointer to the order array
 *	+ number of elements of the order array
 * RETURN:
 *	+ number of items with zero frequency
 **/
int reordering(int* order, int* reorder, int num){
	
	int i;
	//int debagger = 0;
	//int box[num];
	
	PAIR pairs[num];
	// creating the list
	for(i = 0; i < num; i++){
		pairs[i].left = order[i];
		pairs[i].right = i;
	}
	q_sort(pairs, 0, 0, num-1); // quick sorting the pairs by the left value
	
	int zeroNum = 0;
	// scanning the pairs for counting the zero values and changing the left values
	for(i = 0; i < num; i++){
		if( pairs[i].left == 0 ){
			zeroNum++;
		}
		pairs[i].left = i;
	}
	
	q_sort(pairs, 1, 0, num-1); // quick sorting the pairs by the right value
	
	// reseting the values in the order array
	for(i = 0; i < num; i++){
		reorder[i] = pairs[i].left;
	}
	
	return zeroNum;
	
}

/** 
 * FUNCTION: q_sort
 *	+ quick-sorting the array
 * OPTIONS
 *	+ pairs - the array of pairs
 *	+ flag - if 0 then sorting by the left value; otherwise by the right value
 *	+ left - left index for search
 *  + right - right index for search
 * RETURN
 *  + void
 **/
void q_sort(PAIR* pairs, int flag, int s, int e){
	
	int start, end;
	int s_value, e_value;
	int l_temp, r_temp;
	
	//selecting pivot to be modified (11/8)
	int pivot = pairs[s].left; 
	if(flag == 1){
		pivot = pairs[s].right;
	}
	
	//paritioning
	start = s;
	end = e;
	
	while(1){
		//printf("message\n");
		s_value = pairs[start].left;
		e_value = pairs[end].left;
		if(flag == 1){
			s_value = pairs[start].right;
			e_value = pairs[end].right;
		}
		
		while( s_value < pivot ){	
			start++;
			
			//printf("start %d ", start);
			
			s_value = pairs[start].left;
			if(flag == 1){
				s_value = pairs[start].right;
			}
		}
		
		while( e_value > pivot ){
			end--;
			
			//printf("end %d ", end);
			
			e_value = pairs[end].left;
			if(flag == 1){
				e_value = pairs[end].right;
			}
			
		}
		if( start >= end ){
			break;
		}
		//swapping
		l_temp = pairs[start].left;
		r_temp = pairs[start].right;
		pairs[start].left = pairs[end].left;
		pairs[start].right = pairs[end].right;
		pairs[end].left = l_temp;
		pairs[end].right = r_temp;
		
		//updating start and end
		start++;
		end--;
		
	}
	
	//printf("s: %d, start: %d, e: %d, end: %d\n", s, start, e, end);		
	
	
	if( s < start - 1){
		q_sort(pairs, flag, s, start-1);
	}
	//printf("s: %d, start: %d, e: %d, end: %d\n", s, start, e, end);		
	
	if( end + 1 < e){
		//printf("message\n");
		q_sort(pairs, flag, end+1, e);
	}
	
}


/**
 * FUNCTION: IsIncluded
 *	+ checking if the entry set is included in the stream itemset
 * OPTIONS:
 *	+ entry - pointer to the entry
 *	+ stream - the stream
 *	+ order - pointer to the array for frequcies ordering
 * RETURN:
 *	+ int - return 1 if the entry is included: else return 0
 **/
int IsIncluded(ENTRY* entry, STREAM stream, int* order){
	
	int i;
	
	// roughly checking
	if(entry->length > stream.num){
		return 0;
	}
	//printf("entry length: %d\n", entry->length);
	for(i = 0; i < entry->length; i++){
		// we check this in the manner of binary search
		//printf("item: %d, start: %d, end: %d\n", entry->subset[i], 0, stream.num-1);
		if( !isIncluded(entry->subset[i], &stream, 0, stream.num-1, order) )
			return 0;
	}
	return 1;
}

/**
 * FUNCTION: isIncluded
 *	+ checking if the item is included in the stream itemset
 * OPTIONS:
 *	+ item - int number
 *	+ stream - pointer to the stream data
 *	+ start - index for the start point to be searched
 *  + end	- index for the end point to be searched
 *	+ order - pointer to the array for frequcies ordering
 * RETURN:
 *	+ int - return 1 if the item is included: else return 0
 **/
int isIncluded(int item, STREAM* stream, int start, int end, int* order){
	
	// binary search
	if(start == end){
		if(stream->itemset[start] == item){
			// this item is included
			order[start]++;
			return 1;
		}
		else{
			// this item is not included
			return 0;
		}
	}
	else{
		// obtaining the next start and end indexes
		int span = (end - start) / 2;
		//printf("case: span: %d\n", span);
		
		if( stream->itemset[start + span] == item){
			//printf("case 1\n");
			// this item is included
			order[start + span]++;
			return 1;
		}
		else if( stream->itemset[start + span] > item ){
			// this item may exist in the left hand side 
			end = start + span - 1;
			//printf("case 2: start: %d end: %d\n", start, end);
			if(start <= end){
				return isIncluded(item, stream, start, end, order);
			}
			else{
				return 0;
			}
		}
		else {
			// this item may exist in the right hand side
			start = start + span + 1;
			//printf("case 3: start: %d end: %d\n", start, end);
			if(start <= end){
				return isIncluded(item, stream, start, end, order);
			}
			else {
				return 0;
			}
			
		}
	}
}


/**
 * FUNCTION: ReplaceSet
 *	+ replacing the minimal entry with the new subset in the heap sort manner
 *	+ changing the corresponding link element so that it is correctly located in the access map box
 * OPTIONS:
 *	+ subset - pointer to the subset
 *	+ subsetLen - length of the subset
 *	+ map - pointer to the access map
 *	+ table - pointer to the entry table
 *	+ hash_size - hash size
 * RETURN:
 *	+ the estimated count of the minimal entry to be replaced
 **/
int ReplaceSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hash_size){
	
	int parent, child;
	int min_est_count = table->heap[1]->e_count;
		
	// updating each membership value of heap[1] with the new subset
	UpdateEntry(subset, subsetLen, table->delta, map, table->heap[1], hash_size);
	
	// updating the minimal flag of the heap[1] entry
	if(table->min < table->heap[1]->e_count){
		// heap[1] is no longer minimal entry
		table->heap[1]->min_flag = 0;
		table->n_min--;
	}
	
	// index focusing on the initial parent
	parent = 1;
	// index for its left child
	child = child(parent);
		
	// continue to search for the relevant position of the new entry
	while(child <= table->last){
		// change the child index if the e_count of right child < e_count of left child
		if(child != table->last && table->heap[child+1]->e_count < table->heap[child]->e_count){
			child++;
		}
		// swap if the e_count of child < e_count of parent
		if(table->heap[child]->e_count < table->heap[parent]->e_count){
			Swap(table, parent, child, map);
		}
		else{
			break;
		}
		// updating the parent and child indexes
		parent = child;
		child = child(parent);
	}

	return min_est_count;
}

/**
 * FUNCTION: UpdateEntry
 *	+ updating each membership value of heap[1] with the new subset
 * OPTIONS:
 *	+ subset - pointer to the subset
 *	+ subsetLen - length of the subset
 *	+ map - pointer to MAP
 *	+ entry - pointer to the entry to be replaced
 *	RETURN
 *	+ void
 **/
void UpdateEntry(int* subset, int subsetLen, int delta, LIST** map, ENTRY* entry, int hash_size){
	
	int i, hash_key, prev_key;
	int prev_pos;
	
	// 0. getting the previous key of this entry
	prev_key = GetHashKey(entry->subset, entry->length, hash_size);
	prev_pos = entry->link->pos;
	
	// 1. updating the itemset
	for(i = 0; i < subsetLen; i++){
		entry->subset[i] = subset[i];
	}
	
	entry->length = subsetLen;			// 2. updating length
	entry->e_count = 1 + delta;		// 3. updating error count
	entry->t_count = 1;			    // 4. updating true count
	
	// 5.1 getting the new hash key of subset;
	hash_key = GetHashKey(subset, subsetLen, hash_size);
	
	// 5.2 updating the link
	if(hash_key != prev_key){
		LIST* temp = map[hash_key];
		//creaning the prev and next of entry up 
		
		//updating entry->link->prev
		if(entry->link->prev != NULL){
			// updating entry->link->prev->next with entry->link->next
			entry->link->prev->next = entry->link->next;
		}
		else{
			// this entry is head element
			map[prev_key] = entry->link->next;
		}
		//updating entry->link->next
		if(entry->link->next != NULL){
			entry->link->next->prev = entry->link->prev;
		}
		
		if(temp == NULL){
			//updating entry->link
			map[hash_key] = entry->link;
			entry->link->prev = NULL;
			entry->link->next = NULL;
		}
		else{
			//updating entry->link
			map[hash_key] = entry->link;
			temp->prev = entry->link;
			entry->link->next = temp;
			entry->link->prev = NULL;
		}
	}
}


/**
 * FUNCTION: GetMinSetTable
 *	+ removing the minimal entry from the table
 * OPTIONS
 *	+ table - pointer to the TABLE
 * RETURN:
 *	+ ENTRY* - the pointer to the entry 
 **/
ENTRY* GetMinSetTable(TABLE* table, LIST** map)
{ 
	int parent, child;
	ENTRY* min = table->heap[1];
	table->heap[1] = table->heap[table->last];
	table->last--;
	
	// index focusing on the initial parent
	parent = 1;
	
	// index for its left child
	child = child(parent);
	
	// continue to search for the relevant position of the new entry
	while(child <= table->last){
		
		// change the child index if the e_count of right child < e_count of left child
		if(table->heap[child+1]->e_count < table->heap[child]->e_count){
			child++;
		}
		// swap if the e_count of child < e_count of parent
		if(table->heap[child]->e_count < table->heap[parent]->e_count){
			Swap(table, parent, child, map);
		}
		else{
			break;
		}
		// updating the parent and child indexes
		parent = child;
		child = child(parent);
	}
	
	return min;
	
}


/**
 * FUNCTION: GetSubset
 *	+ creating the subset
 * OPTIONS:
 *	+ pointer to the newly created subset that will be changed in the process
 *	+ index: int value
 *	+ stream: current stream
 * RETURN:
 *  + length of the created subset
 **/
int GetSubset(int* subset, int id, STREAM stream){
	
	int i, j;
	//initializing the entry
	j = 0;
	for(i = 0; i < stream.num; i++){
		if((id >> i) & 1){
			subset[j] = stream.itemset[i];
			j++;
		}
	}	
	return j; //the length of this subset
}


/**
 * FUNCTION: GetSubsetFromArray
 *	+ creating the subset
 * OPTIONS:
 *	+ pointer to the newly created subset that will be changed in the process
 *	+ index: int value
 *	+ stream: current stream
 *	+ array:  order array
 * RETURN:
 *  + length of the created subset
 **/

int GetSubsetFromArray(int* subset, int* id, STREAM stream, int* array){
	
	// example
	// int* subset: <0, 3, 6, 7>
	// int* order:  <3, 2, 0, 1> <= this shows each rank wrt the element's frequency
	//								for instance, the item 0 is 3rd rank on the frequency
	// from the above two arraies, we describe the reordered subset wrt the frequency as follows
	// <6, 7, 3, 0> 
	// Thus, given int id <1, 0, 1, 0>,   
	// the corresponding subset <3, 6> is derived with the subset and order as the following process
	// 
	
	int i, j, k;
	
	//initializing the entry
	j = 0;
	for(i = 0; i < stream.num; i++){
		k = array[i];
		//printf("id = %d, k = %d, i = %d", id, k, i);
		if(id[stream.num - k - 1] == 1){
			//printf(" => 1 \n");
			subset[j] = stream.itemset[i];
			j++;
		}
		else {
			//printf(" => 0 \n");
		}
	}	

	return j; //the length of this subset
	
}

/**
 * FUNCTION: RegistSet
 *	+ register the corresponding entry to the table in the manner of heap sort
 *	+ register the corresponding entry to the access map
 * OPTIONS:
 *	+ subset: pointer to the candidate subset
 *	+ subsetLen: the length of candidate subset
 *	+ map: pointer to the MAP
 *	+ table: pointer to the TABLE
 *	+ hashKey: hash key
 *	+ collision: pointer to the collision number
 * RETURN:
 *	+ void
 **/
void RegistSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hashKey){
	
	int parent, child;
	
	// step 1 insert the subset to entry table (heap insert)
	
	// adding the new entry to the last of heap
	table->last++;
	
	// deleting num of minimal entries
	table->n_min--;
	
	LIST* newList = CreateList(hashKey, table->last, map);
	
	table->heap[table->last] = CreateEntry(subset, subsetLen, table->delta, newList);

	// index currently focusing on the child entry
	child = table->last;
	// index currently focusing on the parent entry
	parent = parent(child);
	
	// continue to search the relevant position of nEntry
	// heap sort (asending order)
	while(parent != 0){
				
		// swap if parent's estimated count > child's estimated count
		if( table->heap[parent]->e_count > table->heap[child]->e_count){
			Swap(table, parent, child, map); 
		}
		// else finish the search
		else{
			//printf("break\n");
			break;
		}
		
		// updating the parent and child indexes
		child = parent;
		parent = parent(child);
	}
	
}

/**
 * FUNCTION: Swap
 *	+ swap the parent entry with the child entry
 * OPTIONS:
 *	+ table - pointer to the Table
 *	+ parent - parent index
 *	+ child - child index
 * RETURN:
 *	+ void
 **/
void Swap(TABLE* table, int parent, int child, LIST** map){
	// swap the every memembership value
	// Note that the link value indicates the pointer to the access map.
	// Its member pos value should correspond to index of this entry in the table
	// We thus need to change the pos value too.
	
	ENTRY* temp;
	
	table->heap[parent]->link->pos = child;
	table->heap[child]->link->pos = parent;
	
	temp = table->heap[parent];
	table->heap[parent] = table->heap[child];
	table->heap[child] = temp;
	
}


/**
 * FUNCTION: CreateEntry
 *	+ Create the new entry corresponding the current subset
 * OPTIONS:
 *	+ subset: pointer to the subset
 *	+ subsetLen: length of the subset
 *	+ delta: the maximal error at this moment
 *	+ link: the pointer to the newly created LIST
 * RETURN
 *	+ void
 **/
ENTRY* CreateEntry(int* subset, int subsetLen, int delta, LIST* link){
	// creating the new entry
	int i;
	ENTRY* newEntry = (ENTRY*)malloc(sizeof(ENTRY));
	
	if(newEntry == NULL){
		fprintf(stderr, "cannot allocate memory to the new entry\n");
		exit(0);
	}
	
	for(i = 0; i < SUBSEQ_LEN; i++){
		if(i < subsetLen){
			newEntry->subset[i] = subset[i];
		}
		else {
			newEntry->subset[i] = -1;
		}
	}
	
	// setting the other member values of entry
	newEntry->e_count = 1 + delta;
	newEntry->t_count = 1;
	newEntry->length = subsetLen;
	newEntry->link = link;
	
	return newEntry;
}

/**
 * FUNCTION: CreateList
 *	+ creating the new LIST instance in the access map
 * OPTIONS:
 *	+ hashKey - hash key
 *	+ pos - position of the corresponding entry in the entry table
 *	+ map - pointer to the access map
 * RETURN:
 *	+ pointer to the new LIST
 **/
LIST* CreateList(int hashKey, int pos, LIST** map){
	
	LIST* newList = (LIST*)malloc(sizeof(LIST));
	
	if(newList == NULL){
		fprintf(stderr, "cannot allocate memory to the new lists\n");
		exit(0);
	}
	
	newList -> pos = pos;
	newList -> prev = NULL;
	newList -> next = NULL;
	LIST* temp = map[hashKey];
	
	// adding this to the top of linked list
	// if temp is NULL, then just add newList to map[hashKey]
	
	//fprintf(stderr, "a");
	
	if(temp == NULL){
		//fprintf(stderr, "bb");
		map[hashKey] = newList;
	}
	else{
		//fprintf(stderr, "bc");
		// map[hashKey] ==> newList
		map[hashKey] = newList;
		// newList ==> temp
		newList -> next = temp;
		// temp ==> newList
		temp -> prev = newList;
	}
	//fprintf(stderr, "d");
	return newList;
}



/**
 * FUNTION: GetRegistSet
 *	+ return the subset if it is registered 
 * OPTIONS:
 *	+ subset: the pointer to the candidate set
 *	+ map:	pointer to the access map
 *	+ table: pointer to the table
 *  + hashKey: hash key
 * RETURN:
 *	+ index of the corresponding entry in the table
 **/
ENTRY* GetRegistSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hashKey){
	
	// step 1. getting the box corresponding the hash key (chaining method)
	//fprintf(stderr, "s");
	
	LIST* entryKey = map[hashKey];
	//listPrint(entryKey);
	
	//fprintf(stderr, "c");
	//int num = 100;
	
	// step 2. finding the corresponding the subset
	while(entryKey != NULL){
		
		ENTRY* entry; 
		
		//fprintf(stderr, "-> %d. %d", entryKey->pos, table->last);
		//fprintf(stderr, ".k%d.", entryKey->pos);
		
		if(entryKey->pos >= 0 && entryKey->pos <= table->last){
			entry = table->heap[entryKey->pos];	
		}
		else{
			fprintf(stderr, "Error at GetRegistSet (%d. %d)", entryKey->pos, table->last);
			exit(0);
		}
		//EntryPrint(entry);
		//fprintf(stderr, "d");
		
		if(IsMatched(entry, subset, subsetLen)){
			//fprintf(stderr, "e");
			// finding the corresponding the subset
			return entry;
		}
		//fprintf(stderr, "f");
		entryKey = entryKey->next;
		//fprintf(stderr, "h");
	}
	//fprintf(stderr, "i");
	
	// this subset is not registered yet
	return NULL;
}


/**
 * FUNTION: GetHashKey
 *	+ return the hash key of the input subset
 * OPTIONS
 *	+ subset - the pointer to the input subset
 *	+ hash_size - hash size
 * RETURN:
 *	* key - hash key
 */
int GetHashKey(int* subset, int length, int hash_size)
{
	unsigned long int h = 0;
	int i, pos;
	int key;
	int primes[7] = {1,547,1229,1993,2749,3581,4421};
	
	//prime numbers
	/*
	 int sosu[168]=
	 {2,3,5,7,11,13,17,19,23,29,
	 31,37,41,43,47,53,59,61,67,71,
	 73,79,83,89,97,101,103,107,109,113,
	 127,131,137,139,149,151,157,163,167,173,
	 179,181,191,193,197,199,211,223,227,229,
	 233,239,241,251,257,263,269,271,277,281,
	 283,293,307,311,313,317,331,337,347,349,
	 353,359,367,373,379,383,389,397,401,409,
	 419,421,431,433,439,443,449,457,461,463,
	 467,479,487,491,499,503,509,521,523,541,
	 547,557,563,569,571,577,587,593,599,601,
	 607,613,617,619,631,641,643,647,653,659,
	 661,673,677,683,691,701,709,719,727,733,
	 739,743,751,757,761,769,773,787,797,809,
	 811,821,823,827,829,839,853,857,859,863,
	 877,881,883,887,907,911,919,929,937,941,
	 947,953,967,971,977,983,991,997};*/
	
	
	int size = sizeof(primes) / sizeof(int);
	
	for(i = 0; i < length; i++){
		if(i >= size)
			pos= (int)i % size;
		else
			pos = i;
		h += (subset[i] * primes[pos]); 
	}
	key = (int)h % hash_size;
	return key;
}

/**
 * FUNTION: IsMatched
 *	+ return 1 or 0 if the entry is matched with the current candidate set
 * OPTIONS
 *	+ entry - entry
 *	+ subset - pointer to the current candidate set
 * RETURN:
 *	* 1 or 0
 **/
int IsMatched(ENTRY* entry, int* subset, int subsetLen){
	
	if(entry == NULL){
		return 0;
	}
	if(entry->length != subsetLen){
		// unmatched (for debagging)
		//fprintf(stderr, "ERROR: in IsMatched function\n");
		return 0;
	}
	
	int i;
	int* itemset = entry->subset;
	
	for(i = 0; i < subsetLen; i++){
		if(subset[i] != itemset[i]){
			// unmatched
			return 0;
		}
	}
	return 1;
}


void increment(TABLE* table){

	int i;
	for(i = 1; i <= table->last; i++){
		 // incrementing the estimated count in the table
		table->heap[i]->e_count++;
	}
	
}



/*
 FUNCTION: MakeTable
 +function for allocating the memory of the entry table
 OPTIONS:
 +*para - pointer to the PARAMETER value
 RETURN:
 +table - pointer to the TABLE value
 */
TABLE *MakeTable(PARAMETER *para)
{
	//int i, j;
	TABLE *entryTable = (TABLE *)malloc(sizeof(TABLE));
	
	fprintf(stderr,"MakeTable\t");
	
	/*memory allocation of entry table*/
	entryTable->heap = (ENTRY**)malloc(sizeof(ENTRY*) * (para->table_size+1)); 
	if(entryTable->heap == NULL){
		fprintf(stderr,"TABLE: malloc failed...\n");
		exit(1);
	}
	
	entryTable->delta = 0;
	entryTable->last = 0;
	entryTable->t_skip_num = 0;
	
	/*Initializng the values of parameters on skip operations*/
	entryTable->registNum = 0;		//the number of candidate sets that have been registered
	entryTable->min = 0;			//the estimated count of the minimal entry at the start time
	entryTable->n_min = para->table_size;	//the number of minimal entires at the start time
	
	fprintf(stderr,"OK!\n");
	
	return entryTable;
	
}


/*
 FUNCTION: InitMap
 +function for initializing the access map
 OPTIONS:
 +*map - pointer to the LIST value
 RETURN:
 +void
 */
void InitMap(LIST** map, int hash_size)
{
	int i;
	//LIST* p; //Chaining method
	
	fprintf(stderr, "InitMap\t\t");
	
	/*Initialization (modified in 11/8 with fukuda)*/
	for(i = 0; i < hash_size; i++){
		
		/*
		 p = (LIST*)malloc(sizeof(LIST));
		 if(p == NULL){
		 fprintf(stderr,"malloc failed (InitMap)...\n");
		 exit(1);
		 }
		 p->pos = -1;
		 p->next = NULL;
		 p->prev = NULL;
		 map[i] = p;*/
		
		map[i] = NULL;
	}
	
	fprintf(stderr,"OK!\n");	
}



/*
 FUNCTION: GetStream
 + reading the stream file (.seq) into the stream and return the pointer to stream
 OPTIONS:
 + *para - pointer to the PARAMETER value
 RETURN:
 + *stream - pointer to the STREAM value
 */
STREAM* GetStream(PARAMETER *para)
{
	FILE *input;
	//int num;
	int i, j;
	int dust;
	STREAM* stream;
	
	fprintf(stderr, "GetStream\t");
	
	if((input=fopen(para->in_filename,"r"))==NULL){
		fprintf(stderr,"Can not open \"%s\"\n",para->in_filename);
		exit(0);
	}
	
	/*Obtaineing the number of item kinds and stream size*/
	fscanf(input,"itemtype %d\n",&para->itemtype);
	if(para->stream_len == -1){
		fscanf(input,"sequence %d\n",&para->stream_len);
	}
	else
		fscanf(input,"sequence %d\n",&dust);
	
	/*Allocating the memory size of input streams*/
	stream=(STREAM *)malloc( sizeof(STREAM) * para->stream_len );  
	if(stream == NULL){
		fprintf(stderr,"malloc failed...\n");
		exit(1);
	}
	
	/*Debagging (number of item kinds and size of input streams*/
	
	/*Reading the input streams*/  
	for(i = 0; i < para->stream_len; i++){
		fscanf(input, "itemset %d\n", &stream[i].num); //Obtaining the number of items
		stream[i].itemset = (int*)malloc(sizeof(int)*stream[i].num); //Allocating the itemset
		for(j = 0; j < stream[i].num; j++)
			fscanf(input, "%d\n", &stream[i].itemset[j]);//Obtaining each item
	}
	
	fclose(input);
	
	fprintf(stderr,"OK!\n");
	
	return stream;
	
}


/*
 FUNCTION: ShowParameter
 + outputting the parameters as stderr
 OPTIONS:
 + *para - pointer to the PARAMETER value
 RETURN:
 + void
 */
void ShowParameter(PARAMETER *para, TABLE* table)
{
	float pre = 0;
	float rec = 0;
	
	// computing precision and recall values
	if(para->lcm_check){
		pre = (float)(para->match_freq_items*100/para->num_outputs);
		rec = (float)(para->match_freq_items*100/para->num_freq_items);
	}
	fprintf(stderr,"inputFile: %s\n",para->in_filename);
	fprintf(stderr,"outputFile: %s (ans/table/map/csv)\n",para->out_filename);
	fprintf(stderr,"prunning: <");
	if(para->r_skip){
		fprintf(stderr,"r_skip. ");
	}
	if(para->t_skip){
		fprintf(stderr,"t_skip. ");
	}
	if(para->reduce){
		fprintf(stderr,"reduction. ");
	}
	fprintf(stderr, ">\n");
	fprintf(stderr,"min support: %.2f\n", para->support*para->time);
	fprintf(stderr,"num of streams: %d\n",para->stream_len);
	fprintf(stderr,"num of item types: %d\n",para->itemtype);
	fprintf(stderr,"max length of streams: %d\n", para->max_stream_len);
	fprintf(stderr,"ave length of streams: %.2f\n", (double)para->total_stream_len/para->time);
	fprintf(stderr,"delta: %d\n", table->delta);
	fprintf(stderr,"num of outputs: %d\n", para->num_outputs);
	fprintf(stderr,"num of frequent sets: %d\n",para->num_freq_items);
	fprintf(stderr,"num of matched sets: %d\n",para->match_freq_items);
	fprintf(stderr,"recall: %.2f\n", rec);
	fprintf(stderr,"precision: %.2f\n",pre);
}


/*
 FUNCTION: ShowStream
 + outputting the input streams (for debagging)
 OPTIONS:
 + *stream - pointer to the STREAME data
 + *para - pointer to the PARAMETER value
 RETURNS:
 + void
 */ 
void ShowStream(STREAM *stream, PARAMETER *para)
{
	int i;
	
	//printf("---ShowStream---\n");
	
	printf("stream_len=%d\n",para->stream_len);
	
	for(i = 0; i < stream->num; i++){
		printf("%d ", stream->itemset[i]);
	}
	printf("\n");  
}


/**
 * FUNCTION: OutputTable
 *	+ outputting the entry table
 * OPTIONS:
 *	+ entryList - list of all the entries 
 *	+ table - pointer to the TABLE 
 *	+ para - pointer to the PARAMETER
 * RETURN:
 * + void
 **/
void OutputTable(TABLE *table, PARAMETER *para)
{
	FILE *ft;
	int i,j;
	char outTable[FILE_LEN];
	
	// making the output file(.table)
	fprintf(stderr,"OutputTable\t");
	strcpy(outTable,para->out_filename);
	strcat(outTable,".table");
	
	if((ft=fopen(outTable,"w"))==NULL){
		fprintf(stderr,"CAN NOT OPEN \"%s\"\n",outTable );
		exit(0);
	} 
	
	fprintf(ft,"---OutputTable---\n");
	fprintf(ft,"time=%d\n",para->time-1); //the time step when the process has been stopped
	fprintf(ft,"table_size=%d\n",table->last); //the size of entry table
	
	// outputting all the content in the entry table
	for(i = 1; i <= table->last; i++)
	{
		fprintf(ft,"[%d] - ",i);
		fprintf(ft,"<");
		ENTRY* entry_i = table->heap[i];
		for(j = 0; j < entry_i->length; j++)
			fprintf(ft,"%d ",entry_i->subset[j]);
		fprintf(ft,"> ");
		fprintf(ft,"true count=%d, ", entry_i->t_count);
		fprintf(ft,"estimated count=%d, ", entry_i->e_count);
		fprintf(ft,"Key=%d\n", GetHashKey(entry_i->subset, entry_i->length, para->hash_size));      
	}
	
	fprintf(stderr,"OK!\n");
	
}

/**
 * FUNCTIONS: OutputResult
 *	+ outputting the frequent itemsets
 * OPTIONS:
 *	+ table - pointer to the TABLE
 *  + para - pointer to the PARAMETER
 * RETURN:
 *	+ void
 **/

void OutputResult(LIST** map, TABLE *table, PARAMETER *para)
{
	int i, j;
	int num = 0;
	FILE *output;
	FILE *experiment;
	char outRes[FILE_LEN];
	char exeRes[FILE_LEN];
	
	
	strcpy(outRes,para->out_filename);
	strcpy(exeRes,para->out_filename);
	
	char k[64];
	sprintf(k,"%d", para->table_size);
	strcat(outRes, k);
	
	strcat(outRes,".ans");
	strcat(exeRes,".txt");
	
	fprintf(stderr,"OutputResult\t");

	if((output=fopen(outRes,"w"))==NULL){
		fprintf(stderr,"Can not open \%s\"\n",para->out_filename);
		exit(0);
	}
	if((experiment=fopen(exeRes,"a"))==NULL){
		fprintf(stderr,"Can not open \%s\"\n",para->out_filename);
		exit(0);
	}
	
	// outputting the file
	fprintf(output,"InputFile: %s\n",para->in_filename);
	fprintf(output,"OutputFile: %s\n",para->out_filename);
	fprintf(output,"stream_length: %d\n",para->stream_len);
	fprintf(output,"min-spport: %.3f\n",para->support);
	fprintf(output,"max-error: %.3f\n",para->error);
	if(para->table_cap == 1)
		fprintf(output,"table_size_cap: %d \n",para->table_size);
	else
		fprintf(output,"table_size_cap: OFF\n");
	fprintf(output,"time=%d, s*t=%.2f, e*t=%.2f, delta=%d, t_skip=%d, table_size (MAX) = %d (%d)\n",
			para->time-1,
			para->support*(para->time-1),
			para->error*(para->time-1),
			table->delta,
			table->t_skip_num,
			table->last,
			para->table_max);

	fprintf(output,"seq, est_count, t_count\n");
	
	int entryNum = table->last;
	// Search all the entries
	for(i = 0; i < entryNum; i++){
		
		ENTRY* min = GetMinSetTable(table, map);
		int est_count = min->e_count;
		//int thresh_hold = para->support*(para->time-1) - table->t_skip_num;
		//int thresh_hold = para->support*(para->time-1);
		int thresh_hold = table->delta+1;
		if(est_count >= thresh_hold){
			//printf("this is frequent set\n");
			//EntryPrint(min);
			// this is a frequent itemset
			fprintf(output,"<");
			for(j = 0; j < min->length; j++)
				fprintf(output,"%d.",min->subset[j]);
			fprintf(output,">,");
			
			// frequent information of this entry
			fprintf(output,"%d,", min->e_count);
			fprintf(output,"%d\n",min->t_count); 
			num++; // the number of frequent itemsets
			free(min);
			min = NULL;
		}
		else{
			//printf("this is non frequent set\n");
			//EntryPrint(min);
		}
	}
	
	//updating the number of outputs
	para->num_outputs = num;
	
	if(num != 0){
		// the number of outputs
		fprintf(output,"Num of outputs: %d\n", num); 
	}
	else{
		fprintf(output,"No outputs\n");
	}
	
	//computing the precision 
	double total_time = (double)(para->time_end - para->time_start)/(double)CLOCKS_PER_SEC;
	//fprintf(output,"End   date: %s",asctime(para->date_end));
	fprintf(output,"CPU time: %.3f sec\n",total_time);
	
	fclose(output);
	
	//double pre = (float)(para->match_freq_items*100/para->num_outputs);
	//double rec = (float)(para->match_freq_items*100/para->num_freq_items);
	
	fprintf(experiment, "%s, %d, %d, %d, %f, %f, %f\n",
			para->in_filename,
			para->time,
			para->table_size,
			table->delta, 
			(double)table->delta/para->time, 
			total_time, 
			(double)total_time/para->time
			);
	
	fclose(experiment);
	
	fprintf(stderr,"OK!\n");
	
}


/**
 * FUNCTION: OutputMap
 *	+ outputting the access map
 * OPTIONS
 *	+ map - pointer to LIST*
 *	+ para - pointer to PARAMETER
 * RETURN
 *	+ void
 **/
void OutputMap(LIST** map, TABLE* table, PARAMETER *para)
{
	FILE *fm;
	int i;	
	char outMap[FILE_LEN];
	
	fprintf(stderr,"OutputMap\t");
	
	strcpy(outMap,para->out_filename);
	strcat(outMap,".map");
	
	if((fm=fopen(outMap,"w"))==NULL){
		fprintf(stderr,"CAN NOT OPEN \"%s\"\n",para->out_map);
		exit(0);
	} 
	fprintf(fm,"---OutputMap---\n");
	fprintf(fm,"collision: %d\n", para->collision);
	
	for(i = 0; i < para->hash_size; i++)
	{
		LIST* m = map[i];
		while(m != NULL){
			fprintf(fm, "map[%d]:", i);
			
			fprintf(fm, "%d. ", m->pos);
			if(m->next != NULL){
				para->collision++;
				fprintf(fm," -> ");
			}
			m = m->next;
			fprintf(fm,"\n");    
		}
	}
	fclose(fm);
	
	fprintf(stderr,"OK!\n");
	
}

/**
 * FUNCTION: FreeMap
 *	+ make the access map free
 * OPTIONS: 
 *	+ map - pointer to MAP
 *	+ hash_size - hash size
 * RETURN:
 *	+ void
 **/
void FreeMap(LIST** map, int hash_size)
{
	int i;
	fprintf(stderr,"FreeMap    \t");
	
	for(i = 0; i < hash_size; i++){
		while(map[i] != NULL){
			//fprintf(stderr, "%d\n", i);
			LIST* m = map[i];
			map[i] = map[i]->next;
			free(m);
			m = NULL;
		}
	}
	fprintf(stderr,"OK!\n");
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

/**
 * FUNCTION: FreeTable
 *	+ opening the entry table
 * OPTIONS
 *	+ table - pointer to TABLE
 *	+ para - pointer to PARAMETER
 * RETURN
 *	+ void
 **/
void FreeTable(TABLE *table, PARAMETER *para)
{
	fprintf(stderr,"FreeTable\t");
	
	int i;
	for(i = 0; i < table->last; i++){
		free(table->heap[i]);
		table->heap[i] = NULL;
	}
	
	free(table);
	free(para);
	table = NULL;
	para = NULL;
	
	fprintf(stderr,"OK!\n");
}

/** 
 * FUNCTION: EntryPrint
 *	+ printing the entry
 * OPTIONS:
 *	+ entry - pointer to the entry
 * RETURNS:
 *	+ void
 **/
void EntryPrint(ENTRY* entry){
	int i;
	printf("+ entry: <");
	for(i = 0; i < entry->length; i++){
		printf("%d. ", entry->subset[i]);
	}
	printf("> est_count: %d, t_count: %d, min_flag: %d, pos %d\n", entry->e_count, entry->t_count, entry->min_flag, entry->link->pos);
}

/** 
 * FUNCTION: SubsetPrint
 *	+ printing the subset
 * OPTIONS:
 *	+ entry - pointer to the entry
 * RETURNS:
 *	+ void
 **/
void SubsetPrint(int* subset, int length){
	int i;
	fprintf(stderr, "< ");
	for(i = 0; i < length; i++){
		fprintf(stderr, "%d. ", subset[i]);
	}
	fprintf(stderr, ">\n");
}

// for debagging
void pairPrint(PAIR* pairs, int length){
	int i;
	fprintf(stderr, "pairs: ");
	for(i = 0; i < length; i++){
		fprintf(stderr, "<%d, %d>.", pairs[i].left, pairs[i].right);
	}
	fprintf(stderr, "\n");
}

// for debagging
void tablePrint(TABLE* table){
	int i;
	printf("min: %d, n_min: %d\n", table->min, table->n_min);
	for(i = 1; i <= table->last; i++){
		EntryPrint(table->heap[i]);
	}
	
}

// for debagging
void listPrint(LIST* key){
	LIST* tmp = key;
	while(tmp != NULL){
		fprintf(stderr, "%d.. ", tmp->pos);
		tmp = tmp->next;
	}
	fprintf(stderr, "\n");
}


#endif


