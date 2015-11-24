/*
 * fishery_functions.c
 *
 * Contains all functions which create or manipulate fisheries.
 *
 */

#include "fishery_functions.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* setting_order, settings_size and list_indexes_sizes are used to parse 
 * settings in the python module. */
const char *setting_order[] = { "size_x", "size_y",
"initial_vegetation_size", "vegetation_level_max", "vegetation_level_spread_at",
"vegetation_level_growth_req", "soil_energy_max", "soil_energy_increase_turn",
"vegetation_consumption", 
"initial_fish_size", "fish_level_max", "fish_growth_req", "fish_moves_turn",
"fish_consumption", "random_fishes_interval", "split_fishes_at_max", "fishing_chance"};
const int settings_size = 17;
const int list_indexes_sizes[] =  {8, 3, 13, 10};

/* Function CheckFishMemory()
 * Temporary function used to check no mistakes are made when fish pools are moved
 * around in the simulation, i.e. the fishing list contains the same information
 * as the vegetation layer.

 * Input parameters:
 * fishery      - Pointer to fishery to memory of.
 * settings     - Settings of fishery.

 * Output parameters:
 * memory_ok    - 1 if memories match, 0 otherwise.
 */
int CheckFishMemory(Fishery *fishery, Fishery_Settings settings) {
	int pos, memory_ok=1;
	LList_Node *node;
	Fish_Pool *fish;

	node = fishery->fish_list;
	while (node != NULL && node->node_value != NULL) {
		fish = node->node_value;
		/* pos = fish->pos_x + fish->pos_y*settings.size_x; */
		pos = fish->pos_y + fish->pos_x*settings.size_y;
		if (fishery->vegetation_layer[pos].local_fish != fish) {
			printf("Fish memory doesn't match.\n");
			memory_ok = 0;
			return memory_ok;
		}
		node = node->next;
	}
	/* printf("Fish memory matches.\n"); */
	return memory_ok;
}

/*  Function ValidateSettings()
 * Validates fishery settings, i.e. checks for impossible values
 * which would break the simulation. Returns 1 on success, 0
 * if invalid settings.

 * settings     - Settings to be validated.
 * output_print - If 1, prints output regarding invalid settings. 
 *                If 0, no output is printed.
 */
