/*
 *  treeFunc.c
 *  
 *
 *  Created by yamamoto on 15/02/08.
 *  Modified by yamamoto on 15/04/02; 15/04/08
 *  Copyright 2015 Univ. of Yamanashi All rights reserved.
 *
 */
#include "entryTable.h"
#include "treeFunc.h"

int doRemove(TABLE* table, PARAMETER* para);
void intersection(int* subset, int* length, int* a_set, int a_length, int* b_set, int b_length);

//int count = 0;


/**
 * Function: SetTree
 *	+function for allocatingg the memory of the tree consisting
 *	 of table_max+1 elements
 * OPTIONS:
 *	+*para - pointer to the PARAMETER value
 * RETURN:
 *	+tree - pointer to the TREE value
 **/
TREE* SetTree(int table_max){
	
	int i;
	
	fprintf(stderr,"SetTree\t\t");
	
	/*memory allocation of tree*/
	TREE* tree = (TREE*)malloc(sizeof(TREE));
	if(tree == NULL){
		fprintf(stderr,"tree: malloc failed...\n");
		exit(1);
	}
	/*memory allocation of stack table*/
	tree->stack = (ELEMENT**)malloc(sizeof(ELEMENT*) * (table_max+1));
	if(tree->stack == NULL){
		fprintf(stderr,"tree->stack: malloc failed...\n");
		exit(1);
	}
	/*memory allocation of root node*/
	tree->root = (NODE*)malloc(sizeof(NODE));
	if(tree->root == NULL){
		fprintf(stderr,"tree->root: malloc failed...\n");
		exit(1);
	}
	
	//setting the root node
	tree->root->pos = 0; // this is root node
	tree->root->hasRsibling = 0;
	tree->root->hasLsibling = 0;
	tree->root->hasChild = 0;
	
	/*creating elements and allocating them into stack*/
	tree->cur_index = table_max;
	
	for(i = 0; i <= table_max; i++){
		tree->stack[i] = MakeElement();
	}
	fprintf(stderr,"OK!\n");
	
	return tree;
}

/**
 * FUNCTION: MakeElement
 * + Create the new element
 **/
ELEMENT* MakeElement(){
	ELEMENT* newElement = (ELEMENT*)malloc(sizeof(ELEMENT));
	if(newElement == NULL){
		fprintf(stderr, "cannot allocate memory to the new element\n");
		exit(1);
	}
	LIST* newList = (LIST*)malloc(sizeof(LIST));
	if(newList == NULL){
		fprintf(stderr, "cannot allocate memory to the new lists\n");
		exit(1);
	}
	ENTRY* newEntry = (ENTRY*)malloc(sizeof(ENTRY));
	if(newEntry == NULL){
		fprintf(stderr, "cannot allocate memory to the new entry\n");
		exit(1);
	}
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if(newNode == NULL){
		fprintf(stderr, "cannot allocate memory to the new node\n");
		exit(1);
	}
	newElement->entry = newEntry;
	newElement->list = newList;
	newElement->node = newNode;
	return newElement;
}


/**
 * FUNCTION: UpdateNode
 * + update a target node with a target entry 
 * + recursive funtion
 **/
