/*****************************************************************************
 * Filename: fishery_py_module.c											 *
 *																			 *
 * Contains the interface functions for the Python extension of the fishery  *
 * simulation.																 *
 * Fishery simulations created by these functions are assigned unique IDs	 *
 * and stored in a global linked list. The interface functions uses these    *
 * IDs to manipulate simulations further.									 *
 *****************************************************************************/
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fishery_functions.h"
#include "fishery_settings.h"


/*
The simulation(s) are stored, with a unique ID, in a global variable to 
avoid constant transfer of simulation data between python and c extension. 
*/

LList_Node *fishery_llist;		/* Fishery storage.*/
unsigned int fishery_id_n = 0;	/* Fishery unique ID generated from this. 
							       The Python module should be reset from 
								   Python before any possible overflow.   */

extern char *MASTER_SETTING_LIST[17][3];   /* List of setting names in the proper order.
											  Accessed from fishery_settings.c. */
extern int SETTINGS_SIZE;		/* Number of settings. Accessed from 
								   fishery_settings.c. */

/* Function: MPyCreateFishery
 * --------------------------
 * Initializes setting and simulation variables for the provided settings. These are 
 * assigned a unique ID which is used to keep track of the simulation.
 *
 * *args:	Dictionary of settings. See documentation for details.
 *
 * Returns:	Python integer representing fishery id.
 */
static PyObject *MPyCreateFishery(PyObject *self, PyObject *args) {
	int i, j, list_len, *tmp_list, item;
	PyObject *dict, *list_item;
	Fishery_Settings *settings;
	Fishery *fishery;

	/* Check provided parameter is a dictionary and that it contains all 
	   settings. Errors are raised as TypeError and KeyError exceptions. */
	if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict))
		return NULL;
	for (i = 0; i < SETTINGS_SIZE; i++) {
		if (PyDict_Contains(dict, PyUnicode_FromString(MASTER_SETTING_LIST[i][0])) != 1) {
			PyErr_Format(PyExc_KeyError, MASTER_SETTING_LIST[i][0]);
			return NULL;
		}
	}
	/* Parse given settings and store in Fishery_Settings data type. */
	settings = (Fishery_Settings*)malloc(sizeof(Fishery_Settings));
	for (i = 0; i < SETTINGS_SIZE; i++) {
		if (strcmp(MASTER_SETTING_LIST[i][1], "int") == 0) {
			/* If setting is not a list. */
			item = (int)PyLong_AsLong(
				PyDict_GetItemString(dict, MASTER_SETTING_LIST[i][0]));
			AddSetting(settings, MASTER_SETTING_LIST[i][0], &item);
		}
		else {
			/* If setting is a list. */
			list_item = PyDict_GetItemString(dict, MASTER_SETTING_LIST[i][0]);
			if (PyList_Check(list_item) == 0)
				return PyErr_Format(PyExc_TypeError, "Not a list.");
			/* Length of setting list. */
			list_len = (int)PyLong_AsLong(
				PyDict_GetItemString(dict, MASTER_SETTING_LIST[i][2])) + 1;
			/* Copy into temporary C array. */
			tmp_list = (int *)malloc(sizeof(int)*list_len);
			for (j = 0; j < list_len; j++)
				tmp_list[j] = (int)
					PyLong_AsLong(PyList_GetItem(list_item, (Py_ssize_t)j));
			/* Add to Fishery_Setting data structure. */
			AddSetting(settings, MASTER_SETTING_LIST[i][0], tmp_list);
			/* Remember to free temporary array memory. */ 
			free(tmp_list);
		}
	}
	/* Create fishery simulation. */
	fishery = CreateFishery(*settings);
	fishery->fishery_id = fishery_id_n;
	fishery->settings = settings;
		
	/* Store simulation in the linked list of simulations. */
	if (fishery_llist == NULL)
		fishery_llist = LListCreate();
	LListAdd(fishery_llist, fishery);
	
	return Py_BuildValue("i", fishery_id_n++);
}
/* Function: MPyGetFisherySettingOrder
 * -----------------------------------
 * Returns the order which settings are to be entered in for the CreateSettings
 * function. The order is dedfined in defined in the file fishery_functions.c.
 *
 * Returns: Python list of strings.
 */
PyObject *MPyGetFisherySettingOrder(PyObject *self) {
	PyObject *py_setting_order, *item;
	int i;
	
	py_setting_order = PyList_New(SETTINGS_SIZE);
	if (!py_setting_order) /* PyList_New sets exception. */
		return NULL;
	for (i = 0; i < SETTINGS_SIZE; i++) {
		item = Py_BuildValue("s", MASTER_SETTING_LIST[i][0]);
		if (PyList_SetItem(py_setting_order, i, item) == -1) {
			/* Steals reference, no need to worry about item. */
			Py_DECREF(py_setting_order);
			return NULL;
		}
	}
	return py_setting_order;
}
/* Function: MPySetRNGSeed
 * -----------------------
 * Sets the seed for the random number generator. 
 *
 * *args:	Positive python integer to set RNG with. 
 *          If -1, current time is used.
 * 
 * Returns: Python integer representing success. 1 if 
 *          successful, 0 otherwise.
 *         
 */