int ValidateSettings(Fishery_Settings settings, int output_print) {
	int settings_valid = 1, i;

	if (settings.size_x <= 0 || settings.size_x > 1000) {
		settings_valid = 0;
		if (output_print == 1) printf("size_x is invalid (%d).\n", settings.size_x);
	}
	if (settings.size_y <= 0 || settings.size_y > 1000) {
		settings_valid = 0;
		if (output_print == 1) printf("size_y is invalid (%d).\n", settings.size_y);
	}

	if (settings.initial_vegetation_size < 0 ||
		settings.initial_vegetation_size > settings.size_x*settings.size_y) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("initial_vegetation_size is invalid (%d).\n",
				settings.initial_vegetation_size);
	}
	if (settings.vegetation_level_max <= 0 ||
		settings.vegetation_level_max > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("vegetation_level_max is invalid (%d).\n",
				settings.vegetation_level_max);
	}
	if (settings.vegetation_level_spread_at < 0) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("vegetation_level_spread_at is invalid (%d).\n",
				settings.vegetation_level_spread_at);
	}
	if (settings.vegetation_level_growth_req < 0 ||
		settings.vegetation_level_growth_req > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("vegetation_level_growth_req is invalid (%d).\n", 
				settings.vegetation_level_growth_req);
	}
	for (i = 0; i < settings.vegetation_level_max+1; i++) {
		if (settings.vegetation_consumption[i] < 0) {
			settings_valid = 0;
			if (output_print == 1) 
				printf("vegetation_consumption is invalid (%d).\n", 
					settings.vegetation_consumption[i]);
		}
	}
	if (settings.soil_energy_increase_turn < 0 ||
		settings.soil_energy_increase_turn > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("soil_energy_increase_turn is invalid (%d).\n", 
				settings.soil_energy_increase_turn);
	}
	if (settings.soil_energy_max < 0 || settings.soil_energy_max > 1000) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("soil_energy_max is invalid (%d).\n", settings.soil_energy_max);
	}

	if (settings.initial_fish_size < 0 ||
		settings.initial_fish_size > settings.size_x*settings.size_y) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("initial_fish_size is invalid (%d).\n", settings.initial_fish_size);
	}
	if (settings.fish_growth_req < 0 || settings.fish_growth_req > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("fish_growth_req is invalid (%d).\n", settings.fish_growth_req);
	}
	if (settings.fish_level_max < 0 || settings.fish_level_max > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("fish_level_max is invalid (%d).\n", settings.fish_level_max);
	}
	if (settings.fish_moves_turn < 0 || settings.fish_moves_turn > 100) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("fish_moves_turn is invalid (%d).\n", settings.fish_moves_turn);
	}
	for (i = 0; i < settings.fish_level_max + 1; i++) {
		if (settings.fish_consumption[i] < 0) {
			settings_valid = 0;
			if (output_print == 1) 
				printf("fish_consumption is invalid (%d).\n", settings.fish_consumption[i]);
		}
	}
	if (settings.random_fishes_interval < 0 ||
		settings.random_fishes_interval > 1000) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("random_fishes_interval is invalid (%d).\n", 
				settings.random_fishes_interval);
	}
	if (settings.split_fishes_at_max < 0 ||
		settings.split_fishes_at_max > settings.fish_level_max) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("split_fishes_at_max is invalid (%d), fish_level_max: %d.\n", 
				settings.split_fishes_at_max, settings.fish_level_max);
	}
	if (settings.fishing_chance < 0.0 || settings.fishing_chance > 1.0) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("fishing_chance is invalid (%f).\n", settings.fishing_chance);
	}

	return settings_valid;
}
/* Function PrintSettings()
 * Prints settings, only used for debugging.
 *
 * settings    - Settings to be printed.
 */
void PrintSettings(Fishery_Settings settings) {
	int i;
	printf("%s: %d\n", "size_x", settings.size_x);
	printf("%s: %d\n", "size_y", settings.size_y);
	printf("%s: %d\n", "initial_vegetation_size", settings.initial_vegetation_size);
	printf("%s: %d\n", "vegetation_level_max", settings.vegetation_level_max);
	printf("%s: %d\n", "vegetation_level_spread_at",
		settings.vegetation_level_spread_at);
	printf("%s: %d\n", "vegetation_level_growth_req",
		settings.vegetation_level_growth_req);
	printf("%s: [", "vegetation_consumption");
	for (i = 0; i < settings.vegetation_level_max + 1; i++) {
		printf("%d ", settings.vegetation_consumption[i]);
	}
	printf("]\n");
	printf("%s: %d\n", "soil_energy_increase_turn", settings.soil_energy_increase_turn);
	printf("%s: %d\n", "soil_energy_max", settings.soil_energy_max);
	printf("%s: %d\n", "initial_fish_size", settings.initial_fish_size);
	printf("%s: %d\n", "fish_growth_req", settings.fish_growth_req);
	printf("%s: %d\n", "fish_level_max", settings.fish_level_max);
	printf("%s: %d\n", "fish_moves_turn", settings.fish_moves_turn);
	printf("%s: [", "fish_consumption");
	for (i = 0; i < settings.fish_level_max + 1; i++) {
		printf("%d ", settings.fish_consumption[i]);
	}
	printf("]\n");
	printf("%s: %d\n", "random_fishes_interval", settings.random_fishes_interval);
	printf("%s: %d\n", "split_fishes_at_max", settings.split_fishes_at_max);
	printf("%s: %f\n", "fishing_chance", settings.fishing_chance);
}
/* Function CreateSettings()
 *  Creates Settings structure which is used in functions which manipulate the 
 *  fishery. Returns settings structure.
 */