void UpdateNodeApp(NODE* start, int* i_set, int i_setLen, TREE* tree, TABLE* table, LIST** map, PARAMETER* para)
{
	
	int num, t_count, e_count, hash_key;
	//int regist_flag = 1; // detecting if the i_set should be registeded or not (if 1 then should be registered)
	int subset[SUBSEQ_LEN]; //intersection
	//fprintf(stderr, "start: ");
	
	// updating the true and estimated counts
	if(start->pos == 0){
		t_count = 0;
		e_count = table->delta;
		//fprintf(stderr, "root: \n");
		
	}
	else {
		t_count = table->heap[start->pos]->t_count;
		e_count = table->heap[start->pos]->e_count;
		//entryPrint(table->heap[start->pos]);
		
	}
	//fprintf(stderr, "itemset: ");
	//subsetPrint(i_set, i_setLen);		
	
	if(!start->hasChild){
		
		hash_key = GetHashKey(i_set, i_setLen, HASH_SIZE);
		ENTRY* corresEntry = GetRegistSet(i_set, i_setLen, map, table, hash_key);
		
		// this intersection has been registered
		if(corresEntry == NULL){
			
			// step 0. Regist i_set to this node
			ELEMENT* e = GetElement(tree);
			//fprintf(stderr, "a");
			
			//fprintf(stderr, "b");
			SetElement(e, i_set, i_setLen, start, tree, table, map, t_count+1, e_count+1);
		}
		else{
			if(corresEntry->e_count < e_count+1){
				//we update the e_count of corresEntry
				corresEntry->e_count = e_count+1;
			}	
			if(corresEntry->t_count < t_count+1){
				//we update the t_count of corresEntry
				corresEntry->t_count = t_count+1;
			}
			
		}
		//fprintf(stderr, "c");
		return;
	}
	else {
		//fprintf(stderr, "step 1\n ");
		
		NODE* cur = start->lchild; // current node to be checked
		while(1){
			//fprintf(stderr, "cur: ");
			//entryPrint(table->heap[cur->pos]);
			//subsetPrint(i_set, i_setLen);
			
			// step 1. compute the intersection of node's entry and target entry
			intersection(subset, &num, table->heap[cur->pos]->subset, table->heap[cur->pos]->length, i_set, i_setLen);
			
			//fprintf(stderr, "intersection: ");
			//subsetPrint(subset, num);
			
			//if(debag3(subset, num)){
			//entryPrint(table->heap[cur->pos]);
			//}
			
			// step 2. continue the update process in accordance with the result of subset
			if(num != 0){
				//fprintf(stderr, "step 2 ");
				
				// step 2-1 cur is included in i_set			
				if(num == table->heap[cur->pos]->length){
					//fprintf(stderr, "-1 ");
					
					// step 2-1-1 inclement and then move to the right sibling
					Increment(cur, tree, table, map);
					//fprintf(stderr, "-1\n ");
					
					// step 2-1-2 subset is equal to i_set 
					//if(num == i_setLen){
					//	fprintf(stderr, "-2\n ");
					// we do not need to newly regist i_set
					//	regist_flag = 0;
					// breaking the while loop and we do 
					//	break;
					//}
					// step 2-1-3 move to rsibling
					if(cur->hasRsibling){
						//fprintf(stderr, "-3\n ");
						cur = cur->rsibling;
					}
					// step 2-1-4 breaking the while loop
					else {
						//fprintf(stderr, "-4\n ");
						break;
					}
				}				
				// step 2-2 cur and i_set has an intersection that is neither empty set nor cur 
				else{
					//fprintf(stderr, "-2 ");
					
					// step 2-2-1 checking if there is a representative class before node node
					//if(isRClassOf(cur, subset, num, tree, table, map, para)){
					//fprintf(stderr, "-1 ");
					
					// Recall UpdateNode function (in this routine, subset has been registered)
					UpdateNodeApp(cur, subset, num, tree, table, map, para);
					
					// i_set is included in cur 
					if(num == i_setLen){
					//fprintf(stderr, "-2\n ");
					// we do not need to newly regist i_set
					//	regist_flag = 0;
					// breaking the while loop and we do 
						break;
					}
					//fprintf(stderr, "-2\n ");
					
					//}
					// step 2-2-2 move to rsibling
					if(cur->hasRsibling){
						//fprintf(stderr, "-2\n ");
						cur = cur->rsibling;
					}
					// step 2-2-3 breaking the while loop
					else {
						//fprintf(stderr, "-3\n ");
						break;
					}
				}
			}			
			// step 3. move to rsibling
			else {
				//fprintf(stderr, "step 3\n ");
				// step 6.b move to rsibling
				if(cur->hasRsibling){
					cur = cur->rsibling;
				}
				// step 7.a breaking the while loop
				else {
					break;
				}
			}
			
		}
		// finally we regist i_set if necessary
		
		hash_key = GetHashKey(i_set, i_setLen, HASH_SIZE);
		ENTRY* corresEntry = GetRegistSet(i_set, i_setLen, map, table, hash_key);
		
		// this intersection has been registered
		if(corresEntry == NULL){
			//fprintf(stderr, "regist_flag on\n ");
			//fprintf(stderr, "regist i_set\n");
			// regist the new element into the right most child of the left child
			ELEMENT* e = GetElement(tree);
			SetElement(e, i_set, i_setLen, start, tree, table, map,  t_count+1, e_count+1);
		}
		else{
			if(corresEntry->e_count < e_count+1){
				//we update the e_count of corresEntry
				corresEntry->e_count = e_count+1;
			}	
			if(corresEntry->t_count < t_count+1){
				//we update the t_count of corresEntry
				corresEntry->t_count = t_count+1;
			}
			
		}
		return;
	}
}


/**
 * FUNCTION: UpdateNode (Representative pattern)
 * + update a target node with a target entry 
 * + recursive funtion
 **/
