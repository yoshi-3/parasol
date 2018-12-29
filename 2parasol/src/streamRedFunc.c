



/* Declaration of prototypes */


#include"entryTable.h" 
#include"streamReduction.h" 
#include"streamRedFunc.h"


/**
 * FUNCTION: I_UpdateTable
 * + Extracting each subset from the current transaction and register it to the entry table
 * OPTIONS:
 * + stream - the stream data
 * + table - pointer to the entry table
 * + map - pointer to the access map
 * + para - pointer to the I_PARAMETER value
 * RETURN:
 * + void
 **/
void I_UpdateTable(STREAM curStream, I_TABLE *table, MAP** map, I_PARAMETER *para){
	
	int i;
	int hashkey;
	int check_item[curStream.num];
	int item;

	I_CheckTable(curStream, table, map, para, check_item); 
	for (i = 0; i < curStream.num; i++) {
		if (check_item[i] != 1) {

			item = curStream.itemset[i];
			hashkey = I_GetHashKey(item, para->hash_size);
			
			if (table->last == para->table_size) {
				I_SwapEntry(item, table, map, para);
			}
			else {
				table->heap[table->last] = I_CreateEntry(item, table->last);
				I_CreateMap(hashkey, table->last, map);
				table->last++;
			}
		}
	}
}


/*
*/
void I_CheckTable(STREAM curStream, I_TABLE *table, MAP** map, I_PARAMETER *para, int *check_item)
{
	int i;
	int hashkey;
	I_ENTRY* entry;
	int sort_flag = 0;

	for (i = 0; i < curStream.num; i++) {

		hashkey = I_GetHashKey(curStream.itemset[i], para->hash_size);
		MAP *entryKey = map[hashkey];

		while (entryKey != NULL) {

			entry = table->heap[entryKey->pos];
			if (I_IsMatched(entry, curStream.itemset[i])) {
				entry->e_count++;
				entry->t_count++;

				check_item[i] = 1;
				sort_flag++;
				break;
			}
			else {
				entryKey = entryKey->next;
			}
		}
		if (entryKey == NULL) {
			check_item[i] = 0;
		}
	}
	
	
	if (sort_flag != 0) {
		QuickSort(table, 0, table->last-1, para, map);
	}
}

/*
*/
int I_GetHashKey(int item, int hash_size)
{
	int key = (int)item % hash_size;
	return key;

}

/**
 * FUNTION: I_IsMatched
 *	+ return 1 or 0 if the entry is matched with the current candidate set
 * OPTIONS
 *	+ entry - entry
 *	+ subset - pointer to the current candidate set
 * RETURN:
 *	* 1 or 0
 **/
int I_IsMatched(I_ENTRY* entry, int item)
{
	
	if(entry == NULL){
		return 0;
	}
	

	if (entry->item != item)
		return 0;
	else
		return 1;
}

/**/
void QuickSort(I_TABLE *table, int left, int right, I_PARAMETER *para, MAP** map)
{
	int i,j;
	I_ENTRY* pivot;

	i=left;
	j=right;
	pivot = table->heap[(left+right)/2];

	while(1){    

		while(table->heap[i]->e_count > pivot->e_count)
			i++;    
		while(pivot->e_count > table->heap[j]->e_count)
			j--;

		if(i>=j)
			break;

		I_Swap(table, i, j, para, map);
		i++;
		j--;
	}

	if(left < i-1)
		QuickSort(table, left, i-1, para, map);
	if(j+1 < right)
		QuickSort(table, j+1, right, para, map);
}


/*
*/
void I_Swap(I_TABLE *table, int i, int j, I_PARAMETER *para, MAP** map)
{
	I_ENTRY* temp;

	temp = table->heap[i];
	table->heap[i] = table->heap[j];
	table->heap[j] = temp;


	ExchangeAnchor(i, table, map, para);
	ExchangeAnchor(j, table, map, para);
}


