/*
fishery_tests.h

Contains functions for testing different properties of the fishery simulation.

*/

#ifndef FISHERY_TESTS_H_
#define FISHERY_TESTS_H_

#include <stdlib.h>
#include "fishery_data_types.h"
#include "fishery_functions.h"
#include "help_functions.h"
#include "llist_tests.h"

int TestFisheryAll(void);
int TestFisherySettings(void);
int TestInitialFishery(void);

int TestGetNewCoords(void);
#endif /* FISHERY_TESTS_H_ */