void UpdateNode(NODE* start, int* i_set, int i_setLen, TREE* tree, TABLE* table, LIST** map)
{

	int num, t_count, e_count, hash_key;
	int regist_flag = 1; // detecting if the i_set should be registeded or not (if 1 then should be registered)
	int subset[SUBSEQ_LEN]; //intersection
	
	// updating the true and estimated counts
	if(start->pos == 0){
		t_count = 1;
		e_count = 1 + table->delta;
	}
	else {
		t_count = table->heap[start->pos]->t_count + 1;
		e_count = table->heap[start->pos]->e_count + 1;
	}
	
	if(!start->hasChild){
		
		hash_key = GetHashKey(i_set, i_setLen, HASH_SIZE);
		ENTRY* corresEntry = GetRegistSet(i_set, i_setLen, map, table, hash_key);
		
		// this intersection has been registered
		if(corresEntry != NULL){
			// assertion
			if(start->pos != 0){
				fprintf(stderr, "start: ");
				entryPrint(table->heap[start->pos]);
			}
			fprintf(stderr, "i_set: ");
			subsetPrint(i_set, i_setLen);
			allChildPrint(tree->root, table, 0);	
			exit(0);
		}
		else {
			// step 0. Regist i_set to this node
			ELEMENT* e = GetElement(tree);
            SetElement(e, i_set, i_setLen, start, tree, table, map, t_count, e_count);
		}
		return;
	}
	else {
        NODE* cur = start->lchild; // current node to be checked
		while(1){
			// step 1. compute the intersection of node's entry and target entry
			intersection(subset, &num, table->heap[cur->pos]->subset, table->heap[cur->pos]->length, i_set, i_setLen);
            
			// step 2. continue the update process in accordance with the result of subset
			if(num != 0){
				// step 2-1 cur is included in i_set
				if(num == table->heap[cur->pos]->length){
					// step 2-1-1 inclement and then move to the right sibling
					Increment(cur, tree, table, map);
					// step 2-1-2 subset is equal to i_set
					if(num == i_setLen){
						// we do not need to newly regist i_set
						regist_flag = 0;
						// breaking the while loop and we do 
						break;
					}
					// step 2-1-3 move to rsibling
					else if(cur->hasRsibling){
						cur = cur->rsibling;
					}
					// step 2-1-4 breaking the while loop
					else {
						break;
					}
				}				
				// step 2-2 cur and i_set has an intersection that is neither empty set nor cur 
				else{
					// step 2-2-1 checking if there is a representative class before node
					if(isRClassOf(cur, subset, num, tree, table, map)){
						// Recall UpdateNode function (in this routine, subset has been registered)
						UpdateNode(cur, subset, num, tree, table, map);
						
						// i_set is included in cur 
						if(num == i_setLen){
							// we do not need to newly regist i_set
							regist_flag = 0;
							// breaking the while loop and we do 
							break;
						}
					}
					// step 2-2-2 move to rsibling
					if(cur->hasRsibling){
						cur = cur->rsibling;
					}
					// step 2-2-3 breaking the while loop
					else {
						break;
					}
				}
			}			
			// step 3. move to rsibling
			else {
				// step 6.b move to rsibling
				if(cur->hasRsibling){
					cur = cur->rsibling;
				}
				// step 7.a breaking the while loop
				else {
					break;
				}
			}
			
		}
		// finally we regist i_set if necessary
		if(regist_flag){
			
			hash_key = GetHashKey(i_set, i_setLen, HASH_SIZE);
			ENTRY* corresEntry = GetRegistSet(i_set, i_setLen, map, table, hash_key);
			
			// this intersection has been registered
            // assertion
			if(corresEntry != NULL){
				fprintf(stderr, "regist_flag on\n ");
				
				if(start->pos != 0){
					fprintf(stderr, "start: ");
					entryPrint(table->heap[start->pos]);
				}
				//fprintf(stderr, "i_set: ");
				subsetPrint(i_set, i_setLen);
				allChildPrint(tree->root, table, 0);	
				exit(0);
			}
			
			// regist the new element into the right most child of the left child
            ELEMENT* e = GetElement(tree);
            SetElement(e, i_set, i_setLen, start, tree, table, map,  t_count, e_count);
		}
		return;
	}
}

int isRClassOf(NODE* start, int* itemset, int num, TREE* tree, TABLE* table, LIST** map)
{
	return rClassCheck_byHash(start, itemset, num, tree, table, map);
}

int rClassCheck_byHash(NODE* start, int* itemset, int num, TREE* tree, TABLE* table, LIST** map)
{
	int hash_key;
	// step 1. check if itemset has been registered or not
	hash_key = GetHashKey(itemset, num, HASH_SIZE);
	ENTRY* corresEntry = GetRegistSet(itemset, num, map, table, hash_key);
	
	// this intersection has been registered
	if(corresEntry == NULL){
		return 1;
	}
	else{
		// check if itemset has been registered in the start's descendances
		if(start->hasChild){
			int ret = 0;
			NODE* cur = start->lchild;
			while(1){
				if(EqualNode(cur, corresEntry->n_link)){
					// itemset has been registered in the start's descendance
					ret = 1;
					break;
				}
				else if(Includes(table->heap[cur->pos], itemset, num)){
					if(cur->hasChild){
						cur = cur->lchild;
					}
					else {
						// There may be no chance to reach here...
						break;
					}
				}
				else{
					if(cur->hasRsibling){
						cur = cur->rsibling;
					}
					else{
						break;
					}
				}
			}
			return ret;
		}
		else {
			return 0;
		}
	}
}


int rClassCheck_byadhoc(NODE* start, int* itemset, int num, TREE* tree, TABLE* table)
{
	NODE* cur = start;
	int ret = 1;	
	while(1){
		int flag = 0;
		if(cur->hasLsibling){
			cur = cur->lsibling;
		}
		else {
			// there is no left sibling
			while(cur->parent->pos != 0){
				// move to parent
				cur = cur -> parent;
				if(cur -> hasLsibling){
					// move to left sibling
					cur = cur -> lsibling;
					flag = 1;
					break;
				}
			}
			if(!flag){
				// terminate the while loop
				break;
			}
		}
		// move to left sibling
		if(Includes(table->heap[cur->pos], itemset, num)){
			ret = 0;
			break;
		}
	}
	return ret;
}