Fishery_Settings CreateSettings(
	int size_x, int size_y, int initial_vegetation_size,
	int vegetation_level_max, int vegetation_level_spread_at,
	int vegetation_level_growth_req, 	int soil_energy_max,
	int soil_energy_increase_turn,	int *vegetation_consumption, 
	int initial_fish_size, int fish_level_max, int fish_growth_req, int fish_moves_turn,
	int *fish_consumption,
	int random_fishes_interval, int split_fishes_at_max, 
	double fishing_chance) {

	Fishery_Settings settings;

	settings.size_x = size_x;
	settings.size_y = size_y;
	settings.initial_vegetation_size = initial_vegetation_size;
	
	settings.vegetation_level_spread_at = vegetation_level_spread_at;
	settings.soil_energy_max = soil_energy_max;
	settings.vegetation_level_max = vegetation_level_max;
	settings.vegetation_level_growth_req = vegetation_level_growth_req;
	settings.soil_energy_increase_turn = soil_energy_increase_turn;
	settings.vegetation_consumption = 
		malloc((settings.vegetation_level_max + 1)*sizeof(int));
	memcpy(settings.vegetation_consumption,
		vegetation_consumption, (settings.vegetation_level_max+1)*sizeof(int));

	settings.fish_level_max = fish_level_max;
	settings.fish_consumption = malloc(settings.fish_level_max*sizeof(int));
	memcpy(settings.fish_consumption, fish_consumption,
		(settings.fish_level_max + 1)*sizeof(int));
	settings.fish_growth_req = fish_growth_req;
	settings.fish_moves_turn = fish_moves_turn;
	settings.initial_fish_size = initial_fish_size;

	settings.split_fishes_at_max = split_fishes_at_max;
	settings.random_fishes_interval = random_fishes_interval;

	settings.fishing_chance = fishing_chance;

	if (ValidateSettings(settings, 1))
		return settings;
	else
		exit(EXIT_FAILURE);
}
/* Function CreateFishery()
 *  Initializes and returns fishery according to given fishery settings.
 *  
 *  settings - Settings for fishery, created with CreateSettings function.
 */
Fishery *CreateFishery(
	Fishery_Settings settings) {
	Fishery *fishery;
	Fish_Pool *fish;
	int i, pos, *pos_avail;
		
	fishery = malloc(sizeof(Fishery));
	fishery->fish_list = NULL;
	/* Vegetation tiles - reserve memory and initialize tiles. */	
	fishery->vegetation_layer = calloc(settings.size_x*settings.size_y, sizeof(Tile));
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		fishery->vegetation_layer[i].local_fish = NULL;
		fishery->vegetation_layer[i].vegetation_level = 0;
		fishery->vegetation_layer[i].soil_energy = settings.soil_energy_increase_turn;
	}
	/* Place initial vegetation randomly (using second array
	   of available positions) .*/
	pos_avail = malloc(sizeof(int)*settings.size_x*settings.size_y);
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		pos_avail[i] = i;
	}
	for (i = 0; i < settings.initial_vegetation_size; i++) {
		pos = (int) ((double) rand() / (RAND_MAX + 1)*(settings.size_x*settings.size_y-1-i));
		fishery->vegetation_layer[pos_avail[pos]].vegetation_level = 1;
		pos_avail[pos] = pos_avail[settings.size_x*settings.size_y - 1 - i];
	}
	free(pos_avail);

	/* Create initial fish population. */
	fishery->fish_list = LListCreate();
	pos_avail = malloc(sizeof(int)*settings.size_x*settings.size_y);
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		pos_avail[i] = i;
	}
	for (i = 0; i < settings.initial_fish_size; i++) {
		pos = (int)(rand() / (double) (RAND_MAX + 1)*(settings.size_x*settings.size_y - 1 - i));
		fish = malloc(sizeof(Fish_Pool));
		fish->food_level = 0;
		fish->pop_level = 1;
		fish->pos_x = pos_avail[pos] / settings.size_y;
		fish->pos_y = pos_avail[pos] % settings.size_y;
		fish->reprod_counter = 1;
		LListAdd(fishery->fish_list, fish);
		fishery->vegetation_layer[pos_avail[pos]].local_fish = fish;
		pos_avail[pos] = pos_avail[settings.size_x*settings.size_y - 1 - i];
	}
	free(pos_avail);

	CheckFishMemory(fishery, settings);

	return fishery;
}
/* Function UpdateFishery().
 * 
 * Progresses the fishery n steps using the given settings. Returns
 * the results of the simulation steps in a Fishery_Result structure. 
 * The results contain total vegetation level, total fish population 
 * and total fishing yield, as well as their standard deviations. See 
 * the Fishery_Results structure for details.
 *
 * fishery     - Initialized or progressed fishery.
 * settings    - Settings for fishery.
 * n           - Number of steps to progress the simulation.
 * 
 * results     - Results of simulation run, including total amount 
 *               of fish present as well as total fishing yield.
 */
