/*****************************************************************************
* Filename: fishery_data_types.h											 *
*																			 *
* Contains data structures/types used in the fishery simulation.			 *
*																			 *
******************************************************************************/

#ifndef FISHERY_DATA_TYPES_H_
#define FISHERY_DATA_TYPES_H_

#include <stdlib.h>

/* Linked list. */
typedef struct llist_node
{
	struct llist_node *next;
	void *node_value;
} LList_Node;
/* Stores fish pool information. */
typedef struct fish_pool
{
	int pop_level;
	int food_level;
	int pos_x;
	int pos_y;
} Fish_Pool;
/* Stores vegetation tile information. */
typedef struct tile
{
	int vegetation_level;
	int soil_energy;
	Fish_Pool *local_fish;
} Tile;
/* Stores fishery settings. */
typedef struct fishery_settings
{
	int size_x;
	int size_y;

	int initial_vegetation_size;
	int vegetation_level_max;
	int vegetation_level_spread_at;
	int vegetation_level_growth_req;
	int soil_energy_max;
	int soil_energy_increase_turn;
	int *vegetation_consumption;

	int initial_fish_size;
	int fish_level_max;
	int fish_growth_req;
	int fish_moves_turn;
	int *fish_consumption;

	int random_fishes_interval;
	int split_fishes_at_max;

	int fishing_chance;

} Fishery_Settings;
/* Stores fishery simulation, including settings, vegetation layer 
   and fish population. */
typedef struct fishery
{
	Tile *vegetation_layer;
	LList_Node *fish_list;
	unsigned int fishery_id;
	Fishery_Settings *settings;
} Fishery;
/* Stores fishery simulation results. */
typedef struct fishery_results
{
	int yield;
	int fish_n;
	int vegetation_n;
	int debug_stuff;

	double yield_std_dev;
	double fish_n_std_dev;
	double vegetation_n_std_dev;

	int steps;
} Fishery_Results;

#endif /* FISHERY_DATA_TYPES_H_ */