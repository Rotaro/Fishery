/*****************************************************************************
 * Filename: fishery_settings.c												 *
 *																			 *
 * Contains functions for creating and manipulating fishery simulation       *
 * settings.																 *
 *																			 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "fishery_settings.h"

 /* SETTING_ORDER, SETTINGS_SIZE and LIST_INDEXES_SIZES are used to parse
 * settings in the python module. */
const char *SETTING_ORDER[] = { "size_x", "size_y",
"initial_vegetation_size", "vegetation_level_max", "vegetation_level_spread_at",
"vegetation_level_growth_req", "soil_energy_max", "soil_energy_increase_turn",
"vegetation_consumption",
"initial_fish_size", "fish_level_max", "fish_growth_req", "fish_moves_turn",
"fish_consumption", "random_fishes_interval", "split_fishes_at_max", "fishing_chance" };
const int SETTINGS_SIZE = 17;
const int LIST_INDEXES_SIZES[] = { 8, 3, 13, 10 };

/* List of all settings and their types. Third column is the settings which 
   determines the size of the list setting.*/
const char *MASTER_SETTING_LIST[17][3] = {
	{ "size_x",							"int",	""						},
	{ "size_y",							"int",	""						},
	{ "initial_vegetation_size",		"int",	""						},
	{ "vegetation_level_max",			"int",	""						},
	{ "vegetation_level_spread_at",		"int",	""						},
	{ "vegetation_level_growth_req",	"int",	""						},
	{ "soil_energy_max",				"int",	""						},
	{ "soil_energy_increase_turn",		"int",	""						},
	{ "vegetation_consumption",			"list",	"vegetation_level_max"	},
	{ "initial_fish_size",				"int",	""						},
	{ "fish_level_max",					"int",	""						},
	{ "fish_growth_req",				"int",	""						},
	{ "fish_moves_turn",				"int",	""						},
	{ "fish_consumption",				"list",	"fish_level_max"		},
	{ "random_fishes_interval",			"int",	""						},
	{ "split_fishes_at_max",			"int",	""						},
	{ "fishing_chance",					"int",	""						}
};

/* Function: CreateSettings
 * ------------------------
 * Creates Fishery_Settings data structure from parameters.
 *
 * See documentation for information regarding settings/parameters.
 *
 * Returns: Initialized Fishery_Settings data structure.
 */
Fishery_Settings CreateSettings(
	int size_x, int size_y, int initial_vegetation_size,
	int vegetation_level_max, int vegetation_level_spread_at,
	int vegetation_level_growth_req, int soil_energy_max,
	int soil_energy_increase_turn, int *vegetation_consumption,
	int initial_fish_size, int fish_level_max, int fish_growth_req, int fish_moves_turn,
	int *fish_consumption,
	int random_fishes_interval, int split_fishes_at_max,
	int fishing_chance) {

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
		vegetation_consumption, (settings.vegetation_level_max + 1)*sizeof(int));

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

	return settings;
}

 /* Function: ValidateSettings
  * -------------------------- 
  * Validates fishery settings, i.e. checks for impossible values
  * which would break the simulation. Returns 1 on success, 0
  * if invalid settings.
  *
  * settings:		Settings to be validated.
  * output_print:	If 1, prints output regarding invalid settings.
  *					If 0, no output is printed.
  *
  * Returns:		Integer representing validity of settings. 1 if valid,
  *					0 if invalid.
  */
