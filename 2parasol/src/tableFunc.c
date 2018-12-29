/*
 tableFunc.c
 
 functions which are used for accessing entry table and hash map
 
 */

// Definition of data structures and constant symbols

//#include"dataStructI.h" 
//#include"functionsI.h"
#include"entryTable.h"
#include"tableFunc.h"
#include"treeFunc.h"

// declaration of local functions
int isIncluded(int item, STREAM* stream, int start, int end, int* order);
void swap(TABLE* table, TREE* tree, int parent, int child, LIST** map);


/**
 * FUNCTION: Update
 * + function for updating the entry table wrt a new transaction
 * OPTIONS:
 * + tree - pointer to the TREE value
 * + stream - pointer to the STREAM value
 * + table - pointer to the TABLE value
 * + map - pointer to the access map
 * + para - pointer to the PARAMETER value
 * RETURN:
 * + void
 **/ 
void UpdateTable(TREE* tree, STREAM stream, TABLE* table, LIST** map, PARAMETER* para)
{
    // incremental intersection
    UpdateNode(tree->root, stream.itemset, stream.num, tree, table, map);
    
    if(para->lc == 1 || para->ss == 1){
        // minimal entry deletion
        Remove(tree, table, map, para);
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
TABLE* MakeTable(int table_max)
{
	//int i, j;
	TABLE* entryTable = (TABLE *)malloc(sizeof(TABLE));
	
	fprintf(stderr,"MakeTable\t");
	
	/*memory allocation of entry table*/
	// remark: we are not using the first box in heap (heap[0]) 
	entryTable->heap = (ENTRY**)malloc(sizeof(ENTRY*) * (table_max+1));
	if(entryTable->heap == NULL){
		fprintf(stderr,"TABLE: malloc failed...\n");
		exit(1);
	}
	
	entryTable->delta = 0;
	entryTable->last = 0;
	//entryTable->t_skip_num = 0;
	
	/*Initializng the values of parameters on skip operations*/
	//entryTable->registNum = 0;		//the number of candidate sets that have been registered
	//entryTable->min = 0;			//the estimated count of the minimal entry at the start time
	//entryTable->n_min = para->table_size;	//the number of minimal entires at the start time
	
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
LIST** InitMap()
{
	int i;
	//LIST* p; //Chaining method
	
    fprintf(stderr, "Init Map\t\t");
    LIST** map = (LIST**)malloc(sizeof(LIST*) * HASH_SIZE);
    if(map == NULL){
        fprintf(stderr, "fail to allocation ...");
        exit(0);
    }
	/*Initialization (modified in 11/8 with fukuda)*/
	for(i = 0; i < HASH_SIZE; i++){
		map[i] = NULL;
	}
	fprintf(stderr,"OK!\n");
    return map;
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
void FreeMap(LIST** map)
{
	int i;
	fprintf(stderr,"FreeMap    \t");
	
	for(i = 0; i < HASH_SIZE; i++){
		while(map[i] != NULL){
			//fprintf(stderr, "%d\n", i);
			LIST* m = map[i];
			map[i] = map[i]->next;
			free(m);
			//m = NULL;
		}
	}
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
	fprintf(stderr,"CAN NOT OPEN %s\n",outTable );
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
		fprintf(ft,"Key=%d\n", GetHashKey(entry_i->subset, entry_i->length, HASH_SIZE));
	}
	fclose(ft);
	
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
void OutputResult(LIST** map, TABLE *table, TREE* tree, PARAMETER *para)
{
	int i, j;
	int num = 0;
	FILE *output;
	char outRes[FILE_LEN];
	
	fprintf(stderr,"OutputResult\t");
	
	strcpy(outRes,para->out_filename);
	strcat(outRes,".ans");
	
	if((output=fopen(outRes,"w"))==NULL){
		fprintf(stderr,"Can not open \%s\n",para->out_filename);
		exit(0);
	}
	
	// outputting the file
	fprintf(output,"InputFile: %s\n",para->in_filename);
	fprintf(output,"OutputFile: %s\n",para->out_filename);
	fprintf(output,"stream_length: %d\n",para->time);
	fprintf(output,"mininal spport threshold: %.3f\n",para->support);
    if(para->lc){
        fprintf(output,"error parameter: %.3f\n", para->error);
    }
    if(para->ss){
        fprintf(output,"size constand: %d\n", para->table_size);
    }
    
	fprintf(output,"t=%d, s*t=%.2f, e*t=%.2f, delta=%d, table_size (MAX) = %d (%d)\n",
			para->time,
			para->support*(para->time),
			para->error*(para->time),
			table->delta,
			table->last,
			para->table_size);
	
	fprintf(output,"<itemset, frequency count, true count (frequency count - error count) >\n");
	
	int entryNum = table->last;
	int est_count;
	
    int threshold = ceil(para->support * para->time);
    
    // Search all the entries
	for(i = 0; i < entryNum; i++){
		ENTRY* min = GetSetTable(1, table, tree, map);
		est_count = min->e_count;
		if(est_count >= threshold){
			// this is an itemset to be output
			fprintf(output,"<");
			for(j = 0; j < min->length; j++)
				fprintf(output,"%d.",min->subset[j]);
			fprintf(output,">,");
			
			// frequent information of this entry
			fprintf(output,"%d,", min->e_count);
			fprintf(output,"%d\n",min->t_count); 
			num++; // the number of frequent itemsets
			free(min);
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
	
	//fprintf(output,"End   date: %s",asctime(para->date_end));
	fprintf(output,"CPU time: %.3f sec\n",(double)(para->time_end - para->time_start)/(double)CLOCKS_PER_SEC);
	
	fclose(output);	
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
		fprintf(stderr,"CAN NOT OPEN %s\n",para->out_map);
		exit(0);
	} 
	fprintf(fm,"---OutputMap---\n");
	
	for(i = 0; i < HASH_SIZE; i++)
	{
		LIST* m = map[i];
		while(m != NULL){
			fprintf(fm, "map[%d]:", i);
			
			fprintf(fm, "%d. ", m->pos);
			if(m->next != NULL){
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
 * FUNCTION: GetSetTable
 *	+ removing the minimal entry from the table
 * OPTIONS
 *	+ table - pointer to the TABLE
 * RETURN:
 *	+ ENTRY* - the pointer to the entry 
 **/
ENTRY* GetSetTable(int index, TABLE* table, TREE* tree, LIST** map)
{ 
	int parent, child;
	ENTRY* target = table->heap[index];

	//fprintf(stderr, "index = index\n");
	target->link->pos = -1;
	table->heap[index] = table->heap[table->last];
	// modify 2015/4/28
	table->heap[index]->link->pos = index;
	table->heap[index]->n_link->pos = index;
	table->last--;
	
	// index focusing on the initial parent
	parent = index;
	
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
			swap(table, tree, parent, child, map);
		}
		else{
			break;
		}
		// updating the parent and child indexes
		parent = child;
		child = child(parent);
	}
	
	return target;
	
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
    unsigned long int key;
	int primes[7] = {1,547,1229,1993,2749,3581,4421};
	
	//prime numbers
	/*
	 int primes[168]= {2,3,5,7,11,13,17,19,23,29,
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
	 947,953,967,971,977,983,991,997};
	*/
	int size = sizeof(primes) / sizeof(int);
	
	for(i = 0; i < length; i++){
		if(i >= size)
			pos= (int)i % size;
		else
			pos = i;
		h += (subset[i] * primes[pos]);
        
	}
	key = (unsigned long int)h % hash_size;
    //fprintf(stderr, "%ld\n", key);
	return key;
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
	LIST* entryKey = map[hashKey];
	// step 2. finding the corresponding the subset
	while(entryKey != NULL){
		
        ENTRY* entry;
		
		//if(entryKey->pos >= 0 && entryKey->pos <= table->last){
		//modified 2015/2/22
		if(entryKey->pos >= 0 && entryKey->pos <= table->last){
			entry = table->heap[entryKey->pos];
		}
		else{
			fprintf(stderr, "Error at GetRegistSet (%d. %d)", entryKey->pos, table->last);
			exit(0);
		}
		
		if(IsMatched(entry, subset, subsetLen)){
			//finding the corresponding the subset
			return entry;
		}
		entryKey = entryKey->next;
	}
	// this subset is not registered yet
	return NULL;
}


/**
 * FUNCTION: RegistEntry
 *	+ register the corresponding entry to the table in the manner of heap sort
 *	+ register the corresponding entry to the access map
 * OPTIONS:
 *	+ entry: pointer to the registered entry
 *	+ map: pointer to the MAP
 *	+ table: pointer to the TABLE
 * RETURN:
 *	+ void
 **/
void RegistEntry(ENTRY* entry, TREE* tree, LIST** map, TABLE* table)
{
	
	int parent, child;
	
	// step 1 insert the subset to entry table (heap insert)
	
	// adding the new entry to the last of heap
	table->last++;
		
	table->heap[table->last] = entry;
	
	// index currently focusing on the child entry
	child = table->last;
	// index currently focusing on the parent entry
	parent = parent(child);
	
	// continue to search the relevant position of nEntry
	// heap sort (asending order)
	while(parent != 0){
		
		// swap if parent's estimated count > child's estimated count
		if( table->heap[parent]->e_count > table->heap[child]->e_count){
			swap(table, tree, parent, child, map); 
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
 * FUNCTION: ResetList
 */
void ResetList(LIST* list, LIST** map, int hash_key)
{	
	//fprintf(stderr, "a");

	if(list->pos == map[hash_key]->pos){
		//fprintf(stderr, "b");

		// reset the head of map list
		if(list->next != NULL){
			map[hash_key] = list->next;
			list->next->prev = NULL;
		}
		else {
			// list is the unique entry
			map[hash_key] = NULL;
		}
	}
	else{
		//fprintf(stderr, "c");

		if(list->next != NULL){
			
			//fprintf(stderr, "d");

			if(list->prev == NULL){
				fprintf(stderr, "list->prev is null (%d, %d)\n", list->pos, map[hash_key]->pos);
				exit(0);
			}
			
			list->prev->next = list->next;
			list->next->prev = list->prev;
		}
		else {
			list->prev->next = NULL;
		}
	}
	//list = NULL;
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
void RegistSet(int* subset, int subsetLen, TREE* tree, LIST** map, TABLE* table, int hashKey){
	
	int parent, child;
	
	// step 1 insert the subset to entry table (heap insert)
	
	// adding the new entry to the last of heap
	table->last++;
	
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
			swap(table, tree, parent, child, map); 
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
 * FUNCTION: swap
 *	+ swap the parent entry with the child entry
 * OPTIONS:
 *	+ table - pointer to the Table
 *	+ parent - parent index
 *	+ child - child index
 * RETURN:
 *	+ void
 **/
void swap(TABLE* table, TREE* tree, int parent, int child, LIST** map){
	// swap the every memembership value
	// Note that the link value indicates the pointer to the access map.
	// Its member pos value should correspond to index of this entry in the table
	// We thus need to change the pos value too.
	
	ENTRY* temp;
	table->heap[parent]->link->pos = child;
	table->heap[child]->link->pos = parent;
	
	table->heap[parent]->n_link->pos = child;
	table->heap[child]->n_link->pos = parent;
	
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
 */
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

/*
 * FUNCTION: CreateList
 *	+ creating the new LIST instance in the access map
 * OPTIONS:
 *	+ hashKey - hash key
 *	+ pos - position of the corresponding entry in the entry table
 *	+ map - pointer to the access map
 * RETURN:
 *	+ pointer to the new LIST
 */
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
 * FUNCTION: SetList
 *	+ Setting the new LIST instance in the access map
 * OPTIONS:
 *  + list - pointer to the list
 *	+ hashKey - hash key
 *	+ pos - position of the corresponding entry in the entry table
 *	+ map - pointer to the access map
 * RETURN:
 *	+ void
 **/
void SetList(LIST* newList, int hashKey, int pos, LIST** map)
{
	newList -> pos = pos;
	newList -> prev = NULL;
	newList -> next = NULL;
	LIST* temp = map[hashKey];

	//if(hashKey == 255){
		//fprintf(stderr, "before 255 list::");
		//listPrint(map[hashKey]);
	//}	
	
	// adding this to the top of linked list
	// if temp is NULL, then just add newList to map[hashKey]
	if(temp == NULL){
		map[hashKey] = newList;
	}
	else{
		// map[hashKey] ==> newList
		map[hashKey] = newList;
		// newList ==> temp
		newList -> next = temp;
		// temp ==> newList
		temp -> prev = newList;
	}
	
	//if(hashKey == 255){
	//	fprintf(stderr, "after 255 list::");
	//	listPrint(map[hashKey]);
	//}
	
}

/**
 * FUNCTION: SetEntry
 *	+ Set the entry corresponding the current subset
 * OPTIONS:
 *  + entry: pointer to the modified entry 
 *	+ subset: pointer to the subset
 *	+ subsetLen: length of the subset
 *	+ link: the pointer to the newly created LIST
 *  + t_count: true count to be registered
 *  + e_count: estimated count to be registered
 * RETURN
 *	+ void
 **/
void SetEntry(ENTRY* newEntry, int* subset, int subsetLen, LIST* link, int t_count, int e_count)
{
	int i;
	
	if(newEntry == NULL){
		fprintf(stderr, "Set entry error\n");
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
    newEntry->tmp = 0;
    newEntry->e_count = e_count;
	newEntry->t_count = t_count;
	newEntry->length = subsetLen;
	newEntry->link = link;
	
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
int Reorder(TABLE* table, TREE* tree, int id, LIST** map){
	
	int parent, child;
	
	// index focusing on the initial parent
	parent = id;
	
	// index for its left child
	child = child(parent);
	//fprintf(stderr, "1\n");
	// continue to search for the relevant position of the new entry
	while(child <= table->last){
		//fprintf(stderr, "table->last = %d, child = %d\n", table->last, child);
		// change the child index if the e_count of right child < e_count of left child
		
		//fprintf(stderr, "table->heap[%d] = %d\n", child, table->heap[child]->e_count);
		//fprintf(stderr, "2\n");

		if(child != table->last && table->heap[child+1]->e_count < table->heap[child]->e_count){
			//fprintf(stderr, "3\n");
			child++;
		}
		// swap if the e_count of child < e_count of parent
		if(table->heap[child]->e_count < table->heap[parent]->e_count){
			//fprintf(stderr, "4\n");
			swap(table, tree, parent, child, map);
		}
		else{
			//fprintf(stderr, "5\n");
			break;
		}
		// updating the parent and child indexes
		parent = child;
		child = child(parent);
	}
	return parent;
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



/** 
 * FUNCTION: entryPrint
 *	+ printing the entry
 * OPTIONS:
 *	+ entry - pointer to the entry
 * RETURNS:
 *	+ void
 **/
void entryPrint(ENTRY* entry){
	int i;
	fprintf(stderr, "+ entry: <");
	for(i = 0; i < entry->length; i++){
		fprintf(stderr, "%d. ", entry->subset[i]);
	}
	fprintf(stderr, "> est_count: %d, t_count: %d, pos %d\n", entry->e_count, entry->t_count, entry->link->pos);
}

/** 
 * FUNCTION: subsetPrint
 *	+ printing the subset
 * OPTIONS:
 *	+ entry - pointer to the entry
 * RETURNS:
 *	+ void
 **/
void subsetPrint(int* subset, int length){
	int i;
	fprintf(stderr, "< ");
	for(i = 0; i < length; i++){
		fprintf(stderr, "%d. ", subset[i]);
	}
	fprintf(stderr, ">\n");
}


// for debagging
void tablePrint(TABLE* table){
	int i;
	//printf("min: %d, n_min: %d\n", table->min, table->n_min);
	for(i = 1; i <= table->last; i++){
		entryPrint(table->heap[i]);
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


