/* fishery_py_module.c
Contains the interface functions between python and the fishery implementation in c. 
Initialized fishery simulations are stored in a global variable with unique IDs, which 
are returned to the Python callers. Further manipulation of specific simulations requires 
the corresponding ID.
*/
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fishery_functions.h"

/*
The simulation(s) are stored, with a unique ID, in a global variable to 
avoid constant transfer of simulation data between python and c extension. 
*/

LList_Node *fishery_llist;
int fishery_id_n = 0;

extern char *SETTING_ORDER[];
extern int SETTINGS_SIZE;

/* Function MPyCreateFishery

Initializes settings variable and simulation according to provided settings. The 
fishery is assigned a numerical ID which is returned.

args	- Dictionary of settings. Setting names are defined in fishery_functions.c.	  		 
*/
static PyObject *MPyCreateFishery(PyObject *self, PyObject *args) {
	int i;
	PyObject *dict, *list_item;
	Fishery_Settings *settings;
	Fishery *fishery;

	/* Check provided parameter is a dictionary and contains all settings.
	Errors are raised as TypeError and KeyError exceptions. */
	if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict))
		return NULL;
	for (i = 0; i < SETTINGS_SIZE; i++) {
		if (PyDict_Contains(dict, PyUnicode_FromString(SETTING_ORDER[i])) != 1) {
			PyErr_Format(PyExc_KeyError, SETTING_ORDER[i]);
			return NULL;
		}
	}
	/* Parse given settings. Ugly, but avoids dictionary structure / messing around
	with pointers. All references are borrowed. */
	settings = (Fishery_Settings*)malloc(sizeof(Fishery_Settings));
	settings->size_x = (int)PyLong_AsLong(PyDict_GetItemString(dict, "size_x"));
	settings->size_y = (int)PyLong_AsLong(PyDict_GetItemString(dict, "size_y"));
	settings->initial_vegetation_size = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "initial_vegetation_size"));
	settings->vegetation_level_max = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "vegetation_level_max"));
	settings->vegetation_level_spread_at = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "vegetation_level_spread_at"));
	settings->vegetation_level_growth_req = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "vegetation_level_growth_req"));
	settings->soil_energy_max = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "soil_energy_max"));
	settings->soil_energy_increase_turn = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "soil_energy_increase_turn"));
	list_item = PyDict_GetItemString(dict, "vegetation_consumption");
	if (PyList_Check(list_item) == 0)
		return PyErr_Format(PyExc_TypeError, "vegetation_consumption is not a list.");
	settings->vegetation_consumption =
		(int *)malloc(sizeof(int)*(settings->vegetation_level_max + 1));
	for (i = 0; i < settings->vegetation_level_max + 1; i++)
		settings->vegetation_consumption[i] = 
			PyLong_AsLong(PyList_GetItem(list_item, (Py_ssize_t)i));

	settings->initial_fish_size = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "initial_fish_size"));
	settings->fish_level_max = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "fish_level_max"));
	settings->fish_growth_req = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "fish_growth_req"));
	settings->fish_moves_turn = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "fish_moves_turn"));
	list_item = PyDict_GetItemString(dict, "fish_consumption");
	if (PyList_Check(list_item) == 0)
		return PyErr_Format(PyExc_TypeError, "fish_consumption is not a list.");
	settings->fish_consumption =
		(int *)malloc(sizeof(int)*(settings->fish_level_max + 1));
	for (i = 0; i < settings->fish_level_max + 1; i++)
		settings->fish_consumption[i] =
		PyLong_AsLong(PyList_GetItem(list_item, (Py_ssize_t)i));
	settings->random_fishes_interval = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "random_fishes_interval"));
	settings->split_fishes_at_max = 
		(int)PyLong_AsLong(PyDict_GetItemString(dict, "split_fishes_at_max"));
	settings->fishing_chance = 
		PyFloat_AsDouble(PyDict_GetItemString(dict, "fishing_chance"));

	fishery = CreateFishery(*settings);
	fishery->fishery_id = fishery_id_n;
	fishery->settings = settings;
		
	/* Store simulation in the linked list of simulations. */
	if (fishery_llist == NULL)
		fishery_llist = LListCreate();
	LListAdd(fishery_llist, fishery);
		
	return Py_BuildValue("i", fishery_id_n++);
}
/* Function MPyGetFisherySettingOrder()

Returns the order which settings are to be entered in, defined in 
fishery_functions.c.
*/
PyObject *MPyGetFisherySettingOrder(PyObject *self) {
	PyObject *py_setting_order, *item;
	int i;

	srand((unsigned int)time(NULL));
	
	py_setting_order = PyList_New(SETTINGS_SIZE);
	if (!py_setting_order) /* PyList_New sets exception. */
		return NULL;
	for (i = 0; i < SETTINGS_SIZE; i++) {
		item = Py_BuildValue("s", SETTING_ORDER[i]);
		if (PyList_SetItem(py_setting_order, i, item) == -1) {
			/* Steals reference, no need to worry about item. */
			Py_DECREF(py_setting_order);
			return NULL;
		}
	}
	return py_setting_order;
}
/* Function MPyGetFisheryVegetation()

Returns vegetation layer levels as a python array.
*/
PyObject *MPyGetFisheryVegetation(PyObject *self, PyObject *args) {
	PyObject *py_vegetation_list, *item;
	Fishery *fishery=NULL;
	LList_Node *node;
	int i, fishery_id;
	
	/* Find fishery with provided ID. */
	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;
	node = fishery_llist;
	while (node != NULL && node->node_value != NULL) {
		fishery = node->node_value;
		if (fishery->fishery_id == fishery_id)
			break;
		else
			fishery = NULL;
		node = node->next;
	}
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
/* Function MPyGetFisheryFishPopulation()

Returns the fish population of the simulation as a list.
Each entry of the list contains fish position and 
population level.

*args	- Python integer representing simulation ID.

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
	node = fishery_llist;
	while (node != NULL && node->node_value != NULL) {
		fishery = node->node_value;
		if (fishery->fishery_id == fishery_id)
			break;
		else
			fishery = NULL;
		node = node->next;
	}
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
/* Function MPyUpdateFishery()

Progresses the fishery simulation n steps. The result of the n steps are returned
as a python list containing the following values:

number of fish encountered, fishing yield, vegetation level encountered,
std dev of fish encountered, std dev of fishing yield, std dev of vegetation level encountered,
steps progressed, debugging variable, fishing chance of simulation

The last three are elements are included simply to make debugging easier. To be removed
later on.

*args - Two Python integers, first the simulation ID and 
		second the amount of steps to progress the simulation.
*/
PyObject *MPyUpdateFishery(PyObject *self, PyObject *args) {
	LList_Node *node;
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
	node = fishery_llist;
	while (node != NULL && node->node_value != NULL) {
		fishery = node->node_value;
		if (fishery->fishery_id == fishery_id)
			break;
		else
			fishery = NULL;
		node = node->next;
	}
	if (fishery == NULL) {
		PyErr_Format(PyExc_KeyError, "Fishery with ID %d not found.\n", fishery_id);
		return NULL;
	}
	/* Update fishery and save results in python data types. */
	results = UpdateFishery(fishery, (*(fishery->settings)), n);
	results_py = Py_BuildValue("[iiidddiid]", 
		results.fish_n, results.yield, results.vegetation_n, 
		results.fish_n_std_dev, results.yield_std_dev, results.vegetation_n_std_dev, 
		results.steps, results.debug_stuff, fishery->settings->fishing_chance);
	if (!results_py)
		return NULL;
	
	return results_py;
}
/* Function MPyDestroyFishery()

Frees memory used by simulation(s).

*args  - Python integer representing simulation ID. If the ID
		 is -1, all simulations are removed.
*/
PyObject *MPyDestroyFishery(PyObject *self, PyObject *args) {
	LList_Node *node;
	int i = 0, fishery_id;
	Fishery *fishery;

	
	if (!PyArg_ParseTuple(args, "i", &fishery_id))
		return NULL;
	
	if (fishery_id == -1) {
		/* Destroy all simulation(s). */
		LListDestroy(fishery_llist, DestroyFishery);
	}
	else {
		/* Find fishery with provided ID. */
		node = fishery_llist;
		while (node != NULL && node->node_value != NULL) {
			fishery = node->node_value;
			if (fishery->fishery_id == fishery_id)
				break;
			else
				fishery = NULL;
			node = node->next;
		}
		if (fishery == NULL) {
			printf("Fishery with ID %d not found.\n", fishery_id);
			exit(EXIT_FAILURE);
		}
		LListPop(fishery_llist, fishery, ComparePointers);
		DestroyFishery(fishery);
	}
	return Py_BuildValue("i", 1);
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
