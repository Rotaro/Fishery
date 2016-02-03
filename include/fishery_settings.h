/*****************************************************************************
* Filename: fishery_settings.h												 *
*																			 *
* Contains functions for creating and manipulating fishery simulation        *
* settings.																	 *
*																			 *
*****************************************************************************/


#ifndef FISHERY_SETTINGS_H_
#define FISHERY_SETTINGS_H

#include "fishery_data_types.h"
#include "help_functions.h"

int ValidateSettings(
	Fishery_Settings settings, int output_print);
Fishery_Settings CreateSettings(
	int size_x, int size_y, int initial_vegetation_size,
	int vegetation_level_max, int vegetation_level_spread_at, int vegetation_level_growth_req,
	int soil_energy_max, int soil_energy_increase_turn,
	int *vegetation_consumption,
	int initial_fish_size, int fish_level_max, int fish_growth_req, int fish_moves_turn,
	int *fish_consumption,
	int random_fishes_interval, int split_fishes_at_max,
	int fishing_chance);
void PrintSettings(
	Fishery_Settings settings);
int AddSetting(
	Fishery_Settings *settings, const char *setting_name, void *setting_value);

#endif