// main.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "fishery_data_types.h"
#include "fishery_functions.h"
#include "fishery_tests.h"

#define RUN_TESTS 1
#define RUN_TEMP_TEST 0
#define RUN_PROGRAM 1

int main(void)
{
	Fishery_Settings settings;
	Fishery *fishery;
	Fish_Pool *fish;
	LList_Node *ptr;

	int vegetation_requirements[] = {0, 1, 1, 2, 2, 3 };
	int fish_requirements[] = { 0, 1, 2, 3, 4, 5};
	int i, j, tt = 1, k=0;

	double total_yield=0.0;

	if (RUN_TESTS == 1)
		assert(TestFisheryAll() == 1);

	if (RUN_TEMP_TEST == 1) {
		settings = CreateSettings(10, 10, 80,
			5, 3, 3, 10, 3, vegetation_requirements,
			10, 5, 3, 3, fish_requirements, 10, 1, 0.1);
		fishery = CreateFishery(settings);
		for (i = 0; i < settings.size_y; i++) {
			for (j = 0; j < settings.size_x; j++) {
				printf("%d ", fishery->vegetation_layer[i*settings.size_x + j].vegetation_level);
			}
			printf("\n");
		}
		printf("-----------\n");
		for (i = 0; i < settings.size_y; i++) {
			for (j = 0; j < settings.size_x; j++) {
				if (fishery->vegetation_layer[i*settings.size_x + j].local_fish)
					printf("%d ", fishery->vegetation_layer[i*settings.size_x + j].local_fish->pop_level);
				else
					printf("0 ");
			}
			printf("\n");
		}
		printf("-----------\n");
		GetNewCoords(12, 1, settings.size_x, settings.size_y, fishery);
		for (i = 0; i < 1000000; i++) {
			j = (int)(((double)rand()) / (RAND_MAX+1)*(10));
			if (j == 10)
				k++;
		}
		printf("%d\n", k);
	}

	if (RUN_PROGRAM == 1) {

		settings = CreateSettings(10, 10,
			80, 5, 3, 3, 10, 3, vegetation_requirements,
			10, 5, 3, 3, fish_requirements, 10, 1, 0.15);
		printf("Settings validated and created!\n");
		printf("---------------------\n");
		fishery = CreateFishery(settings);
		printf("Fishery validated and created!\n");
		printf("---------------------\n");
		if (fishery->fish_list) {
			ptr = fishery->fish_list;
			fish = ptr->node_value;
			while (ptr != NULL && fish != NULL) {
				fish = ptr->node_value;
				printf("Pos: %d %d\n", fish->pos_x, fish->pos_y);
				ptr = ptr->next;
			}
		}

		while (tt) {
			if (fishery) {
				ptr = fishery->fish_list;
				i = 0;
				while (ptr != NULL && ptr->node_value != NULL) {
					i++;
					ptr = ptr->next;
				}
				printf("%d fishes in simulation.\n", i);
			}
			if (fishery) {
				for (i = 0; i < settings.size_y; i++) {
					for (j = 0; j < settings.size_x; j++) {
						printf("%d ", fishery->vegetation_layer[i*settings.size_x + j].vegetation_level);
					}
					printf("\n");
				}
				printf("-----------\n");
			}
			if (fishery) {
				for (i = 0; i < settings.size_y; i++) {
					for (j = 0; j < settings.size_x; j++) {
						if (fishery->vegetation_layer[i*settings.size_x + j].local_fish) {
							printf("%d ", fishery->vegetation_layer[i*settings.size_x + j].local_fish->pop_level);
						}
						else
							printf("0 ");
					}
					printf("\n");
				}
			}
			printf("-----------\n");
			scanf("%d", &tt);
			for (j = 0; j < tt; j++){
				for (i = 0; i < 1; i++) {
					UpdateFishery(fishery, settings, 1);
					total_yield += FishingEvent(fishery, settings);
				}
			}
			printf("Yield was: %f\n", total_yield / tt);
			total_yield = 0.0;
		}
	}
	return EXIT_SUCCESS;
}