PyObject *MPySetRNGSeed(PyObject *self, PyObject *args) {
	int py_seed, success = 0;

	if (!PyArg_ParseTuple(args, "i", &py_seed))
		return NULL;
	if (py_seed == -1) {
		srand((unsigned int)time(NULL));
		success = 1;
	}
	else {
		srand((unsigned int)py_seed);
		success = 1;
	}

	return Py_BuildValue("i", success);
}
/* Function: MPyGetFisheryVegetation
 * ---------------------------------
 * Returns vegetation layer as a Python list.
 *
 * *args:	Python integer representing simulation ID.
 *
 * Returns:	Python list of integers. Each element is a vegetation tile: element index
 *          is the position of the tile, element value is the tile vegetation level.
*/
PyObject *MPyGetFisheryVegetation(PyObject *self, PyObject *args) {
	PyObject *py_vegetation_list, *item;
	Fishery *fishery=NULL;
	int i, fishery_id;
	
	/* Find fishery with provided ID. */
	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;
	fishery = LListSearch(fishery_llist, &fishery_id, CompareFisheries);
	if (fishery == NULL) {
		PyErr_Format(PyExc_KeyError, "Fishery with ID %d not found.\n", fishery_id);
		return NULL;
	}

	py_vegetation_list = PyList_New(fishery->settings->size_x*fishery->settings->size_y);
	if (!py_vegetation_list)
		return NULL;
	for (i = 0; i < fishery->settings->size_x*fishery->settings->size_y; i++) {
		item = PyLong_FromLong(fishery->vegetation_layer[i].vegetation_level);
		/* Rotate coordinates. */
		if (PyList_SetItem(py_vegetation_list, (i / fishery->settings->size_y) +
			(i % fishery->settings->size_y)*fishery->settings->size_x, item) == -1) {
			Py_DECREF(py_vegetation_list);
			return NULL;
		}
	}
	return py_vegetation_list;
}
/* Function: MPyGetFisheryFishPopulation
 * -------------------------------------
 * Returns the fish population of the simulation.
 * Each fish is represented by its position in the vegetation layer and
 * its population level.
 * 
 * *args:	Python integer representing simulation ID.
 *
 * Returns: Python list of lists of integers, i.e. [[pos, pop],...]. 
*/
PyObject *MPyGetFisheryFishPopulation(PyObject *self, PyObject *args) {
	PyObject *py_fish_list, *py_fish;
	Fish_Pool *fish_ptr;
	LList_Node *node;
	Fishery *fishery = NULL;
	int i, fish_pos, fish_population_size = 0, fishery_id, no_error=1;

	/* Find fishery with provided ID. */
	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;
	fishery = LListSearch(fishery_llist, &fishery_id, CompareFisheries);
	if (fishery == NULL) {
		PyErr_Format(PyExc_KeyError, "Fishery with ID %d not found.\n", fishery_id);
		return NULL;
	}

	/* Find fish population size. */
	node = fishery->fish_list;
	while (node != NULL && node->node_value != NULL) {
		node = node->next;
		fish_population_size++;
	}
	/* Create python list of fish population. */
	if (fish_population_size > 0) {
		py_fish_list = PyList_New(fish_population_size);
		if (!py_fish_list)
			goto error;
		node = fishery->fish_list;
		for (i = 0; i < fish_population_size; i++) {
			/* Fish in python will contain position and population level. */
			fish_ptr = node->node_value;
			py_fish = PyList_New(2);
			if (!py_fish)
				goto error;
			/* fish position returned here is for the rotated coordinate system,
			not the array structure of the c program. */
			fish_pos = fish_ptr->pos_x + fish_ptr->pos_y*fishery->settings->size_x;
			if(PyList_SetItem(py_fish, 0, PyLong_FromLong(fish_pos)) == -1 ||
				PyList_SetItem(py_fish, 1, PyLong_FromLong(fish_ptr->pop_level)) == -1)
				goto error;
			if (PyList_SetItem(py_fish_list, i, py_fish) == -1) 
				goto error;
			node = node->next;
		}
	}
	else {
		py_fish_list = PyList_New(1);
		if (!py_fish_list)
			goto error;
		if (PyList_SetItem(py_fish_list, 0, PyLong_FromLong(-1)) == -1)
			goto error;
	}
	no_error = 1;

	error:
	if (!no_error) {
		Py_XDECREF(py_fish);
		Py_XDECREF(py_fish_list);
		return NULL;
	}

	return py_fish_list;
}
/* Function: MPyUpdateFishery
 * --------------------------
 * Progresses the fishery simulation n steps. The results of the simulation are
 * reported with the following information:
 *
 * number of fish encountered, fishing yield, vegetation level encountered,
 * std dev of fish encountered, std dev of fishing yield, 
 * std dev of vegetation level encountered, steps progressed, 
 * debugging variable, fishing chance of simulation
 *
 * The last three are numbers are included simply to make debugging easier. 
 * To be removed later on.
 *
 * *args:	Two Python integers. The first is the simulation ID and
 * 			second the amount of steps to progress the simulation.
 *
 * Returns:	Results of the simulation update as a Python list of numerics.
*/
PyObject *MPyUpdateFishery(PyObject *self, PyObject *args) {
	int n, fishery_id;
	Fishery_Results results;
	Fishery *fishery;
	PyObject *results_py;

	if (!PyArg_ParseTuple(args, "ii", &fishery_id, &n))
		return NULL;
	/* Check number of steps to progress simulation is 
	   reasonable. */
	if (n < 0 || n > 100000) {
		PyErr_Format(PyExc_ValueError, "Amount of steps invalid (%d). \
			Should be larger than 0 and smaller than 100000.\n", n);
		return NULL;
	}
	/* Find correct fishery. */
	fishery = LListSearch(fishery_llist, &fishery_id, CompareFisheries);
	if (fishery == NULL) {
		PyErr_Format(PyExc_KeyError, "Fishery with ID %d not found.\n", fishery_id);
		return NULL;
	}
	/* Update fishery and save results in Python data types. */
	results = UpdateFishery(fishery, (*(fishery->settings)), n);
	results_py = Py_BuildValue("[iiidddiii]", 
		results.fish_n, results.yield, results.vegetation_n, 
		results.fish_n_std_dev, results.yield_std_dev, results.vegetation_n_std_dev, 
		results.steps, results.debug_stuff, fishery->settings->fishing_chance);
	if (!results_py)
		return NULL;
	
	return results_py;
}
/* Function: MPyDestroyFishery
 * ---------------------------
 * Frees memory used by simulation(s).
 *
 * *args:	Python integer representing simulation ID. If the ID
 *			is -1, all simulations are removed.
 *
 * Returns:	Python integer representing success of simulation removal.
 *			1 if simulation(s) were successfully removed, 0 otherwise.
*/
PyObject *MPyDestroyFishery(PyObject *self, PyObject *args) {
	int fishery_id;
	Fishery *fishery;

	
	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;

	if (fishery_id == -1) {
		/* Destroy all simulation(s). */
		LListDestroy(fishery_llist, DestroyFishery);
	}
	else {
		/* Find fishery with provided ID. */
		fishery = LListSearch(fishery_llist, &fishery_id, CompareFisheries);
		if (fishery == NULL) {
			PyErr_Format(PyExc_KeyError, "Fishery with ID %d not found.\n", fishery_id);
			return NULL;
		}
		LListPop(fishery_llist, fishery, ComparePointers);
		DestroyFishery(fishery);
	}
	return Py_BuildValue("i", 1);
}

