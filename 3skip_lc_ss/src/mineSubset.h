/*
 mineSubset.c
 
 Input: streams data (*.seq)
 Output: frequent itemsets
 
 Embedding the SKIP operation into the baseline implementation
 
 For the detail on the usage, please refer users to the option -h
  
 */

#ifndef MINESUBSET_H
#define MINESUBSET_H

#include"headerSubset.h" // Definition of data structures and constant symbols
#include"getline.h" // Definition of data structures and constant symbols


/*Declaration of prototypes*/

void GetCurTrans(char* stream, STREAM* trans, int offset, int* itemKind, PARAMETER* para);
int comp( const void *c1, const void *c2 );

TABLE *MakeTable(PARAMETER *para);
void InitMap(LIST** map, int hash_size);
STREAM *GetStream(PARAMETER *para);
void ShowParameter(PARAMETER *para, TABLE* table);//outputing the vaious kinds of parameters
void ShowStream(STREAM *stream, PARAMETER *para);//for debagging
void MiningStream(TABLE *table, LIST** map, FILE* stream, PARAMETER *para);
void UpdateTable(STREAM curStream, TABLE *table, LIST** map, PARAMETER *para);
ENTRY* GetRegistSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hashKey);
int GetHashKey(int* subset, int length, int hash_size);
int IsMatched(ENTRY* entry, int* subset, int subsetLen);
int GetSubset(int* subset, int id, STREAM stream);
int GetSubsetFromArray(int* subset, int* id, STREAM stream, int* order);
void RegistSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hashKey);
ENTRY* CreateEntry(int* subset, int subsetLen, int delta, LIST* link);
LIST* CreateList(int hashKey, int pos, LIST** map);
void Swap(TABLE* table, int parent, int child, LIST** map);
int ReplaceSet(int* subset, int subsetLen, LIST** map, TABLE* table, int hash_size);
void ReplaceByTableScan(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* order, int zeroNum, int num_replace_set);
void ReplaceByTransEnum(STREAM stream, TABLE* table, LIST** map, PARAMETER* para, int* n_reg_array, int num_replace_set);
void UpdateEntry(int* subset, int subsetLen, int delta, LIST** map, ENTRY* entry, int hash_size);
ENTRY* GetMinSetTable(TABLE *table, LIST** map);
void OutputTable(TABLE *table, PARAMETER *para);
void OutputResult(LIST** map, TABLE *table, PARAMETER *para);
void OutputMap(LIST** map, TABLE *table, PARAMETER *para);
void FreeMap(LIST** map, int hash_size);
void FreeStream(STREAM *stream, int stream_len, int phase);
void FreeTable(TABLE *table, PARAMETER *para);
void EntryPrint(ENTRY* entry);
void SubsetPrint(int* subset, int length);
void UpdateByTableScan(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* order);
void UpdateByTransEnum(STREAM stream, TABLE *table, LIST** map, PARAMETER *para, int* n_reg_array);
int IsIncluded(ENTRY* entry, STREAM stream, int* order);
int isIncluded(int item, STREAM* stream, int start, int end, int* order);
int Reorder(TABLE* table, int id, LIST** map);
void UpdateMinEntries(TABLE* table, int j);
int updateMinEntries(TABLE* table, int id);
int updateRSkip(TABLE* table, int id);
void increment(TABLE* table);
int reordering(int* order, int* reorder, int num);
void q_sort(PAIR* list, int flag, int start, int end);
void pairPrint(PAIR* pairs, int legth);
void tablePrint(TABLE* table);
void listPrint(LIST* key);
void SetBit(int* array, int size, int bit);
int EqualBitZero(int* index, int size);
void UpdateBit(int* array, int size);

#endif


