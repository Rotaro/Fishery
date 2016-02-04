/*
llist_tests.h

Contains functions for testing the linked list structure used in the fishery.

*/

#ifndef LLIST_TESTS_H_
#define LLIST_TESTS_H_

#include "help_functions.h"
#include "fishery_data_types.h"
#include <stdio.h>
#include <assert.h>

int TestLList(void);
void TestLListCreate(void);
void TestLListIsEmpty(void);
void TestLListAdd(void);
void TestLListPop(void);
void TestLListSearch(void);


#endif /* LLIST_TESTS_H_ */