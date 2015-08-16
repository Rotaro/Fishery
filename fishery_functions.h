/*
fishery_functions.h

Contains all functions which create, manipulate or return simulation instances.

*/

#ifndef FISHERY_FUNCTIONS_H_
#define FISHERY_FUNCTIONS_H_

#include "fishery_data_types.h"
#include "help_functions.h"

int CheckFishMemory(Fishery *fishery, Fishery_Settings settings);

int ValidateSettings(Fishery_Settings settings, int output_print);
Fishery_Settings CreateSettings(
	int size_x, int size_y, int initial_vegetation_size,
	int vegetation_level_max, int vegetation_level_spread_at, int vegetation_level_growth_req,
	int soil_energy_max, int soil_energy_increase_turn,
	int *vegetation_consumption,
	int initial_fish_size, int fish_level_max, int fish_growth_req, int fish_moves_turn,
	int *fish_consumption,
	int random_fishes_interval, int split_fishes_at_max,
	double fishing_chance);

Fishery *CreateFishery(Fishery_Settings settings);

Fishery_Results UpdateFishery(Fishery *fishery, Fishery_Settings settings, int n);
void UpdateFisheryVegetation(Fishery *fishery, Fishery_Settings settings);
void UpdateFisheryFishPopulation(Fishery *fishery, Fishery_Settings settings);
int FishingEvent(Fishery *fishery, Fishery_Settings settings);
void DestroyFishery(Fishery *fishery);
Fishery *GetFishery(Fishery *fishery, Fishery_Settings settings); /* Hmmmm */

#endif /* FISHERY_FUNCTIONS_H_ */