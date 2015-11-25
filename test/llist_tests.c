/*
Tests for the linked list structure in the fishery project.
*/

#include "llist_tests.h"

int TestLList(void) {
	printf("-----------\n");
	printf("Testing LList functionality.\n");
	printf("-----------\n");
	TestLListCreate();
	TestLListIsEmpty();
	TestLListAdd();
	TestLListPop();
	printf("-----------\n");
	printf("All LList tests passed.\n");
	return 1;
}


void TestLListCreate(void) {
	LList_Node *root;

	printf("Testing LListCreate()!\n");

	root = LListCreate();
	assert(root != NULL);

	assert(LListIsEmpty(root));

	free(root);
	printf("Test passed.\n");
}
void TestLListIsEmpty(void) {
	LList_Node *root;

	printf("Testing LListIsEmpty()!\n");
	root = LListCreate();
	assert(LListIsEmpty(root));

	free(root);
	printf("Test passed.\n");
}
void TestLListAdd(void) {
	LList_Node *root;
	int *node_value1, *node_value2;

	node_value1 = malloc(sizeof(int));
	node_value2 = malloc(sizeof(int));

	*node_value1 = 5;
	*node_value2 = 10;


	printf("Testing LListAdd()!\n");
	root = LListCreate();
	LListAdd(root, node_value1);
	assert(LListIsEmpty(root) != 1);
	LListAdd(root, node_value2);
	assert(LListIsEmpty(root) != 1);
	assert(LListPop(root, node_value1, CompareInts));
	assert(LListPop(root, node_value2, CompareInts));
	assert(LListIsEmpty(root));

	free(root); free(node_value1); free(node_value2);
	printf("Test passed.\n");
}
void TestLListPop(void) {
	LList_Node *root, *tmp_ptr;
	int *node_value1, *node_value2, *ptr1, i;
	
	node_value1 = malloc(sizeof(int));
	node_value2 = malloc(sizeof(int));
	
	*node_value1 = 5;
	*node_value2 = 10;

	printf("Testing LListPop()!\n");
	root = LListCreate();
	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	i = 0; tmp_ptr = root;
	while (tmp_ptr) {
		i++; tmp_ptr = tmp_ptr->next;
	}
	assert(i == 2);
	assert((ptr1 = LListPop(root, node_value1, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	i = 0; tmp_ptr = root;
	while (tmp_ptr) {
		i++; tmp_ptr = tmp_ptr->next;
	}
	assert(i == 1);
	assert((ptr1 = LListPop(root, node_value2, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	i = 0; tmp_ptr = root;
	while (tmp_ptr) {
		i++; tmp_ptr = tmp_ptr->next;
	}
	assert(i == 1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, NULL, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, node_value2, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, node_value1, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, NULL, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, node_value1, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, NULL, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, node_value2, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, NULL, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, NULL, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert(LListPop(root, node_value1, CompareInts) == NULL);

	LListAdd(root, node_value1);
	LListAdd(root, node_value2);
	assert((ptr1 = LListPop(root, node_value2, CompareInts)) != NULL);
	assert(ptr1 == node_value2);
	assert((ptr1 = LListPop(root, node_value2, CompareInts)) == NULL);
	assert(LListIsEmpty(root) == 0);
	assert((ptr1 = LListPop(root, NULL, CompareInts)) != NULL);
	assert(ptr1 == node_value1);
	assert(LListIsEmpty(root));
	assert((ptr1 = LListPop(root, node_value1, CompareInts)) == NULL);

	free(root); free(node_value1); free(node_value2);
	printf("Test passed.\n");
}