Fishery_Results UpdateFishery(
	Fishery *fishery, Fishery_Settings settings, int n) {
	int i, j, fish_n=0, tmp_yield, tmp_fish_n, tmp_vegetation_n;
	LList_Node *node;
	Fishery_Results results;
	Fish_Pool *fish;
	
	results.vegetation_n = 0;
	results.vegetation_n_std_dev = 0.0;
	results.fish_n = 0;
	results.fish_n_std_dev = 0.0;
	results.yield = 0;
	results.yield_std_dev = 0.0;
	results.steps = n;

	results.debug_stuff = 0;
	
	if (n < 0) {
		exit(EXIT_FAILURE);
		printf("Steps to progress simulation less than zero: %d.\n", n);
	}
	for (i = 0; i < n; i++) {
		/* Update vegetation. */
		UpdateFisheryVegetation(fishery, settings);
		/* Update fish population. */
		UpdateFisheryFishPopulation(fishery, settings);
		/* Calculate fishing results and debugging info. */
		tmp_fish_n = 0;
		node = fishery->fish_list;
		while (node != NULL && node->node_value != NULL) {
			fish = node->node_value;
			tmp_fish_n += fish->pop_level;
			node = node->next;
		}
		if (tmp_fish_n == 0) {
			results.debug_stuff++;
		}
		results.fish_n += tmp_fish_n;
		results.fish_n_std_dev += tmp_fish_n*tmp_fish_n;
		if (settings.fishing_chance > 0.0) {
			tmp_yield = FishingEvent(fishery, settings);
			results.yield += tmp_yield;
			results.yield_std_dev += tmp_yield*tmp_yield;
		}
		tmp_vegetation_n = 0;
		for (j = 0; j < settings.size_x*settings.size_y; j++) {
			tmp_vegetation_n += fishery->vegetation_layer[j].vegetation_level;
		}
		results.vegetation_n += tmp_vegetation_n;
		results.vegetation_n_std_dev += tmp_vegetation_n*tmp_vegetation_n;
	}
	results.vegetation_n_std_dev = 
		sqrt(results.vegetation_n_std_dev / n - pow((double) results.vegetation_n / n, 2));
	results.fish_n_std_dev = 
		sqrt(results.fish_n_std_dev / n - pow((double) results.fish_n / n, 2));
	results.yield_std_dev = 
		sqrt(results.yield_std_dev / n - pow((double) results.yield / n, 2));

	return results;
}
/* Function UpdateFisheryVegetation().
 *
 * Increases soil energy and grows the vegetation layer as necessary.
 *
 * fishery     - Initialized or progressed fishery.
 * settings    - Settings for fishery.
 *
 */
