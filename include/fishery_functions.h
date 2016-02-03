/*****************************************************************************
* Filename: fishery_functions.h												 *
*																			 *
* Contains functions for creating and manipulating fishery simulations.		 *
*																			 *
******************************************************************************/

#ifndef FISHERY_FUNCTIONS_H_
#define FISHERY_FUNCTIONS_H_

#include "fishery_data_types.h"
#include "help_functions.h"

int CheckFishMemory(Fishery *fishery, Fishery_Settings settings);

Fishery *CreateFishery(Fishery_Settings settings);
void DestroyFishery(void *fishery);

Fishery_Results UpdateFishery(Fishery *fishery, Fishery_Settings settings, int n);
void UpdateFisheryVegetation(Fishery *fishery, Fishery_Settings settings);
void UpdateFisheryFishPopulation(Fishery *fishery, Fishery_Settings settings);
int FishingEvent(Fishery *fishery, Fishery_Settings settings);

#endif /* FISHERY_FUNCTIONS_H_ */