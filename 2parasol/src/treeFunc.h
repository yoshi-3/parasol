/*
 *  treeFunc.h
 *  
 *
 *  Created by yamamoto on 15/02/08.
 *  Copyright 2015 Univ. of Yamanshi All rights reserved.
 *
 */

#ifndef TREEFUNC
#define TREEFUNC

#include"entryTable.h"
#include"tableFunc.h"

TREE* SetTree(int table_max);
void FreeTree(TREE *tree, PARAMETER *para);
ELEMENT* MakeElement();
ELEMENT* GetElement(TREE *tree);

void RegistNode(NODE* target, NODE* parent, TREE* tree);
void RemoveSibParNode(NODE* target);

void Remove(TREE* tree, TABLE* table, LIST** map, PARAMETER* para);
void Compress(TREE* tree, TABLE* table, LIST** map, int type);
int has_d_covered(NODE* cur, TREE* tree, TABLE* table, LIST** map, int type);
int check_mergeable(int cur, int target, TABLE* table, TREE* tree, LIST** map);

NODE* getLeftMost(NODE* node);
int hasNextLeaf(NODE* node);
NODE* nextLeaf(NODE* node);

int RemoveElement(TREE* tree, int index, TABLE* table, LIST** map);

void ResetNode(NODE* target);

void Increment(NODE* target, TREE* tree, TABLE* table, LIST** map);
int EqualNode(NODE* a, NODE* b);

void UpdateNodeApp(NODE* start, int* i_set, int i_setLen, TREE* tree, TABLE* table, LIST** map, PARAMETER* para);

void UpdateNode(NODE* start, int* subset, int subsetLen, TREE* tree, TABLE* table, LIST** map);
void UpdateRegistNode(NODE* start, NODE* end, TREE* tree, TABLE* table, LIST** map, PARAMETER* para);
void UpdateNewNode(NODE* start, NODE* end, TREE* tree, TABLE* table, LIST** map, PARAMETER* para);

NODE* GetRepEntry(int* itemset, int num, TREE* tree, TABLE* table);
NODE* GetRepEntryApp(int* itemset, int num, TREE* tree, TABLE* table);

int Includes(ENTRY* entry, int* itemset, int num);
int isRClassOf(NODE* start, int* itemset, int num, TREE* tree, TABLE* table, LIST** map);
int rClassCheck_byHash(NODE* start, int* itemset, int num, TREE* tree, TABLE* table, LIST** map);
int rClassCheck_byadhoc(NODE* start, int* itemset, int num, TREE* tree, TABLE* table);

void SetElement(ELEMENT* e, int* subset, int subsetLen, NODE* parent, TREE*, TABLE* table, LIST** map, int t_count, int e_count);

void Intersection(int* subset, int* num, ENTRY* a, ENTRY* b);
void nodePrint(NODE* node, TABLE* table);
void allChildPrint(NODE* node, TABLE* table, int depth);


int debag1(STREAM stream);
void debag2(NODE* root, TABLE* table);
int debag3(int* subset, int length);
void debag4(TABLE* table, LIST** map, PARAMETER* para);
int debag5(ENTRY* entry);

#endif
