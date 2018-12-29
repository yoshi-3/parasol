/*
 *  rTypeMining.h
 *  
 *
 *
 */

#ifndef RTYPEMINING_H
#define RTYPEMINING_H

#include"entryTable.h"
#include"tableFunc.h"
#include"streamReduction.h"
#include"streamRedFunc.h"
#include"treeFunc.h"
#include"stream.h"
#include"getline.h"

/* resource oriented mining */
void MiningStream(TREE* tree, TABLE *table, LIST** map, FILE* stream, PARAMETER *para, I_TABLE *i_table, MAP** I_MAP, I_PARAMETER *i_para);
void UpdateTable(TREE* tree, STREAM curStream, TABLE *table, LIST** map, PARAMETER *para);

#endif

