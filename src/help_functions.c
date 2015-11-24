/* help_functions.c
*
* Contains various help functions needed in the implementation of the fishery simulation.
*
*/

#include "help_functions.h"
#include <stdio.h>
#include <stdlib.h>
/* Function LListCreate().
* Creates empty linked list. Returns pointer to first node.
*/
LList_Node *LListCreate(void) {
	LList_Node *root;

	root = malloc(sizeof(LList_Node));
	if (root == NULL) {
		printf("Failed to reserve memory for stack.\n");
		exit(EXIT_FAILURE);
	}
	root->next = NULL;
	root->node_value = NULL;
	return root;
}
/* Function LListIsEmpty()
* Checks if linked list is empty, e.g. the first node is empty
* and does not link to another node. Returns 1 if empty, 0 otherwise.
*
* root		  - Start of linked list.
*/
int LListIsEmpty(LList_Node *root) {
	if (root->next == NULL && root->node_value == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/* Function LListAdd().
* Adds node pointing to provided value. 
*
* root		  - Start of linked list.
* node_value  - Pointer to element to be stored in list. 
*/
LList_Node *LListAdd(
	LList_Node *root, void *node_value) {
	LList_Node *next;

	if (LListIsEmpty(root)) {
		root->node_value = node_value;
		return root;
	}

	next = malloc(sizeof(LList_Node));
	next->next = NULL;
	next->node_value = node_value;

	while (root->next != NULL)
		root = root->next;
	root->next = next;

	return next;
}
/* Function LListPop().
* Removes first node containing node_value encountered in list. Node values are compared 
* using CompareValues. If node_value is NULL, the last element of the list is removed. Returns 
* a pointer to the value of the removed node, or NULL if no matching node is found. 
*
* root				- Start of linked list.
* node_value			- Value of node to be removed from list.
* CompareValue		- Function which compares node values. Returns 1 if a match, 0
*					  otherwise.
* 
* found_node_value	- Return value. Pointer to value of removed node.
*/
void *LListPop(
	LList_Node *root, const void *node_value, int (*CompareValues)(const void *value1, const void *value2)) {
	LList_Node *prev;
	void *found_node_value;
	prev = root;
	/* Both root and prev pointers are actively used in traversing the list. */
	if (LListIsEmpty(root))
		return 0;
	/* Pop last element. */
	if (node_value == NULL) {
		while (root->next != NULL) {
			prev = root;
			root = root->next;
		}
		found_node_value = root->node_value;
		root->node_value = NULL;
		if (root == prev) { /* Only one element in list. */
			return found_node_value;
		}
		free(root);
		prev->next = NULL;
		return found_node_value;
	}
	/* Look for matching element. */
	while (CompareValues(root->node_value, node_value) != 1) {
		if (root->next == NULL) 
			/* Value not found in list. */
			return NULL;
		prev = root;
		root = root->next;
	}
	if (root->next != NULL) {
		/* Value found in list and not last element. */
		found_node_value = root->node_value;
		root->node_value = NULL;
		if (root == prev) { 
			/* First element contains desired value,
			set root to contain next element. */
			prev = prev->next;
			root->node_value = root->next->node_value;
			root->next = root->next->next;	
			free(prev);
			return found_node_value;
		}
		/* Remove node containing value. */
		prev->next = root->next;
		free(root);
		return found_node_value;
	}
	else {
		/* Value found in last element of list.*/
		found_node_value = root->node_value;
		root->node_value = NULL;
		if (root == prev) { 
			/* Only one element in list. */
			return found_node_value;
		}
		/* Remove node containing value. */
		prev->next = NULL;
		free(root);
		return found_node_value;
	}
}
/* Function LListDestroy()
* Destroys and frees memory of a linked list.
* 
* root	  - Pointer to start of linked list.
* FreeValue - Function which frees the memory pointed to by a node.
*/
void LListDestroy(LList_Node *root, void (*FreeValue)(void *node_value)) {
	LList_Node *next;
	if (LListIsEmpty(root)) {
		free(root);
		return;
	}
	next = root->next;
	while (next != NULL) {
		FreeValue(root->node_value);
		free(root);
		root = next;
		next = next->next;
	}
	FreeValue(root->node_value);
	free(root);
}
int CompareInts(const void *int1, const void *int2) {
	if (*((int*)int1) == *((int*)int2))
		return 1;
	else
		return 0;
}
void FreeInt(void *value) {
	free(value);
}
/* Function GetNewCoords().
* Generates new, random coordinates for fish pool. New coordinates
* are checked to be not out of bounds and to not contain any fish pools.
* Coordinates are prioritized according to vegetation level.
* Returns -1 if no coordinates can be generated.
* 
* cur_coords	- Current coordinates of fish pool in one dimension.
* radius		- Largest allowed distance from current coordinates to
*				  new coordinates.
* size_x		- Width of vegetation layer in fishery simulation.
* size_y		- Height of vegetation layer in fishery simulation.
* fishery		- Pointer to initialized fishery simulation.
* 
* new_coords  - Return value. New coordinates in one dimension.
* Returns -1 if no possible coordinates are available.
*/
int GetNewCoords(
	int cur_coords, int radius, int size_x, int size_y, Fishery *fishery) {
	int i, j, new_pos = -1, coords_x, coords_y, *poss_coords, *poss_veg_coords,
		candidate_coords, start_x, start_y, end_x, end_y, valid_coords = 0,
		valid_veg_coords = 0, rand_number=0;

	if (cur_coords < 0 || cur_coords > size_x*size_y - 1)
		/* Invalid current coordinates. */
		return -1;
	/* Find possible coordinates. */
	/* coords_x = cur_coords % size_x; */
	coords_x = cur_coords / size_y;
	/* coords_y = cur_coords / size_x; */
	coords_y = cur_coords % size_y;
	start_x = coords_x - radius < 0 ? 0 : coords_x - radius;
	start_y = coords_y - radius < 0 ? 0 : coords_y - radius;
	end_x = coords_x + radius > size_x - 1 ? size_x - 1 : coords_x + radius;
	end_y = coords_y + radius > size_y - 1 ? size_y - 1 : coords_y + radius;
	poss_coords = malloc(sizeof(int)* // for all coords
		(end_x - start_x + 1)*(end_y - start_y + 1));
	poss_veg_coords = malloc(sizeof(int)* // for coords with vegetation
		(end_x - start_x + 1)*(end_y - start_y + 1));
	/* Determine if vegetation tile is empty of fish and if it contains
	vegetation. */
	for (i = start_x; i <= end_x; i++) {
		for (j = start_y; j <= end_y;j++ ) {
			/* candidate_coords = i + j*size_x; */
			candidate_coords = j + i*size_y;
			if (fishery->vegetation_layer[candidate_coords].local_fish == NULL &&
				candidate_coords != cur_coords && 
				fishery->vegetation_layer[candidate_coords].vegetation_level > 1) {
				poss_veg_coords[valid_veg_coords] = candidate_coords;
				valid_veg_coords++;
			}
			else if (fishery->vegetation_layer[candidate_coords].local_fish == NULL &&
				candidate_coords != cur_coords) {
				poss_coords[valid_coords] = candidate_coords;
				valid_coords++;
			}
		}
	}
	/* Choose random coordinates if possible.*/
	if (valid_veg_coords) {
		rand_number = (int)(rand() / (double)(RAND_MAX + 1) * valid_veg_coords);
		new_pos = poss_veg_coords[rand_number];
	}
	if (valid_coords > 0) {
		rand_number = (int)(rand() / (double) (RAND_MAX + 1) * valid_coords);
		new_pos = poss_coords[rand_number];
	}

	free(poss_coords);
	free(poss_veg_coords);
	return new_pos;
}
/* Function ComparePointers()
* Simple function to compare two pointers, to be replaced by macro?
*/
int ComparePointers(const void *ptr1, const void *ptr2) {
	if (ptr1 == ptr2)
		return 1;
	else
		return 0;
}