/* Function: MPyDoesFisheryExist
 * -----------------------------
 * Checks if fishery with fishery_id exists. Returns 1 if it exists, 
 * 0 otherwise.
 * 
 * *args:	Python integer representing simulation ID.
 *
 * Returns: Python integer as boolean for simulation existence. 1 if 
 *			simulation exists, 0 otherwise.
*/
PyObject *MPyDoesFisheryExist(PyObject *self, PyObject *args) {
	int fishery_id, success=0;
	Fishery *fishery;

	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;
	/* Find fishery with provided ID. */
	fishery = LListSearch(fishery_llist, &fishery_id, CompareFisheries);
	if (fishery != NULL)
		success = 1;
	
	return Py_BuildValue("i", success);
}

static PyMethodDef fishery_methods[] = {
	{ "MPyCreateFishery", (PyCFunction) MPyCreateFishery, METH_VARARGS, NULL },
	{ "MPyGetFisheryVegetation", (PyCFunction)MPyGetFisheryVegetation, 
	METH_VARARGS, NULL },
	{ "MPyUpdateFishery", (PyCFunction) MPyUpdateFishery, 
	METH_VARARGS, NULL },
	{ "MPyGetFisherySettingOrder", (PyCFunction)MPyGetFisherySettingOrder, 
	METH_NOARGS, NULL },
	{ "MPyGetFisheryFishPopulation", (PyCFunction)MPyGetFisheryFishPopulation, 
	METH_VARARGS, NULL },
	{ "MPyDestroyFishery", (PyCFunction)MPyDestroyFishery, METH_VARARGS, NULL },
	{ "MPyDoesFisheryExist", (PyCFunction)MPyDoesFisheryExist, METH_VARARGS, NULL },
	{ "MPySetRNGSeed", (PyCFunction)MPySetRNGSeed, METH_VARARGS, NULL },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef fisherymodule = {
	PyModuleDef_HEAD_INIT,
	"fishery",   /* name of module */
	NULL, /* module documentation, may be NULL */
	-1,       /* size of per-interpreter state of the module,
			  or -1 if the module keeps state in global variables. */
	fishery_methods
};

PyMODINIT_FUNC PyInit_fishery(void)
{
	return PyModule_Create(&fisherymodule);
}
