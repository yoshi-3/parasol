/*
 *  tableFunc.h
 *  
 *
 *
 */

#ifndef TABLEFUNC_H
#define TABLEFUNC_H

#include"entryTable.h" 

// functions which are used for accessing entry table and hash map

/* Main routine */
void UpdateTable(TREE* tree, STREAM stream, TABLE* table, LIST** map, PARAMETER* para);

/* initialization */
TABLE* MakeTable(int table_max);
LIST** InitMap();

/* make the memory space free*/
void FreeMap(LIST** map);
void FreeTable(TABLE *table, PARAMETER *para);

/* writing the outputs */
void OutputTable(TABLE *table, PARAMETER *para);
void OutputMap(LIST** map, TABLE *table, PARAMETER *para);
void OutputResult(LIST** map, TABLE *table, TREE* tree, PARAMETER *para);

/* searching for some entry with the hash table */
ENTRY* GetSetTable(int index, TABLE *table, TREE* tree, LIST** map);
int GetHashKey(int* subset, int length, int hash_size);
ENTRY* GetRegistSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hashKey);

/* registring some new entry */

void RegistSet(int* subset, int subsetLen, TREE* tree, LIST** map, TABLE* table, int hashKey);
ENTRY* CreateEntry(int* subset, int subsetLen, int delta, LIST* link);
LIST* CreateList(int hashKey, int pos, LIST** map);

void RegistEntry(ENTRY* entry, TREE* tree, LIST** map, TABLE* table);
void SetList(LIST* newList, int hashKey, int pos, LIST** map);
void SetEntry(ENTRY* newEntry, int* subset, int subsetLen, LIST* link, int t_count, int e_count);

void ResetList(LIST* list, LIST** map, int hash_key);


/* processing some registered entry */
int Reorder(TABLE* table, TREE* tree, int id, LIST** map);

/* the base operations checking the correspondence and inclusion */
int IsMatched(ENTRY* entry, int* subset, int subsetLen);
int IsIncluded(ENTRY* entry, STREAM stream, int* order);

/* function in treeFunc */
void SwapTailTree(TREE* tree, int parent, int child);


/* for debagging */
void entryPrint(ENTRY* entry);
void subsetPrint(int* subset, int length);
void tablePrint(TABLE* table);
void listPrint(LIST* key);





/* we do not use the following functions: 
 
int ReplaceSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hash_size); 
void ReplaceByTableScan(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* order, int zeroNum, int num_replace_set);
void ReplaceByTransEnum(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* n_reg_array, int num_replace_set);
void UpdateEntry(int* subset, int subsetLen, int delta, LIST** map, ENTRY* entry, int hash_size);
GetSubset(int* subset, int id, STREAM stream);
int GetSubsetFromArray(int* subset, int* id, STREAM stream, int* order);
 void UpdateByTableScan(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* order);
 void UpdateByTransEnum(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* n_reg_array);
 void UpdateMinEntries(TABLE* table, int j);
 int updateMinEntries(TABLE* table, int id);
 void increment(TABLE* table);
 int updateRSkip(TABLE* table, int id);
 int reordering(int* order, int* reorder, int num);
 void SetBit(int* array, int size, int bit);
 int EqualBitZero(int* index, int size);
 void UpdateBit(int* array, int size);
 void pairPrint(PAIR* pairs, int legth);

*/


#endif
