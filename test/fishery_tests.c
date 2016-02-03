/*
fishery_tests.c

Contains functions for testing different properties of the fishery simulation.

*/

#include <assert.h>
#include <stdio.h>
#include "fishery_tests.h"

/* Function TestFisheryAll()

Runs all the available tests for the Fishery simulation, including tests for help functions.
The tests generate their own settings for testing. Outputs 1 if all tests pass (also
information to the standard output regarding the progress of tests). Will terminate on assert if
a test fails.

*/
int TestFisheryAll() {
	printf("-----------\n");
	printf("Testing all Fishery functions (including help functions).\n");
	assert(TestLList() == 1);
	printf("-----------\n");
	printf("Testing Fishery functions.\n");
	printf("-----------\n");
	TestFisherySettings();
	TestInitialFishery();
	TestAddSettings();
	TestGetNewCoords();
	printf("-----------\n");
	printf("All tests passed.\n");
	printf("-----------\n");
	return 1;
}
int TestFisherySettings() {
	Fishery_Settings settings;
	int consumption[] = { 0, 1, 1, 2, 2, 3 };
	int fish_consumption[] = { 0, 1, 2, 3, 4, 5 };

	/* Reasonable, valid settings. Tests will alter one setting at a time. */
	settings.size_x = 10;
	settings.size_y = 10;
	settings.initial_vegetation_size = 10;
	settings.vegetation_level_max = 5;
	settings.vegetation_level_spread_at = 3;
	settings.vegetation_level_growth_req = 3;
	settings.vegetation_consumption = consumption;
	settings.soil_energy_increase_turn = 3;
	settings.soil_energy_max = 10;
	settings.initial_fish_size = 10;
	settings.fish_growth_req = 2;
	settings.fish_level_max = 5;
	settings.fish_moves_turn = 3;
	settings.fish_consumption = fish_consumption;
	settings.split_fishes_at_max = 1;
	settings.random_fishes_interval = 10;
	settings.fishing_chance = 10;

	printf("Testing ValidateSettings()!\n");
	assert(ValidateSettings(settings, 0));

	settings.size_x = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.size_x = 0;
	assert(ValidateSettings(settings, 0) == 0);
	settings.size_x = 10;

	settings.size_y = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.size_y = 0;
	assert(ValidateSettings(settings, 0) == 0);
	settings.size_y = 10;

	settings.initial_vegetation_size = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.initial_vegetation_size = 10;

	settings.vegetation_level_max = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.vegetation_level_max = 5;

	settings.vegetation_level_spread_at = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.vegetation_level_spread_at = 3;

	settings.vegetation_level_growth_req = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.vegetation_level_growth_req = 3;

	settings.vegetation_consumption[2] = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.vegetation_consumption[2] = -1;

	settings.soil_energy_increase_turn = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.soil_energy_increase_turn = 3;

	settings.soil_energy_max = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.soil_energy_max = 10;

	settings.initial_fish_size = -1;
	assert(ValidateSettings(settings, 0) == 0);
	settings.initial_fish_size = 10;

	printf("Test passed.\n");
	return 1;
}
int TestAddSettings(void) {
	Fishery_Settings settings;
	int i, test_val;
	int consumption[] = { 0, 1, 1, 2, 2, 3 };
	int fish_consumption[] = { 0, 1, 2, 3, 4, 5 };

	printf("Testing AddSetting()!\n");
	/* Test settings are properly added. */
	test_val = 10;
	assert(AddSetting(&settings, "size_x", &test_val) == 1);
	assert(settings.size_x == test_val);
	test_val = 10;
	assert(AddSetting(&settings, "size_y", &test_val) == 1);
	assert(settings.size_y == test_val);
	test_val = 80;
	assert(AddSetting(&settings, "initial_vegetation_size", &test_val) == 1);
	assert(settings.initial_vegetation_size == test_val);
	test_val = 5;
	assert(AddSetting(&settings, "vegetation_level_max", &test_val) == 1);
	assert(settings.vegetation_level_max == test_val);
	test_val = 3;
	assert(AddSetting(&settings, "vegetation_level_spread_at", &test_val) == 1);
	assert(settings.vegetation_level_spread_at == test_val);
	test_val = 2;
	assert(AddSetting(&settings, "vegetation_level_growth_req", &test_val) == 1);
	assert(settings.vegetation_level_growth_req == test_val);
	test_val = 10;
	assert(AddSetting(&settings, "soil_energy_max", &test_val) == 1);
	assert(settings.soil_energy_max == test_val);
	test_val = 3;
	assert(AddSetting(&settings, "soil_energy_increase_turn", &test_val) == 1);
	assert(settings.soil_energy_increase_turn == test_val);
	assert(AddSetting(&settings, "vegetation_consumption", consumption) == 1);
	for (i = 0; i < 6; i++)
		assert(settings.vegetation_consumption[i] == consumption[i]);
	test_val = 10;
	assert(AddSetting(&settings, "initial_fish_size", &test_val) == 1);
	assert(settings.initial_fish_size == test_val);
	test_val = 5;
	assert(AddSetting(&settings, "fish_level_max", &test_val) == 1);
	assert(settings.fish_level_max == test_val);
	test_val = 1;
	assert(AddSetting(&settings, "fish_growth_req", &test_val) == 1);
	assert(settings.fish_growth_req == test_val);
	test_val = 3;
	assert(AddSetting(&settings, "fish_moves_turn", &test_val) == 1);
	assert(settings.fish_moves_turn == test_val);
	assert(AddSetting(&settings, "fish_consumption", fish_consumption) == 1);
	for (i = 0; i < 6; i++)
		assert(settings.fish_consumption[i] == fish_consumption[i]);
	test_val = 3;
	assert(AddSetting(&settings, "random_fishes_interval", &test_val) == 1);
	assert(settings.random_fishes_interval == test_val);
	test_val = 3;
	assert(AddSetting(&settings, "split_fishes_at_max", &test_val) == 1);
	assert(settings.split_fishes_at_max == test_val);
	test_val = 30;
	assert(AddSetting(&settings, "fishing_chance", &test_val) == 1);
	assert(settings.fishing_chance == test_val);

	printf("Test passed.\n");
	return 1;
}
int TestInitialFishery(void) {
	Fishery_Settings settings;
	Fishery *fishery;
	LList_Node *fish_node;
	Fish_Pool *fish;
	int fishes_found_in_vege = 0, fishes_found_in_list=0, vegetation_found = 0;
	int i;
	int consumption[] = { 0, 1, 1, 2, 2, 3 };
	int fish_consumption[] = { 0, 1, 2, 3, 4, 5 };

	/* Reasonable, valid settings. Tests will alter one setting at a time. */
	settings.size_x = 10;
	settings.size_y = 10;
	settings.initial_vegetation_size = 10;
	settings.vegetation_level_max = 5;
	settings.vegetation_level_spread_at = 3;
	settings.vegetation_level_growth_req = 3;
	settings.vegetation_consumption = consumption;
	settings.soil_energy_increase_turn = 3;
	settings.soil_energy_max = 10;

	settings.initial_fish_size = 10;
	settings.fish_growth_req = 2;
	settings.fish_level_max = 5;
	settings.fish_moves_turn = 3;
	settings.fish_consumption = fish_consumption;

	settings.split_fishes_at_max = 1;
	settings.random_fishes_interval = 10;

	settings.fishing_chance = 10;
	

	printf("Testing CreateFishery()!\n");

	fishery = CreateFishery(settings);
	
	for (i = 0; i < settings.size_x*settings.size_y; i++) {
		assert(fishery->vegetation_layer[i].soil_energy == settings.soil_energy_increase_turn);
		if (fishery->vegetation_layer[i].vegetation_level > 0)
			vegetation_found++;
		if (fishery->vegetation_layer[i].local_fish)
			fishes_found_in_vege++;;
	}
	assert(vegetation_found == settings.initial_vegetation_size);

	fish_node = fishery->fish_list;
	while (fish_node) {
		if (fish_node->node_value) {
			fishes_found_in_list++;
			fish = fish_node->node_value;
			/* assert(fish == fishery->vegetation_layer[fish->pos_x + fish->pos_y*settings.size_y].local_fish); */
			assert(fish == fishery->vegetation_layer[fish->pos_y + fish->pos_x*settings.size_y].local_fish);
		}
		fish_node = fish_node->next;
	}

	assert(fishes_found_in_vege == settings.initial_fish_size);
	assert(fishes_found_in_list == settings.initial_fish_size);

	printf("Test passed.\n");
	return 1;
}
int TestGetNewCoords(void) {
	Fishery_Settings settings;
	Fishery *fishery;
	Fish_Pool fish1, fish2, fish3, fish4, fish5, fish6, fish7, fish8, fish9;
	int consumption[] = { 0, 1, 1, 2, 2, 3 };
	int fish_consumption[] = { 0, 1, 2, 3, 4, 5};

	/* Reasonable, valid settings. */
	settings.size_x = 10;
	settings.size_y = 10;
	settings.initial_vegetation_size = 10;
	settings.vegetation_level_max = 5;
	settings.vegetation_level_spread_at = 3;
	settings.vegetation_level_growth_req = 3;
	settings.vegetation_consumption = consumption;
	settings.soil_energy_increase_turn = 3;
	settings.soil_energy_max = 10;

	settings.initial_fish_size = 0;
	settings.fish_growth_req = 2;
	settings.fish_level_max = 5;
	settings.fish_moves_turn = 3;
	settings.fish_consumption = fish_consumption;

	settings.split_fishes_at_max = 1;
	settings.random_fishes_interval = 10;

	settings.fishing_chance = 10;

	printf("Testing GetNewCoords()!\n");
	fishery = CreateFishery(settings);

	fishery->vegetation_layer[25].local_fish = &fish1;
	fishery->vegetation_layer[26].local_fish = &fish2;
	fishery->vegetation_layer[27].local_fish = &fish3;
	fishery->vegetation_layer[35].local_fish = &fish4;
	fishery->vegetation_layer[36].local_fish = &fish5;
	fishery->vegetation_layer[37].local_fish = &fish6;
	fishery->vegetation_layer[45].local_fish = &fish7;
	fishery->vegetation_layer[46].local_fish = &fish8;
	fishery->vegetation_layer[47].local_fish = &fish9;
	
	assert(GetNewCoords(26, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(27, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(35, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(36, 1, 10, 10, fishery) == -1);
	assert(GetNewCoords(37, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(45, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(46, 1, 10, 10, fishery) != -1);
	assert(GetNewCoords(47, 1, 10, 10, fishery) != -1);
	printf("Test passed.\n");
	return 1;
}
