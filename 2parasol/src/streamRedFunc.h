/*
 *  streamRedFunc.h
 *  
 *
 *
 */


#ifndef STREAMREDFUNC_H
#define STREAMREDFUNC_H

#include"streamReduction.h" 


I_TABLE* I_MakeTable(I_PARAMETER *para);
void I_InitMap(MAP** map);
void I_UpdateTable(STREAM curStream, I_TABLE *table, MAP** map, I_PARAMETER *para);
void I_CheckTable(STREAM curStream, I_TABLE *table, MAP** map, I_PARAMETER *para, int *check_item);
int I_GetHashKey(int item, int hash_size);
int I_IsMatched(I_ENTRY* entry, int item);
I_ENTRY* I_CreateEntry(int item, int pos);
void I_CreateMap(int hashKey, int pos, MAP** map);
void QuickSort(I_TABLE *table, int left, int right, I_PARAMETER *para, MAP** map);
void I_Swap(I_TABLE *table, int i, int j, I_PARAMETER *para, MAP** map);
void ExchangeAnchor(int index, I_TABLE *table, MAP** map, I_PARAMETER *para);
void I_SwapEntry(int item, I_TABLE *table, MAP** map, I_PARAMETER *para);
void ReduceStream(STREAM* curStream, I_TABLE *table, MAP** map, int hash_size, int threshold);

void I_FreeMap(MAP** map, int hash_size);
void I_FreeTable(I_TABLE *table, I_PARAMETER *para);


#endif