void UpdateFisheryVegetation(
	Fishery
	*fishery, Fishery_Settings settings) {
	int i, j, k, pos_x, pos_y;
	int *vegetation_layer_growth;

	vegetation_layer_growth = calloc(settings.size_x*settings.size_y, sizeof(int));
	/* Grow vegetation layer in different array to avoid double growths. */
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		/* If tile contains vegetation. */
		if (fishery->vegetation_layer[i].vegetation_level > 0) {
			/* If enough soil energy for vegetation growth. */
			if (fishery->vegetation_layer[i].vegetation_level +
				settings.vegetation_level_growth_req <= fishery->vegetation_layer[i].soil_energy) {
				vegetation_layer_growth[i] = 1;
				fishery->vegetation_layer[i].soil_energy = /* Consume energy for growth. */
					fishery->vegetation_layer[i].soil_energy -
					fishery->vegetation_layer[i].vegetation_level
					- settings.vegetation_level_growth_req;
			}
			else { /* Consumption of soil energy to maintain vegetation level. Decrease in
				   vegetation level takes place if there is insufficient soil energy. */
				fishery->vegetation_layer[i].soil_energy =
					fishery->vegetation_layer[i].soil_energy -
					settings.vegetation_consumption[fishery->vegetation_layer[i].vegetation_level];
				if (fishery->vegetation_layer[i].soil_energy < 0)
					vegetation_layer_growth[i] = -1;
			}
		}
		/* If vegetation level is large enough, spread to neighboring tiles. */
		if (fishery->vegetation_layer[i].vegetation_level >= settings.vegetation_level_spread_at) {
			pos_y = i % settings.size_y;
			pos_x = i / settings.size_y;
			/* Spread only to valid tiles, i.e. not outside array
			and only to empty tiles. */
			for (j = -1; j <= 1; j++) {
				for (k = -1; k <= 1; k++) {
					if (pos_x + j >= 0 && pos_x + j < settings.size_x &&
						pos_y + k >= 0 && pos_y + k < settings.size_y &&
						fishery->vegetation_layer[(pos_y + k) + (pos_x + j)*settings.size_y].vegetation_level == 0) {
						vegetation_layer_growth[(pos_y + k) + (pos_x + j)*settings.size_y] = 1;
					}
				}
			}
		}
	}
	/* Add growth layer to vegetation layer. */
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		fishery->vegetation_layer[i].vegetation_level += vegetation_layer_growth[i];
		if (fishery->vegetation_layer[i].vegetation_level > settings.vegetation_level_max)
			fishery->vegetation_layer[i].vegetation_level = settings.vegetation_level_max;
	}
	/* Add soil energy. */
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		fishery->vegetation_layer[i].soil_energy += settings.soil_energy_increase_turn;
		if (fishery->vegetation_layer[i].soil_energy > settings.soil_energy_max)
			fishery->vegetation_layer[i].soil_energy = settings.soil_energy_max;
	}
	free(vegetation_layer_growth);
}
/* Function UpdateFisheryFishPopulation().
 *
 * Updates the fish population of the fishery simulation. This includes
 * growing fish pools, moving fish pools around in search of food and 
 * consuming vegetation. Also generates new fish pools.
 *
 * fishery		- Initialized or progressed fishery.
 * settings		- Settings for fishery.
 *
 */