NODE* GetRepEntry(int* itemset, int num, TREE* tree, TABLE* table)
{
	// search the tree for the representative pattern 
	if(tree->root->hasChild == 0){
		return NULL;
	}
	else{
		NODE* cur = tree->root->lchild;
		NODE* tmp = NULL;
		while(1){
			//count++;
			// compute the intersection of itemset with cur node
			if(Includes(table->heap[cur->pos], itemset, num)){
				tmp = cur;
				if(cur->hasChild == 1){
					cur = cur->lchild;
				}
				else {
					// the cur node is the representative pattern
					break;
				}
			}
			else {
				// the cur node does not include the itemset
				if( cur->hasRsibling == 1 ){
					cur = cur -> rsibling;
				}
				else {
					// the cur node is the representative pattern
					break;
				}
			}
		}
		return tmp;
	}
}

NODE* GetRepEntryApp(int* itemset, int num, TREE* tree, TABLE* table)
{
	int i;
	int cur = -1;
	int cur_e_count = 0;
	for(i = 1; i <= table->last; i++){
		if(Includes(table->heap[i], itemset, num)){
			if(table->heap[i]->e_count > cur_e_count){
				cur = i;
				cur_e_count = table->heap[i]->e_count;
			}
		}
	}
	if(cur == -1){
		return NULL;
	}
	else {
		return table->heap[cur]->n_link;
	}
}


int Includes(ENTRY* entry, int* itemset, int num)
{
	
	if(entry->length < num){
		return 0;
	}
	else {
		int i, j;
		i = j = 0;
		while( i < num && j < entry->length ){
			if( itemset[i] == entry->subset[j] ){
				i++;
				j++;
			}
			else if( itemset[i] > entry->subset[j] ){
				j++;
			}
			else {
				// itemset is no longer included in subset
				break;
			}
		}
		if( i == num ){
			// itemset is included in subset
			return 1;
		}
		else {
			// itemset is not included in subset
			return 0;
		}
	}
}

/**
 * FUNCTION: UpdateNewNode
 * + update a target node with a target entry 
 * + recursive funtion
 **/
void UpdateNewNode(NODE* start, NODE* end, TREE* tree, TABLE* table, LIST** map, PARAMETER* para)
{
	
	int num, hash_key;
	int subset[SUBSEQ_LEN]; //intersection
	NODE* cur = start; // current node to be checked
	
	while(1){
		
		if(EqualNode(cur, end)){
			Increment(cur, tree, table, map);
			break;
		}
		
		// step 1. compute the intersection of node's entry and target entry
		Intersection(subset, &num, table->heap[cur->pos], table->heap[end->pos]);
		
		// step 2. continue the update process in accordance with the result of subset
		if(num != 0){
			
			// cur is included in end			
			if(num == table->heap[cur->pos]->length){
				
				// step 2a. inclement and then move to the right sibling
				Increment(cur, tree, table, map);
				
				// updating the cur node
				if(cur->hasRsibling){
					
					cur = cur->rsibling;
				}
				else {
					// there is no right sibling
					break;
				}
			}
			else{
				
				hash_key = GetHashKey(subset, num, HASH_SIZE);
				ENTRY* corresEntry = GetRegistSet(subset, num, map, table, hash_key);
				
				// this intersection has been registered
				if(corresEntry != NULL){
					
					// step 3a. exchanging the position of the right node to the right most child of left 
					if(EqualNode(end, corresEntry->n_link)){
						// cur includes end node
						RemoveSibParNode(end);
						if(table->heap[end->pos]->e_count < table->heap[cur->pos]->e_count){
							// if the estimated count of right is less than the one of left, 
							// we update the right's count with the left's count					
							table->heap[end->pos]->e_count = table->heap[cur->pos]->e_count;
							table->heap[end->pos]->t_count = table->heap[cur->pos]->t_count;
							Reorder(table, tree, cur->pos, map);
						}					
						RegistNode(end, cur, tree);
						
						// we then move to the left child of cur node for its update process
						if(cur->hasChild){
							UpdateNewNode(cur->lchild, cur->rchild, tree, table, map, para);
						}
						// NOTE: we do not need to search the right sibliing of cur node
						break;
					}
					// step 4a moving to left child with the corresEntry
					else{
						if(cur->hasChild){
							UpdateRegistNode(cur->lchild, corresEntry->n_link, tree, table, map, para);
						}
						
						// updating the cur node
						if(cur->hasRsibling){
							cur = cur->rsibling;
						}
						else{
							break;
						}
						
					}
				}
				else{
					//regist the new element into the right most child of the left child
					ELEMENT* e = GetElement(tree);
					SetElement(e, subset, num, cur, tree, table, map,  table->heap[cur->pos]->t_count, table->heap[cur->pos]->e_count);
					
					if(cur->hasChild){
						UpdateNewNode(cur->lchild, cur->rchild, tree, table, map, para);
					}
					
					// updating the cur node
					if(cur->hasRsibling){
						cur = cur->rsibling;
					}
					else{
						break;
					}
				}
				
			}
		}
		else{
			// updating the cur node
			if(cur->hasRsibling){
				cur = cur->rsibling;
			}
			else{
				break;
			}
		}
		
	}
	return;
}


