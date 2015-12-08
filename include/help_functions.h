/* help_functions.h

Contains various help functions needed in the implementation of the fishery simulation.

*/

#ifndef HELP_FUNCTIONS_H_
#define HELP_FUNCTIONS_H

#include "fishery_data_types.h"

#define GENERATERANDINT(a, b)  ((int) ((double) rand() / (RAND_MAX + 1L) * ((b-a) + 1) + a))

/* Functions for creating, manipulating and destroying linked list structures. */
LList_Node *LListCreate(void);
int LListIsEmpty(LList_Node *root);
LList_Node *LListAdd(LList_Node *root, void *node_value);
void *LListPop(LList_Node *root, const void *node_value, int (*CompareValues)(const void *value1, const void *value2));


void LListDestroy(LList_Node *root, void (*FreeValue)(void *node_value));

/* Helper functions for using ints in linked list structures. */
int CompareInts(const void *int1, const void *int2);
void FreeInt(void *value);

/* Other help functions.*/
int GetNewCoords(int cur_pos, int radius, int size_x, int size_y, Fishery *fishery);
int ComparePointers(const void *ptr1, const void *ptr2);

#endif /* HELP_FUNCTIONS_H */