void UpdateFisheryFishPopulation(
	Fishery *fishery, Fishery_Settings settings) {
	LList_Node *fish_node, *for_deletion = NULL, *tmp_fish_node;
	Fish_Pool *fish, *new_fish, *first_added = NULL;
	int fish_pos, avail_moves, appetite, consumed, new_pos, i, pos_avail_n, *pos_avail;
	double random_fishes_counter = settings.random_fishes_interval / 100.0;

	/* Process fish population. */
	fish_node = fishery->fish_list;
	/* Empty list will have an empty node at the beginning. */
	while (fish_node && fish_node->node_value && fish_node->node_value != first_added) { 
		fish = fish_node->node_value;
		fish_pos = fish->pos_y + fish->pos_x*settings.size_y;
		/* fish_pos = fish->pos_x + fish->pos_y*settings.size_x; */
		/* Consume food and move if needed. */
		avail_moves = settings.fish_moves_turn;
		while (avail_moves > 0 && fish->food_level < 
			settings.fish_consumption[fish->pop_level]* 2 + settings.fish_growth_req) {
			fish_pos = fish->pos_y + fish->pos_x*settings.size_y;
			/* fish_pos = fish->pos_x + fish->pos_y*settings.size_x; */
			if (fishery->vegetation_layer[fish_pos].vegetation_level == 0) {
				/* If no food at current tile, attempt to move. */
				new_pos = GetNewCoords(fish_pos, 1, settings.size_x, settings.size_y, fishery);
				if (new_pos == -1) {
					/* No move possible. */
					break;
				}
				else {
					/* Move fish pool. */
					fishery->vegetation_layer[new_pos].local_fish = fish;
					fishery->vegetation_layer[fish_pos].local_fish = NULL;
					/* fish->pos_x = new_pos % settings.size_x;
					fish->pos_y = new_pos / settings.size_y; */
					fish->pos_x = new_pos / settings.size_y;
					fish->pos_y = new_pos % settings.size_y;
				}
			}
			if (fishery->vegetation_layer[fish_pos].vegetation_level > 0) {
				/* If food at current tile. */
				/* Amount possible for fish to eat.*/
				appetite = settings.fish_consumption[fish->pop_level] * 2 +
					settings.fish_growth_req - fish->food_level; 
				/* Amount actually consumed based on available food. */
				consumed = appetite > fishery->vegetation_layer[fish_pos].
					vegetation_level ? fishery->vegetation_layer[fish_pos].
				    vegetation_level : appetite; 
				fish->food_level += consumed;
				fishery->vegetation_layer[fish_pos].vegetation_level 
					-= consumed;
			}
			avail_moves--;
		}
		/* fish_pos = fish->pos_x + fish->pos_y*settings.size_x; */
		fish_pos = fish->pos_y + fish->pos_x*settings.size_y;
		if (fish->food_level >= settings.fish_growth_req + settings.fish_consumption[fish->pop_level]) {	
			/* If enough food for growth or split present. */ 
			if (fish->pop_level < settings.fish_level_max) {
				/* Grow fish pool if not max size. */
				while (fish->food_level >= settings.fish_growth_req + 
					settings.fish_consumption[fish->pop_level] && 
					fish->pop_level < settings.fish_level_max) {
					fish->pop_level++;
					fish->food_level -= (settings.fish_growth_req + 
						settings.fish_consumption[fish->pop_level]);
				}
			}
			else {
				/* Else split fish pool. */
				new_pos = GetNewCoords(fish_pos, 1, settings.size_x, settings.size_y, fishery);
				if (new_pos != -1 && settings.split_fishes_at_max) {
					/* Position for splitting available. */
					fish->food_level -= (settings.fish_growth_req +
						settings.fish_consumption[fish->pop_level]);
					new_fish = malloc(sizeof(Fish_Pool));
					new_fish->food_level = 0;
					new_fish->pop_level = 1;
					/* fish->pos_x = new_pos % settings.size_x;
					fish->pos_y = new_pos / settings.size_y; */
					new_fish->pos_x = new_pos / settings.size_y;
					new_fish->pos_y = new_pos % settings.size_y;
					new_fish->reprod_counter = 1;
					fishery->vegetation_layer[new_pos].local_fish = new_fish;
					LListAdd(fishery->fish_list, new_fish);
					if (first_added == NULL) first_added = new_fish;
				}
				else {
					/* No position available, consume food normally. */
					fish->food_level -= settings.fish_consumption[fish->pop_level];
				}
			}
		}
		else { 
			/* Else consume food needed by fish pool population. */
			fish->food_level -= settings.fish_consumption[fish->pop_level];
			if (fish->food_level < 0) {
				fish->pop_level--;
				fish->food_level = 0;
				if (fish->pop_level <= 0) {
					for_deletion = fish_node;
				}
			}
		}
		if (for_deletion != NULL) {
			/* fish_pos = fish->pos_x + fish->pos_y*settings.size_x; */
			fish_pos = fish->pos_y + fish->pos_x*settings.size_y;
			fishery->vegetation_layer[fish_pos].local_fish = NULL;
			if (fishery->fish_list != for_deletion) {
				/* If the fish is not the first fish in the list, move pointer to next fish. */
				fish_node = fish_node->next;
			}
			fish = LListPop(fishery->fish_list, for_deletion->node_value, ComparePointers); 
			free(fish);
			for_deletion = NULL;
		}
		else {
			fish_node = fish_node->next;
		}
	}
	if (settings.random_fishes_interval) {
		if (random_fishes_counter >= rand() / ((double) RAND_MAX + 1)) {
			/* Spawn fish randomly. Start by finding 
			   available positions for fishes. */
			pos_avail = malloc(sizeof(int)*settings.size_x*settings.size_y);
			pos_avail_n = 0;
			for (i = 0; i < settings.size_x*settings.size_y; i++) {
				if (fishery->vegetation_layer[i].local_fish == NULL)
					pos_avail[pos_avail_n++] = i;
			}
			if (pos_avail_n > 0) {
				/* If there's room for a new fish. */
				new_pos = (int)(rand() / (double)(RAND_MAX + 1)*(pos_avail_n - 1));
				new_pos = pos_avail[new_pos];
				new_fish = malloc(sizeof(Fish_Pool));
				new_fish->food_level = 0;
				new_fish->pop_level = 1;
				/* new_fish->pos_x = new_pos % settings.size_x;
				new_fish->pos_y = new_pos / settings.size_y; */
				new_fish->pos_x = new_pos / settings.size_y;
				new_fish->pos_y = new_pos % settings.size_y;
				new_fish->reprod_counter = 1;
				LListAdd(fishery->fish_list, new_fish);
				fishery->vegetation_layer[new_pos].local_fish = new_fish;
			}
			random_fishes_counter = 0;
			free(pos_avail);
		}
	}
}
/* Function DestroyFishery().

Frees memory used by fishery simulation.

fishery     - Initialized or progressed fishery.
settings    - Settings for fishery.
*/
void DestroyFishery(Fishery *fishery) {
	LListDestroy(fishery->fish_list, free);
	free(fishery->vegetation_layer);
	if (fishery->settings != NULL) {
		if (fishery->settings->vegetation_consumption != NULL)
			free(fishery->settings->vegetation_consumption);
		if (fishery->settings->fish_consumption != NULL)
			free(fishery->settings->fish_consumption);
		free(fishery->settings);
	}
	free(fishery);
}
/* Function FishingEvent().
 *
 * Releases the fishing boats! Based on the fishing_chance, each
 * fish has a random chance of being fished. Returns the yield
 * of the fishing event, i.e. total amount of fish population
 * level lost.
 *
 * fishery     - Initialized or progressed fishery.
 * settings    - Settings for fishery.
 *
 * yield       - Fish population lost during event. 
 *
 */
