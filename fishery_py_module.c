/* fishery_py_module.c
Contains the interface functions between python and the fishery implementation in c. Initialized 
fishery simulations are stored in a global variable with unique IDs, which are returned to the 
Python callers. Further manipulation of specific simulations requires an ID.
*/
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include "fishery_functions.h"

/*
The simulation(s) are stored, with a unique ID, in a global variable to 
prevent constant transfer of simulation data between python and c extension. 
*/

LList_Node *fishery_llist;
int fishery_id_n = 0;

extern char *setting_order[];
extern int settings_size;

/* Function MPyCreateFishery

Initializes settings variable and simulation according to provided settings. The 
fishery is assigned a numerical ID which is returned.

args	- Dictionary of settings. setting names are defined in fishery_functions.c.	  		 
*/
static PyObject *MPyCreateFishery(PyObject *self, PyObject *args) {
	int i, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, *arg9,
		arg10, arg11, arg12, arg13, *arg14, arg15, arg16;
	double arg17;
	PyObject *dict, *item;
	Fishery_Settings *settings;
	Fishery *fishery;

	/* Check provided dictionary is a dictionary and contains all settings.
	Errors are raised as TypeError and KeyError exceptions. */
	if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict))
		return NULL;
	for (i = 0; i < settings_size; i++) {
		if (PyDict_Contains(dict, PyUnicode_FromString(setting_order[i])) != 1) {
			PyErr_Format(PyExc_KeyError, setting_order[i]);
			return NULL;
		}
	}	
	/* Parse given settings. Should make prettier at some point. All references
	   are borrowed, no need to count references. */
	arg1 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[0]));
	arg2 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[1]));
	arg3 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[2]));
	arg4 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[3]));
	arg5 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[4]));
	arg6 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[5]));
	arg7 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[6]));
	arg8 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[7]));
	arg9 = malloc(sizeof(int)*(arg4 + 1));
	item = PyDict_GetItemString(dict, setting_order[8]);
	for (i = 0; i < arg4 + 1; i++) {
		arg9[i] = PyLong_AsLong(PyList_GetItem(item, (Py_ssize_t)i));
	}
	arg10 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[9]));
	arg11 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[10]));
	arg12 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[11]));
	arg13 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[12]));
	arg14 = malloc(sizeof(int)*(arg11 + 1));
	item = PyDict_GetItemString(dict, setting_order[13]);
	for (i = 0; i < arg11 + 1; i++) {
		arg14[i] = PyLong_AsLong(PyList_GetItem(item, (Py_ssize_t)i));
	}
	arg15 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[14]));
	arg16 = PyLong_AsLong(PyDict_GetItemString(dict, setting_order[15]));
	arg17 = PyFloat_AsDouble(PyDict_GetItemString(dict, setting_order[16]));
	
	/* Generate ID for simulation, reserve memory and initialize simulation. */
	settings = malloc(sizeof(Fishery_Settings));
	*settings = CreateSettings(arg1, arg2, arg3, arg4, arg5, arg6,
		arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17);
	fishery = CreateFishery(*settings);
	fishery->fishery_id = fishery_id_n;
	fishery->settings = settings;
	
	/* Store simulation in the linked list of simulations. */
	if (fishery_llist == NULL)
		fishery_llist = LListCreate();
	LListAdd(fishery_llist, fishery);
	
	free(arg9);
	free(arg14);
	
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
	
	py_setting_order = PyList_New(settings_size);
	if (!py_setting_order) /* PyList_New sets exception. */
		return NULL;
	for (i = 0; i < settings_size; i++) {
		item = Py_BuildValue("s", setting_order[i]);
		if (PyList_SetItem(py_setting_order, i, item) == -1) { /* Steals reference, no need to worry about item. */
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
	int i, fish_population_size = 0, fishery_id, no_error=1;

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
			/* fish position returned here is for the rotated coordinate system, not the array structure of the c program. */
			if(PyList_SetItem(py_fish, 0, PyLong_FromLong(fish_ptr->pos_x + fish_ptr->pos_y*fishery->settings->size_x)) == -1 ||
				PyList_SetItem(py_fish, 1, PyLong_FromLong(fish_ptr->pop_level)) == -1) /* Add fish information. */
				goto error;
			if (PyList_SetItem(py_fish_list, i, py_fish) == -1) /* Add fish to list. */
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

Progresses the fishery simulation n steps, returning the total 
results of the steps, including fish present and fishing yield.

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
		PyErr_Format(PyExc_ValueError, "Amount of steps invalid (%d). Should be larger than 0 and smaller than 100000.\n", n);
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
	results_py = Py_BuildValue("[iiidddii]", results.fish_n, results.yield, results.vegetation_n, 
		results.fish_n_std_dev, results.yield_std_dev, results.vegetation_n_std_dev, results.steps, results.debug_stuff);
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
	{ "MPyGetFisheryVegetation", (PyCFunction)MPyGetFisheryVegetation, METH_VARARGS, NULL },
	{ "MPyUpdateFishery", (PyCFunction) MPyUpdateFishery, METH_VARARGS, NULL },
	{ "MPyGetFisherySettingOrder", (PyCFunction)MPyGetFisherySettingOrder, METH_NOARGS, NULL },
	{ "MPyGetFisheryFishPopulation", (PyCFunction)MPyGetFisheryFishPopulation, METH_VARARGS, NULL },
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