int ValidateSettings(
	Fishery_Settings settings, int output_print) {
	int settings_valid = 1, i;

	if (settings.size_x <= 0 || settings.size_x > 1000) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("size_x is invalid (%d).\n", settings.size_x);
	}
	if (settings.size_y <= 0 || settings.size_y > 1000) {
		settings_valid = 0;
		if (output_print == 1) 
			printf("size_y is invalid (%d).\n", settings.size_y);
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
	for (i = 0; i < settings.vegetation_level_max + 1; i++) {
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
			printf("soil_energy_max is invalid (%d).\n", 
				settings.soil_energy_max);
	}

	if (settings.initial_fish_size < 0 ||
		settings.initial_fish_size > settings.size_x*settings.size_y) {
		settings_valid = 0;
		if (output_print == 1)
			printf("initial_fish_size is invalid (%d).\n", 
				settings.initial_fish_size);
	}
	if (settings.fish_growth_req < 0 || settings.fish_growth_req > 100) {
		settings_valid = 0;
		if (output_print == 1)
			printf("fish_growth_req is invalid (%d).\n", 
				settings.fish_growth_req);
	}
	if (settings.fish_level_max < 0 || settings.fish_level_max > 100) {
		settings_valid = 0;
		if (output_print == 1)
			printf("fish_level_max is invalid (%d).\n", 
				settings.fish_level_max);
	}
	if (settings.fish_moves_turn < 0 || settings.fish_moves_turn > 100) {
		settings_valid = 0;
		if (output_print == 1)
			printf("fish_moves_turn is invalid (%d).\n", 
				settings.fish_moves_turn);
	}
	for (i = 0; i < settings.fish_level_max + 1; i++) {
		if (settings.fish_consumption[i] < 0) {
			settings_valid = 0;
			if (output_print == 1)
				printf("fish_consumption is invalid (%d).\n", 
					settings.fish_consumption[i]);
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
	if (settings.fishing_chance < 0 || settings.fishing_chance > 100) {
		settings_valid = 0;
		if (output_print == 1)
			printf("fishing_chance is invalid (%d).\n", 
				settings.fishing_chance);
	}

	return settings_valid;
}
/* Function: PrintSettings
 * -----------------------
 * Prints settings, only used for debugging.
 *
 * settings:	Settings to be printed.
 */
void PrintSettings(
	Fishery_Settings settings) {
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
	printf("%s: %d\n", "fishing_chance", settings.fishing_chance);
}
/* Function: AddSetting
 * --------------------
 * Adds single setting value to Fishery_Settings data structure.
 *
 * setting:			Fishery_Settings data type setting is added to.
 * *setting_name:	Name of setting to add value to.
 * *setting_value:	Value to add to setting.
 *
 * Returns:			Integer representing success of function. 1 if setting was
 *					added successfully, 0 if failed.
 */
int AddSetting(
	Fishery_Settings *settings, const char *setting_name, void *setting_value) {
	int success = 0;

	if (strcmp(setting_name, "size_x") == 0) {
		settings->size_x = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "size_y") == 0) {
		settings->size_y = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "initial_vegetation_size") == 0) {
		settings->initial_vegetation_size = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "vegetation_level_max") == 0) {
		settings->vegetation_level_max = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "vegetation_level_spread_at") == 0) {
		settings->vegetation_level_spread_at = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "vegetation_level_growth_req") == 0) {
		settings->vegetation_level_growth_req = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "soil_energy_max") == 0) {
		settings->soil_energy_max = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "soil_energy_increase_turn") == 0) {
		settings->soil_energy_increase_turn = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "vegetation_consumption") == 0 &&
		settings->vegetation_level_max) {
		settings->vegetation_consumption =
			malloc(sizeof(int)*(settings->vegetation_level_max + 1));
		if (settings->vegetation_consumption != NULL) {
			memcpy(settings->vegetation_consumption, setting_value,
				sizeof(int)*(settings->vegetation_level_max + 1));
			success = 1;
		}
	}
	else if (strcmp(setting_name, "initial_fish_size") == 0) {
		settings->initial_fish_size = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "fish_level_max") == 0) {
		settings->fish_level_max = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "fish_growth_req") == 0) {
		settings->fish_growth_req = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "fish_moves_turn") == 0) {
		settings->fish_moves_turn = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "fish_consumption") == 0 && 
		settings->fish_level_max >= 0) {
		settings->fish_consumption =
				malloc(sizeof(int)*(settings->fish_level_max + 1));
		if (settings->fish_consumption != NULL) {
			memcpy(settings->fish_consumption, setting_value,
				sizeof(int)*(settings->fish_level_max + 1));
			success = 1;
		}
	}
	else if (strcmp(setting_name, "random_fishes_interval") == 0) {
		settings->random_fishes_interval = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "split_fishes_at_max") == 0) {
		settings->split_fishes_at_max = *((int *)setting_value);
		success = 1;
	}
	else if (strcmp(setting_name, "fishing_chance") == 0) {
		settings->fishing_chance = *((int *)setting_value);
		success = 1;
	}

	return success;
}