/*
*/
void ExchangeAnchor(int index, I_TABLE *table, MAP** map, I_PARAMETER *para)
{

	int hashkey;
	MAP* node;

	hashkey = I_GetHashKey(table->heap[index]->item, para->hash_size);
	node = map[hashkey];

	while (node != NULL) {
		if (node->pos == table->heap[index]->anchor)
			break;
		node = node->next;
	}

	if (node == NULL) {
		fprintf(stderr, "error: ExchangeAnchor \n");
		exit(1);
	}

	table->heap[index]->anchor = index;
	node->pos = index;

}


/**
 * FUNCTION: I_CreateEntry
 *	+ Create the new entry corresponding the current subset
 * OPTIONS:
 *	+ subset: pointer to the subset
 *	+ subsetLen: length of the subset
 *	+ delta: the maximal error at this moment
 *	+ link: the pointer to the newly created LIST
 * RETURN
 *	+ void
 **/
I_ENTRY* I_CreateEntry(int item, int pos){
	// creating the new entry
	//int i;
	I_ENTRY* newEntry = (I_ENTRY*)malloc(sizeof(I_ENTRY));
	
	if(newEntry == NULL){
		fprintf(stderr, "cannot allocate memory to the new entry\n");
		exit(0);
	}
	
	newEntry->item = item;
	
	// setting the other member values of entry
	newEntry->e_count = 1;
	newEntry->t_count = 1;

	newEntry->anchor = pos;
	
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
void I_CreateMap(int hashKey, int pos, MAP** map){
	
	MAP* newMap = (MAP*)malloc(sizeof(MAP));
	
	if(newMap == NULL){
		fprintf(stderr, "cannot allocate memory to the new maps\n");
		exit(0);
	}
	
	newMap->pos = pos;
	newMap->next = map[hashKey];
	map[hashKey] = newMap;
}

/*
 */
void I_SwapEntry(int item, I_TABLE *table, MAP** map, I_PARAMETER *para)
{

	int delta;	
	int hashkey, newHashkey;
	MAP* node;
	MAP* preNode;

	delta = table->heap[table->last-1]->e_count;
	table->delta = table->heap[table->last-1]->e_count;

	hashkey = I_GetHashKey(table->heap[table->last-1]->item, para->hash_size);
	node = map[hashkey];

	if (node == NULL) {
		fprintf(stderr, "error: I_SwapEntry(1) \n");
		exit(1);
	}

	if (node->pos == table->last-1) {
		map[hashkey] = node->next;
	}
	else {
		while (node->pos != table->last-1) {
			preNode = node;
			node = node->next;
			if (node == NULL) {
				fprintf(stderr, "error: I_SwapEntry(2) \n");
				exit(1);
			}
		}

		preNode->next = node->next;
	}
	
	newHashkey = I_GetHashKey(item, para->hash_size);
	node->next = map[newHashkey];
	map[newHashkey] = node;

	table->heap[table->last-1]->item = item;
	table->heap[table->last-1]->e_count = 1 + delta;
	table->heap[table->last-1]->t_count = 1;

	QuickSort(table, 0, table->last-1, para, map);

}

/*
  関数名：RedeceStream
  ・部分系列subseqが頻度表tableに登録されているかマップmapで確認する
  引数：
  ・*table - I_TABLE型ポインタ 頻度表
  ・*map   - MAP型ポインタ アクセスマップ
  ・*para  - I_PARAMETER型ポインタ 各種パラメータを持つ構造体
  戻り値：
  ・なし
*/
void ReduceStream(STREAM* curStream, I_TABLE *table, MAP** map, int hash_size, int threshold)
{
	int i, j;
	int hashkey;
	I_ENTRY* entry;

	int check_item[curStream->num];
	int temp_item[curStream->num];
	for(i = 0; i < curStream->num; i++){
		temp_item[i] = curStream->itemset[i];
	}
	int num = 0;

	//アイテムの頻度を確認
	for (i = 0; i < curStream->num; i++) {

		hashkey = I_GetHashKey(curStream->itemset[i], hash_size);
		MAP *entryKey = map[hashkey];

		while (entryKey != NULL) {

			entry = table->heap[entryKey->pos];
			if (I_IsMatched(entry, curStream->itemset[i])) {
				if (entry->e_count >= threshold) {
					check_item[i] = 1;
					num++;
				}
				else {
					//fprintf(stderr, "e_count = %d, delta = %d\n", entry->e_count, threshold);
					check_item[i] = 0;
				}
				break;
			}
			else {
				entryKey = entryKey->next;
			}
		}
		if (entryKey == NULL) {
			check_item[i] = 0;
		}
	}

	for (i = 0; i < curStream->num; i++) {
		curStream->itemset[i] = -1;
	}

	j = 0;
	for (i = 0; i < curStream->num; i++) {
		if (check_item[i] == 1) {
			curStream->itemset[j] = temp_item[i];
			j++;
		}
	}

	if (j != num) {
		fprintf(stderr, "error: redStream \n");
		fprintf(stderr, "j = %d, cur.num = %d, red.num = %d\n", j, curStream->num, num);
		exit(1);
	}
	// updating
	curStream->num = num;

}

/*
 FUNCTION: I_MakeTable
 +function for allocating the memory of the entry table
 OPTIONS:
 +*para - pointer to the I_PARAMETER value
 RETURN:
 +table - pointer to the I_TABLE value
 */
I_TABLE *I_MakeTable(I_PARAMETER *para)
{
	//int i, j;
	I_TABLE *entryTable = (I_TABLE *)malloc(sizeof(I_TABLE));
	
	fprintf(stderr,"I_MakeTable\t");
	
	// memory allocation of entry table 
	entryTable->heap = (I_ENTRY**)malloc(sizeof(I_ENTRY*) * (para->table_size)); 
	if(entryTable->heap == NULL){
		fprintf(stderr,"I_TABLE: malloc failed...\n");
		exit(1);
	}
	
	entryTable->delta = 0;
	entryTable->last = 0;
	
	
	fprintf(stderr,"OK!\n");
	
	return entryTable;
}

/*
 FUNCTION: I_InitMap
 +function for initializing the access map
 OPTIONS:
 +*map - pointer to the LIST value
 RETURN:
 +void
 */
void I_InitMap(MAP** map)
{
	int i;
	//int flag = 0;
	
	fprintf(stderr, "I_InitMap\t\t");
	
	/*Initialization*/
	for(i = 0; i < HASH_SIZE; i++){
		map[i] = NULL;

	}

	fprintf(stderr,"OK!\n");
	
}


/**
 * FUNCTION: I_FreeMap
 *	+ make the access map free
 * OPTIONS: 
 *	+ map - pointer to MAP
 *	+ hash_size - hash size
 * RETURN:
 *	+ void
 **/
void I_FreeMap(MAP** map, int hash_size)
{
	int i;
	fprintf(stderr,"I_FreeMap    \t");
	
	for(i = 0; i < hash_size; i++){
		while(map[i] != NULL){
			MAP* m = map[i];
			map[i] = map[i]->next;
			free(m);
		}
	}
	fprintf(stderr,"OK!\n");
}



/**
 * FUNCTION: I_FreeTable
 *	+ opening the entry table
 * OPTIONS
 *	+ table - pointer to I_TABLE
 *	+ para - pointer to I_PARAMETER
 * RETURN
 *	+ void
 **/
void I_FreeTable(I_TABLE *table, I_PARAMETER *para)
{
	fprintf(stderr,"I_FreeTable\t");
	
	int i;
	for(i = 0; i < table->last; i++){
		free(table->heap[i]);
	}
	
	free(table);
	free(para);
	
	fprintf(stderr,"OK!\n");
}
