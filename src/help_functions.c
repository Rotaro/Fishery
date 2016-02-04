/*****************************************************************************
* Filename: help_functions.c												 *
*																			 *
* Contains various help functions needed in the implementation of the		 *
* fishery simulation.														 *
*																			 *
******************************************************************************/
#include "help_functions.h"
#include <stdio.h>
#include <stdlib.h>


/* Function: LListCreate
 * ---------------------
 * Creates empty linked list.
 *
 * Returns:		*LList_Node pointer to start of linked list.
 */
LList_Node *LListCreate(void) {
	LList_Node *root;

	root = malloc(sizeof(LList_Node));
	if (root == NULL) {
		printf("Failed to reserve memory for stack.\n");
		return NULL;
	}
	root->next = NULL;
	root->node_value = NULL;
	return root;
}
/* Function: LListIsEmpty
 * ----------------------
 * Checks if linked list is empty, e.g. the first node is empty
 * and does not link to another node. Returns 1 if empty, 0 otherwise.
 *
 * *root	LList_Node poitner to start of linked list.
 *
 * Returns: Integer as boolean for emptiness of linked list. 1 if empty,
 *          0 if not.
 */
int LListIsEmpty(LList_Node *root) {
	if (root->next == NULL && root->node_value == NULL)
		return 1;
	else
		return 0;
}
/* Function: LListAdd
 * ------------------
 * Adds node to linked list pointing to node_value. 
 *
 * root:		LList_Node pointer start of linked list.
 * node_value:	Pointer to value to be stored in list. 
 *
 * Returns:		LList_Node pointer to new node in linked list. 
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
/* Function LListPop
 * -----------------
 * Pops (i.e. removes from list and returns) node_value. 
 * CompareValues is used to compare node values and node_value. 
 * If node_value is NULL, the last node of the list is popped. 
 * Returns a pointer to the value of the removed node, or NULL if 
 * no matching node is found. 
 *
 * *root:			LList_Node pointer to start of linked list.
 * *node_value:		Pointer to value to be removed from list.
 * *CompareValues:	Pointer to function which compares values. Returns 
 *                  1 if values are the same, 0 otherwise.
 * 
 * Returns:			Pointer to value of removed node, or NULL if
 *					value not found.
 */
void *LListPop(
	LList_Node *root, const void *node_value, 
	int (*CompareValues)(const void *value1, const void *value2)) {
	LList_Node *prev, *curr;
	void *found_node_value;

	prev = root;
	curr = root;
	if (LListIsEmpty(curr))
		return NULL;

	if (node_value == NULL) {
		/* Pop last node. */
		while (curr->next != NULL) {
			prev = curr;
			curr = curr->next;
		}
		found_node_value = curr->node_value;
		curr->node_value = NULL;
		if (curr == root) { 
			/* Only one node in list. */
			return found_node_value;
		}
		free(curr);
		prev->next = NULL;
		return found_node_value;
	}
	/* Look for node_value. */
	while (CompareValues(curr->node_value, node_value) != 1) {
		if (curr->next == NULL)
			/* Value not found in list. */
			return NULL;
		prev = curr;
		curr = curr->next;
	}
	if (curr->next != NULL) {
		/* Value found in list and not last node. */
		found_node_value = curr->node_value;
		curr->node_value = NULL;
		if (curr == root) {
			/* First node contains value,
			set root to contain next node value. */
			curr = root->next;
			root->node_value = root->next->node_value;
			root->next = root->next->next;
			free(curr);
			return found_node_value;
		}
		/* Remove node containing value. */
		prev->next = curr->next;
		free(curr);
		return found_node_value;
	}
	else {
		/* Value found in last node of list.*/
		found_node_value = curr->node_value;
		curr->node_value = NULL;
		if (curr == prev) {
			/* Only one node in list. */
			return found_node_value;
		}
		/* Remove node containing value. */
		prev->next = NULL;
		free(curr);
		return found_node_value;
	}
}
/* Function LListSearch
 * --------------------
 * Searches for node_value in linked list. Returns pointer to value
 * of node.
 *
 * *root:			LList_Node pointer to start of linked list.
 * *node_value:		Pointer to value to be removed from list.
 * *CompareValues:	Pointer to function which compares values. Returns
 *                  1 if values are the same, 0 otherwise.
 *
 * Returns:			Pointer to value of removed node, or NULL if
 *					value not found.
 */
void *LListSearch(
	LList_Node *root, const void *node_value,
	int(*CompareValues)(const void *value1, const void *value2)) {
	LList_Node *curr;

	if (LListIsEmpty(root))
		return NULL;

	curr = root;
	/* Look for node value. */
	while (CompareValues(curr->node_value, node_value) != 1) {
		if (curr->next == NULL)
			/* Value not found. */
			return NULL;
		curr = curr->next;
	}
	return curr->node_value;
}

/* Function: LListDestroy
 * ----------------------
 * Destroys and frees memory of a linked list.
 * 
 * root:		LList_Node pointer to start of linked list.
 * FreeValue:	Function which frees the memory pointed to by a node.
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
/* Function GetNewCoords().
 * Generates new, random coordinates for fish pool. New coordinates
 * are checked to be not out of bounds of the vegetation layer and to 
 * not contain any fish pools. Coordinates are prioritized according 
 * to vegetation level. Returns -1 if no coordinates can be generated.
 * 
 * cur_coords:	Current coordinates of fish pool in one dimension.
 * radius:		Largest allowed distance from current coordinates to
 *			    new coordinates.
 * size_x:		Width of vegetation layer in fishery simulation.
 * size_y:		Height of vegetation layer in fishery simulation.
 * *fishery:	Pointer to initialized Fishery.
 * 
 * Returns:		Return value. New coordinates in one dimension.
 *				Returns -1 if no possible coordinates are available.
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
			if (fishery->vegetation_layer[candidate_coords].local_fish == NULL 
				&& candidate_coords != cur_coords && 
				fishery->vegetation_layer[candidate_coords]
					.vegetation_level > 1) {
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
		rand_number = GENERATERANDINT(0, valid_veg_coords - 1);
		new_pos = poss_veg_coords[rand_number];
	}
	else if (valid_coords > 0) {
		rand_number = GENERATERANDINT(0, valid_coords - 1);
		new_pos = poss_coords[rand_number];
	}

	free(poss_coords);
	free(poss_veg_coords);
	return new_pos;
}
/* Function: CompareInts
* Compares integer values of integer pointers.
*
* *int1:	Pointer to integer.
* *int2:	Pointer to integer.
*
* Returns: Integer as result of comparison. 1 if they are
*          the same, 0 otherwise.
*/
int CompareInts(const void *int1, const void *int2) {
	if (*((int*)int1) == *((int*)int2))
		return 1;
	else
		return 0;
}
/* Function: ComparePointers
 * Compares values of two pointers.
 * 
 * *ptr1: Pointer.
 * *ptr2: Pointer.
 *
 * Returns: Integer result of comparison. 1 if pointers have the 
 *          same value, 0 otherwise.
 */
int ComparePointers(const void *ptr1, const void *ptr2) {
	if (ptr1 == ptr2)
		return 1;
	else
		return 0;
}
/* Function: CompareFisheries
 * Compares Fishery to fishery_id.
 *
 * *fishery:	Pointer to Fishery.
 * *fishery_id:	Pointer to fishery_id.
 *
 * Returns: Integer as result of comparison. 1 if they are
 *          the same, 0 otherwise.
 */
int CompareFisheries(const void *fishery1, const void *fishery_id) {
	if (((Fishery *)fishery1)->fishery_id == *(int *) fishery_id)
		return 1;
	else
		return 0;
}