/*
 * FUNCTION: UpdateRegistNode
 * + update a target node with a target entry 
 * + recursive funtion
 */
void UpdateRegistNode(NODE* start, NODE* end, TREE* tree, TABLE* table, LIST** map, PARAMETER* para)
{
	
	int num, hash_key;
	int subset[SUBSEQ_LEN]; //intersection
	NODE* cur = start; // current node to be checked
	
	while(1){
		
		if(EqualNode(cur, end)){
			Increment(cur, tree, table, map);
			break;
		}
		
		// step 1. compute the intersection of node's entry and target entry
		Intersection(subset, &num, table->heap[cur->pos], table->heap[end->pos]);
		
		// step 2. continue the update process in accordance with the result of subset
		if(num != 0){
			
			// cur is included in end			
			if(num == table->heap[cur->pos]->length){
				
				// step 2a. inclement and then move to the right sibling
				Increment(cur, tree, table, map);
				
				// updating the cur node
				if(cur->hasRsibling){					
					cur = cur->rsibling;
				}
				else {
					// there is no right sibling
					break;
				}
			}
			else{
				
				hash_key = GetHashKey(subset, num, HASH_SIZE);
				ENTRY* corresEntry = GetRegistSet(subset, num, map, table, hash_key);
				
				// this intersection has been registered
				if(corresEntry != NULL){
					if(cur->hasChild){
						UpdateRegistNode(cur->lchild, corresEntry->n_link, tree, table, map, para);
					}
					// updating the cur node
					if(cur->hasRsibling){
						cur = cur->rsibling;
					}
					else{
						break;
					}
				}
				else{
					// something strange to be checked
					fprintf(stderr, "something strange to be checked\n");
					subsetPrint(subset, num);
					fprintf(stderr, "cur: \n");
					entryPrint(table->heap[cur->pos]);
					fprintf(stderr, "end: \n");
					entryPrint(table->heap[end->pos]);
					
					allChildPrint(tree->root, table, 0);
					exit(0);
				}
				
			}
		}
		else{
			
			// updating the cur node
			if(cur->hasRsibling){
				cur = cur->rsibling;
			}
			else{
				break;
			}
		}
		
	}
	return;
}




/**
 * FUNCTION: RegistNode
 * + regist this target to the right most child of the parent 
 */
void RegistNode(NODE* target, NODE* parent, TREE* tree)
{
	target->parent = parent; //setting the parent node
	if(parent->hasChild != 1){
		// this parent did not have any child
		parent->rchild = target;
		parent->lchild = target;
		parent->hasChild = 1;
	}
	else if(EqualNode(parent->lchild, parent->rchild)){
		// parent has only one child
		parent->lchild->rsibling = target;
		parent->lchild->hasRsibling = 1;
		
		target->lsibling = parent->lchild;
		target->hasLsibling = 1;
		
		parent->rchild = target;
	}
	else{
		//the current right most child is now changed
		parent->rchild->rsibling = target;
		parent->rchild->hasRsibling = 1;
		
		target->lsibling = parent->rchild;
		target->hasLsibling = 1;
		
		// the next right most child should be changed
		parent->rchild = target;
	}
}



/**
 * FUNCTION: SetElement
 * + set the entry in the entry and hash tables and regist 
 *   its corresponding node into the right most child in the parent node
 *
 **/
void SetElement(ELEMENT* e, int* subset, int subsetLen, NODE* parent, TREE* tree, TABLE* table, LIST** map, int t_count, int e_count)
{
	
	// setting list
	int hash_key = GetHashKey(subset, subsetLen, HASH_SIZE);
	
	// current position where the entry is firstly registed
	SetList(e->list, hash_key, table->last + 1, map);
	
	// setting entry
	SetEntry(e->entry, subset, subsetLen, e->list, t_count, e_count);
	e->entry->n_link = e->node; // we set this entry's n_link as this node
	
	// register this entry
	RegistEntry(e->entry, tree, map, table);
	
	// setting node
	e->node->pos = e->entry->link->pos;
	e->node->hasRsibling = 0; // this has not any right sibling;
	e->node->hasLsibling = 0; // this has not any left sibling;
	e->node->hasChild = 0; // this has not any child;
	
	RegistNode(e->node, parent, tree);
	
}


/**
 * FUNCTION: RemoveSibParNode
 **/
