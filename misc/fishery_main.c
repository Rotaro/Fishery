// main.c : Various ad-hoc tests to check program.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "fishery_data_types.h"
#include "fishery_functions.h"
#include "fishery_tests.h"
#include "help_functions.h"

#define RUN_TESTS 1
#define RUN_TEMP_TEST 0
#define RUN_PROGRAM 1

int main(void)
{
	Fishery_Settings settings;
	Fishery *fishery;
	Fishery_Results results;
	Fish_Pool *fish;
	LList_Node *ptr;

	time_t start, end;
	int vegetation_requirements[] = {0, 1, 1, 2, 2, 3 };
	int fish_requirements[] = { 0, 1, 2, 3, 4, 5};
	int i, j, tt = 1, range, times;

	int *randoms1, *randoms2, *randoms3;

	srand((unsigned int)time(NULL));

	if (RUN_TESTS == 1)
		assert(TestFisheryAll() == 1);

	if (RUN_TEMP_TEST == 1) {
		range = 20;
		times = 1000000;
		randoms1 = (int *) calloc(range + 1, sizeof(int));
		randoms2 = (int *) calloc(range + 1, sizeof(int));
		randoms3 = (int *) calloc(range + 1, sizeof(int));

		
		// test different ways of generating random numbers
		time(&start);
		for (i = 0; i < times; i++) {
			j = (int) (double)rand() / (RAND_MAX + 1L) * (range+1);
			randoms1[j]++;
		}
		time(&end);
		printf("First generation: %f second(s).\n", difftime(end, start));
		time(&start);
		for (i = 0; i < times; i++) {
			j = rand() % (range+1);
			randoms2[j]++;
		}
		time(&end);
		printf("Second generation: %f second(s).\n", difftime(end, start));
		time(&start);
		for (i = 0; i < times; i++) {
			j = GENERATERANDINT(5, range);
			randoms3[j]++;
		}
		time(&end);
		printf("Third generation: %f second(s).\n", difftime(end, start));
		for (i = 0; i < range+1; i++) {
			printf("%d\t%d\n", i, randoms1[i]);
		}
		for (i = 0; i < range + 1; i++) {
			printf("%d\t%d\n", i, randoms2[i]);
		}
		for (i = 0; i < range + 1; i++) {
			printf("%d\t%d\n", i, randoms3[i]);
		}

	}

	if (RUN_PROGRAM == 1) {

		settings = CreateSettings(10, 10,
			80, 5, 3, 3, 10, 3, vegetation_requirements,
			10, 5, 1, 5, fish_requirements, 50, 1, 10);
		printf("Settings validated and created!!\n");
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
						printf("%d ", fishery->vegetation_layer[i + j*settings.size_y].vegetation_level);
					}
					printf("\n");
				}
				printf("-----------\n");
			}
			if (fishery) {
				for (i = 0; i < settings.size_y; i++) {
					for (j = 0; j < settings.size_x; j++) {
						if (fishery->vegetation_layer[i + j*settings.size_y].local_fish) {
							printf("%d ", fishery->vegetation_layer[i + j*settings.size_y].local_fish->pop_level);
						}
						else
							printf("0 ");
					}
					printf("\n");
				}
			}
			printf("-----------\n");
			scanf("%d", &tt);
			for (i = 0; i < 100; i++) {
				fishery = CreateFishery(settings);
				results = UpdateFishery(fishery, settings, tt);
				printf("[%d, %d, %d],\n", results.fish_n, results.yield, results.debug_stuff);
			}
			printf("Yield was: %f (%f)\n", (double) results.yield / tt, results.yield_std_dev);
			printf("Fish pop was: %f (%f)\n", (double)results.fish_n / tt, results.fish_n_std_dev);
			printf("Vegetation level was: %f (%f)\n", (double)results.vegetation_n / tt, results.vegetation_n_std_dev);
		}
	}
	return EXIT_SUCCESS;
}