int FishingEvent(
	Fishery *fishery, Fishery_Settings settings) {
	int yield=0, fish_pos, tot_yield=0;
	LList_Node *fish_node, *for_deletion=NULL;
	Fish_Pool *fish;

	fish_node = fishery->fish_list;
	while (fish_node != NULL && fish_node->node_value != NULL) {
		fish = fish_node->node_value;
		if (rand() / (double)(RAND_MAX + 1) <= settings.fishing_chance) {
			/*  yield = (int) round(rand() / (double)(RAND_MAX + 1) * (fish->pop_level/2+1));
			yield = (int) ceil(fish->pop_level*settings.fishing_chance); */
			/* yield = fish->pop_level; */
			yield = 1;
			fish->pop_level -= yield;
			tot_yield += yield;
			if (fish->pop_level <= 0) {		
				/* fish_pos = fish->pos_x + fish->pos_y*settings.size_x; */
				fish_pos = fish->pos_y + fish->pos_x*settings.size_y;
				for_deletion = fish_node;
				if (fishery->fish_list != fish_node) {
					/* If the fish is not the first fish in the list. */
					fish_node = fish_node->next;
				}
				free(LListPop(fishery->fish_list, for_deletion->node_value, ComparePointers));
				fishery->vegetation_layer[fish_pos].local_fish = NULL;
				for_deletion = NULL;
			}
		}
		else
			fish_node = fish_node->next;
	}
	return tot_yield;
}