void RemoveSibParNode(NODE* target)
{
	
	if(target->hasRsibling){
		// there is a right sibling
		target->hasRsibling = 0;
		if(target->hasLsibling){
			// there is left sibling
			target->hasLsibling = 0;
			// updating the left and right siblings
			target->lsibling->rsibling = target->rsibling;
			target->rsibling->lsibling = target->lsibling;
		}
		else {
			// the target node is the left most child node
			// updating the right sibling and parent nodes
			target->rsibling->hasLsibling = 0; // the rsibling has no left node by removing the target
			// the left most child is now target->rsibling
			target->parent->lchild = target->rsibling;
			
		}
	}
	else {
		// there is no right sibling, taht is the target node is the right most child
		if(target->hasLsibling){
			// there is the left sibling
			target->hasLsibling = 0;
			// updating the left sibling and parent nodes
			target->lsibling->hasRsibling = 0; // the lsibling has no right node by removing the target
			// the right most child is now target->lsibling
			target->parent->rchild = target->lsibling;
		}
		else {
			// there is no right sibling
			// this means that the parent has only the target node as the unique child
			// updating the parent node
			target->parent->hasChild = 0; // there is no child of the parent node by removing the target
			
		}
	}
	
}




/**
 * FUNCTION: RemoveElement
 */
int RemoveElement(TREE* tree, int index, TABLE* table, LIST** map)
{	
	int hash_key;
	int min_count;
	
	//this entry is removed 
	ENTRY* entry = GetSetTable(index, table, tree, map);
	min_count = entry->e_count;
	
	LIST* list = entry->link;
	NODE* node = entry->n_link;
	
	//resetting this node 
	ResetNode(node);
	
	//resetting this list
	hash_key = GetHashKey(entry->subset, entry->length, HASH_SIZE);
	
	ResetList(list, map, hash_key);
	
	tree->cur_index++; //we return this tuple to the stack table
	
	ELEMENT* e = tree->stack[tree->cur_index];
	
	e->entry = entry;
	e->list = list;
	e->node = node;
	
	return min_count;
}

/**
 * FUNCTION: ResetNode
 */
void ResetNode(NODE* target)
{
	if(target->hasChild != 1){
		// there is no child of this target node
		RemoveSibParNode(target);
	}
	else {
		
		if(target->hasRsibling){
			// there is a right sibling
			target->hasRsibling = 0;
			
			//updating the right sibling and right child
			target->rsibling->lsibling = target->rchild;
			target->rchild->rsibling = target->rsibling;
			target->rchild->hasRsibling = 1;
			
			if(target->hasLsibling){
				// there is left sibling
				target->hasLsibling = 0;
				
				// updating the left siblings
				target->lsibling->rsibling = target->lchild;
				target->lchild->lsibling = target->lsibling;
				target->lchild->hasLsibling = 1;
				
			}
			else {
				// the left most child is now target->lsibling
				target->parent->lchild = target->lchild;
			}
		}
		else {
			// there is no right sibling, taht is the target node is the right most child
			// the right most child is now target->lsibling
			
			target->parent->rchild = target->rchild;
			
			if(target->hasLsibling){
				// there is the left sibling
				target->hasLsibling = 0;
				
				// updating the left sibling and parent nodes
				target->lsibling->rsibling = target->lchild;
				target->lchild->lsibling = target->lsibling;
				target->lchild->hasLsibling = 1;
				
			}
			else {
				// there is no left sibling
				// this means that the parent has only the target node as the unique child
				// updating the parent node
				target->parent->lchild = target->lchild;
			}
		}
		
		// update the parent node of target's children
		NODE* cur = target->lchild;
		while(1){
			cur->parent = target->parent;
			if(cur->hasRsibling){
				cur = cur->rsibling;
			}
			else {
				break;
			}
		}
	}
	
}


/**
 * FUNCTION: Remove
 */
void Remove(TREE* tree, TABLE* table, LIST** map, PARAMETER* para)
{
	//int pre_delta = table->delta;
	int next_delta;
	while(doRemove(table, para)){
		// remove the minimal entry
		next_delta = RemoveElement(tree, 1, table, map);
		table->delta = next_delta; //updating the delta value
		//fprintf(stderr, "table delta = %d\n", table->delta);
	}
	
}

/**
 * FUNCTION: Compressed
 * return a compressed tree
 
void Compressed(TREE* tree, TABLE* table, LIST** map, PARAMETER* para)
{
	NODE* cur = getLeftMost(tree->root);
	//int flag = 0;
	while( hasNextLeaf(cur) )
	{
		NODE* tmp = nextLeaf(cur); // the next target leaf node
		fprintf(stderr, "cur: ");
		entryPrint(table->heap[cur->pos]);
		fprintf(stderr, "tmp: ");
		entryPrint(table->heap[tmp->pos]);
		if(mergeable(cur, table))
		{
			// compressing the path from node to the root
			if(table->heap[cur->parent->pos]->e_count < table->heap[cur->pos]->e_count){
				table->heap[cur->parent->pos]->e_count = table->heap[cur->pos]->e_count;
				Reorder(table, tree, cur->parent->pos, map);	
			}
			
			// removing the current node
			RemoveElement(tree, cur->pos, table, map, para);
			
		}
		cur = tmp; // the target node cur is updated to tmp
	}	
}*/


// Delta compression
void Compress(TREE* tree, TABLE* table, LIST** map, int type)
{
	NODE* cur = getLeftMost(tree->root);
	NODE* next = cur;
	//int flag = 0;
	while( cur -> pos != 0 )
	{
		//the next node next is updated
		if(next->hasRsibling){
			next = getLeftMost(next->rsibling);
		}
		else {
			next = next->parent;
		}
        if(has_d_covered(cur, tree, table, map, type)){
			// removing the current node
			RemoveElement(tree, cur->pos, table, map);
		}
		// the cur node is updated
		cur = next;
	}
}

int has_d_covered(NODE* cur, TREE* tree, TABLE* table, LIST** map, int type)
{
    int target;
    if(type == 0){
        // checking only the parent node of cur

        target = cur->parent->pos;

        return check_mergeable(cur->pos, target, table, tree, map);
    }
    else{
        // exactly checking the Delta-coverness
        int* itemset = table->heap[cur->pos]->subset;
        int num = table->heap[cur->pos]->length;
        for(target = 1; target <= table->last; target++){
            if(target == cur->pos || !Includes(table->heap[target], itemset, num)){
                continue;
            }
            if(check_mergeable(cur->pos, target, table, tree, map)){
                return 1;
            }
        }
        return 0;
    }
}

int check_mergeable(int cur, int target, TABLE* table, TREE* tree, LIST** map)
{
    int t_count;
    
    if(target == 0){
        // target corresponds to the root entry
        t_count = 0;
    }
    else{
        // set t_count as the true frequency count of target
        t_count = table->heap[target]->t_count;
    }
    if(target != 0 && table->heap[cur]->e_count <= t_count + table->delta){

        // mergeable
        // updating the target frequency count
        table->heap[target]->e_count = table->heap[cur]->e_count;
        // shifting the taget entry for the heap table

        Reorder(table, tree, target, map);

        // The target Delta-covers the cur node
        return 1;
    }
    else{
        return 0;
    }
}



/*
int mergeable(NODE* node, TABLE* table)
{
	if( node->pos == 0 || node->parent->pos == 0){
		return 0;
	}
	else if( table->heap[node->pos]->e_count <= table->heap[node->parent->pos]->t_count + table->delta ){
		// this is mergeable
		return 1;
	}
	else {
		return 0;
	}
}
 */

NODE* getLeftMost(NODE* node)
{	
	NODE* cur = node;
	while(cur->hasChild == 1){
		cur = cur->lchild;
	}
	return cur;
}

int hasNextLeaf(NODE* node)
{	
	if(node->hasRsibling == 0 && node->parent->pos == 0){
		return 0;
	}
	else {
		// there is a right sibling or a parent that is not root
		return 1;
	}
}

NODE* nextLeaf(NODE* node)
{
	NODE* cur = node;
	if(cur->hasRsibling){
		cur = cur->rsibling;
		fprintf(stderr, "a");
		return getLeftMost(cur);
	}
	else {
		while(cur->parent->pos != 0){
			cur = cur->parent;
			if(cur->hasRsibling){
				fprintf(stderr, "b");
				cur = cur->rsibling;
				return getLeftMost(cur);
			}
		}
		fprintf(stderr, "c");
		return cur;
	}
}



/**
 * FUNCTION: doRemove
 * Modified for integrating PC and RC approximation 2017/11/2
 */
int doRemove(TABLE* table, PARAMETER* para)
{
	if(table->last == 0){
		return 0;
	}
	int flag = 0;	// if 0, stop the remove process; else 1, then continue
    double pc_threshold = para->error*para->time;
    int rc_threshold = para->table_size;
    
    if(para->lc && para->ss){
        // exploiting the integrated PO and RO approximation deletion
        // checking the table size
        if(table->last > rc_threshold){
            // we need to remove the unpromising entry
            flag = 1;
        }
        else{
            if(table->heap[1]->e_count <= pc_threshold){
                // we need to remove the minimal entry
                flag = 1;
            }
        }
        return flag;
    }
	else if(para->lc){
		// in the lossy counting manner
		if(table->heap[1]->e_count <= pc_threshold){
			// we need to remove the unpromising entry
			flag = 1;
		}
	}
	else if(para->ss){
		if(table->last > rc_threshold){
			// we need to remove the unpromising entry
			flag = 1;
		}
	}
	return flag;
}

/**
 * FUNCTION: Equal
 * + checking if both two nodes indicate the same entry
 * output 1 if they are equal to each other; otherwise 0
 */
int EqualNode(NODE* a, NODE* b)
{
	// we decide the correspondence by their positions in the entry table
	if( a->pos == b->pos){
		return 1;
	}
	else {
		return 0;
	}
}

/*
 * FUNCTION: Increment
 * + increment the f value of each child of the target child
 */
void Increment(NODE* target, TREE* tree, TABLE* table, LIST** map)
{
	table->heap[target->pos]->e_count++;
	table->heap[target->pos]->t_count++;
	Reorder(table, tree, target->pos, map);
	if(target->hasChild){
		NODE* a = target->lchild;
		while(1){
			Increment(a, tree, table, map);
			if(a->hasRsibling == 1){
				// there is a's sibling
				a = a->rsibling;
			}
			else {
				// there is no longer sibling 
				break;
			}
		}
	}
	return;
}

/**
 * FUNCTION: GetElement
 * OPTIONS
 *	+ stack - pointer to STACK
 * RETURN
 *  + ELEMENT
 **/
ELEMENT* GetElement(TREE* tree)
{
	ELEMENT* e = tree->stack[tree->cur_index];
	tree->cur_index--;
	if(tree->cur_index < 0){
		fprintf(stderr, "Error: GetElement function\n");
		exit(3);
	}
	return e;
}


/**
 * FUNCTION: Intersection
 * + compute the intersection between two entries
 * + Complexity O(|a| + |b|)
 *
 **/
void Intersection(int* subset, int* length, ENTRY* a, ENTRY* b)
{
	intersection(subset, length, a->subset, a->length, b->subset, b->length);
}


void intersection(int* subset, int* length, int* a_set, int a_length, int* b_set, int b_length)
{
	int num = 0; // initializing num
	int i, j, ai, bj;
	i = j = 0;
	while(i < a_length && j < b_length)
	{
		ai = a_set[i];
		bj = b_set[j];
		if(ai == bj){
			subset[num] = ai;
			num++;
			i++;
			j++;
		}
		else if(ai > bj){
			// bj is no longer appeared in a
			// we increment j
			j++;
		}
		else {
			// corresponding the condition bj > ai
			// ai is no longer appeared in b
			i++;
		}
	}
	*length = num;
}




/** 
 * FUNCTION: nodePrint
 *	+ printing the node
 * OPTIONS:
 *	+ node - pointer to the node
 * RETURNS:
 *	+ void
 **/
void nodePrint(NODE* node, TABLE* table){
	
	
	fprintf(stderr, "[this node: ");
	if(node->pos == 0){
		fprintf(stderr, "root]\n");
	}
	else {
		
		entryPrint(table->heap[node->pos]);
		fprintf(stderr, "\t +parent: ");
		if(node->parent->pos != 0){
			entryPrint(table->heap[node->parent->pos]);
		}
		else {
			fprintf(stderr, "root \n");
		}
		if(node->hasRsibling == 1){
			fprintf(stderr, "\t right sibling: ");
			entryPrint(table->heap[node->rsibling->pos]);
		}
		else {
			fprintf(stderr, "\t no right sibling \n");
		}
		
		if(node->hasChild == 1){
			fprintf(stderr, "\t left most child: ");
			entryPrint(table->heap[node->lchild->pos]);
			fprintf(stderr, "\t right most child: ");
			entryPrint(table->heap[node->rchild->pos]);
		}
		else {
			fprintf(stderr, "\t no child \n");
		}
	}
	
}

/**
 * FUNCTION: treePrint
 *  + printing all the entries in this tree
 * RETURNS;
 *  + void
 **/
void allChildPrint(NODE* node, TABLE* table, int depth)
{
	fprintf(stderr, "depth: %d\n", depth);
	if(node->pos == 0){
		fprintf(stderr, "root, delta = %d\n", table->delta);
	}
	else{
		fprintf(stderr, "pos: %d\t", node->pos);
		entryPrint(table->heap[node->pos]);
	}
	if(node->hasChild == 1){
		depth++;
		allChildPrint(node->lchild, table, depth);
		depth--;
	}
	if(node->hasRsibling == 1){
		allChildPrint(node->rsibling, table, depth);
	}
	return;
}



/**
 * FUNCTION: FreeTree
 *	+ opening tree
 * OPTIONS
 *	+ tree - pointer to TREE
 *	+ para - pointer to PARAMETER
 * RETURN
 *	+ void
 **/
void FreeTree(TREE *tree, PARAMETER *para)
{
	fprintf(stderr,"FreeTree\t");
	free(tree->stack);
	free(tree->root);
	free(tree);
	fprintf(stderr,"OK!\n");
}

int debag1(STREAM stream)
{
	int i;
	for(i = 0; i < stream.num; i++){
		if(stream.itemset[i] == 41){
			return 1;
		}
	}
	return 0;
}

void debag2(NODE* root, TABLE* table)
{
	int i;
	int ans[2];
	ans[0] = 453;
	ans[1] = 908;
	NODE* cur = root->lchild;
	while(1){
		if(table->heap[cur->pos]->length == 2){
			for(i = 0; i < 2; i++){
				if(ans[i] != table->heap[cur->pos]->subset[i]){
					break;
				}
			}
			if(i == 2){
				// find!
				nodePrint(cur, table);
			}
		}
		if(cur->hasRsibling == 1){
			cur = cur->rsibling;
		}
		else {
			break;
		}
	}
}


int debag3(int* itemset, int length)
{
	return 0;

}


void debag4(TABLE* table, LIST** map, PARAMETER* para)
{
}


int debag5(ENTRY* entry)
{
	return 1;
}


