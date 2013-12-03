#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "violation-heap.h"

/* Variables used for timing purpose */
#ifdef TIMING
/* flag to tell join_list() which funtion called it
	0 for insert(), 1 for extract_min() and 2 for decrease_key() */
int join_list_flag = 0;

/* Variables to record the "runtime" */
int insert_step = 0;
int extract_step = 0;
int decrease_step = 0;
#endif

/***********************************************************/
/********** Auxilary Functions for Violation Heap **********/

/* Calculate the ceilings */
int ceiling(float num)
{
#ifdef TIMING
	decrease_step++;
#endif
	if(num == (int)num)
		return (int)num;
	if(num > 0)
		return (int)(num + 1);
	else if(num < 0)
		return (int)(num - 1);
	else
		return 0;
}

/* Find its parent and check if this node is active or not */
int is_active(node_t *node, node_t **p)
{
#ifdef TIMING
	decrease_step++;
#endif
	node_t *parent = node;
	int active = 3;

	if(node->prev == NULL)
		return 0;
	while(parent->prev->child != parent) {
		parent = parent->prev;
		active--;
#ifdef TIMING
		decrease_step++;
#endif
	}
	*p = parent->prev;
	return active > 0 ? 1 : 0;
}

/* Update the node */
int update(node_t *node)
{
#ifdef TIMING
	decrease_step++;
#endif
	int or = node->rank;

	/* Get node's active children */
	node_t *z1 = NULL, *z2 = NULL;
	if(node->child) {
		z1 = node->child;
		z2 = node->child->next;
	}

	float rz1 = z1 ? z1->rank : -1;
	float rz2 = z2 ? z2->rank : -1;
	node->rank = ceiling((rz1 + rz2) / 2) + 1;

	return or;
}

/* Combine list_b into list_a */
void join_list(node_t **list_a, node_t **list_b)
{
#ifdef TIMING
	switch(join_list_flag) {
	case 0:
		insert_step++;
		break;
	case 1:
		extract_step++;
		break;
	case 2:
		decrease_step++;
		break;
	}
#endif
	node_t *next, *curr;

	/* Handle corner cases */
	if(*list_b == NULL)
		return;
	if(*list_a == NULL) {
		*list_a = *list_b;
		return;
	}

	/* Let the list with greater key to be the primary list */
	if((*list_a)->key > (*list_b)->key)
		SWAP_NODE(*list_a, *list_b);

	/* Save the break point of the primary list */
	next = (*list_a)->next;

	/* Connect one end of list_b */
	(*list_a)->next = *list_b;

	/* Move curr to the other end of list_b */
	curr = *list_b;
	while(curr->next) {
#ifdef TIMING
		switch(join_list_flag) {
		case 0:
			insert_step++;
			break;
		case 1:
			extract_step++;
			break;
		case 2:
			decrease_step++;
			break;
		}
#endif
		curr = curr->next;
	}

	/* Connect the other end */
	if(next)
		curr->next = next;
}

/* Link z1 to the child list of z */
void link(node_t *z, node_t *z1)
{
#ifdef TIMING
	extract_step++;
#endif
	node_t *child = z->child;

	/* Attach z1 to the last position in z's children list */
	z->child = z1;

	/* Pointer to its parent */
	z1->prev = z;

	/* Attach other children (if any) to the end of z1*/
	z1->next = child;
	if(child)
		child->prev = z1;
}

/* Consolidate the heap */
void consolidate(heap_t *heap)
{
#ifdef TIMING
	extract_step++;
#endif
	int i, num_nodes = 0,  max_rank = 0;

	/* Allocate memory space to record nodes with the same rank */
	node_t **A_list = (node_t **)calloc(2 * heap->size, sizeof(node_t *));

	/* Temporary list to store all the nodes in the heap */
	node_t **root_list = (node_t **)calloc(heap->size, sizeof(node_t *));

	/* Put all roots in the temporary list */
	node_t *curr = heap->root_list;
	if(curr == NULL)
		return;
	while(curr) {
#ifdef TIMING
		extract_step++;
#endif
		root_list[num_nodes++] = curr;
		curr = curr->next;
	}

	/* Iterate through the temporary list */
	for(i = 0; i < num_nodes; i++) {
#ifdef TIMING
		extract_step++;
#endif
		node_t *z = root_list[i];

		/* Unlink the current node z */
		z->prev = z->next = NULL;

		/* If there are two nodes with the same rank
			perform 3-way-join(z, z1, z2) */
		node_t *z1 = A_list[2 * z->rank];
		node_t *z2 = A_list[2 * z->rank + 1];
		while(z1 != NULL && z2 != NULL) {
#ifdef TIMING
			extract_step++;
#endif
			/* Make sure the key of z is the minimum */
			if(z->key > z1->key)
				SWAP_NODE(z, z1);
			if(z->key > z2->key)
				SWAP_NODE(z, z2);

			/* Link z1 and z2 to z */
			link(z, z1);
			link(z, z2);

			/* Clear old positions */
			A_list[2 * z->rank] = A_list[2 * z->rank + 1] = NULL;

			/* Increment rank */
			z->rank++;

			/* Update new z1 & z2 */
			z1 = A_list[2 * z->rank];
			z2 = A_list[2 * z->rank + 1];
		}

		/* Find a free space and put z back into the list */
		if(z1 == NULL)
			A_list[2 * z->rank] = z;
		else if(z2 == NULL)
			A_list[2 * z->rank + 1] = z;

		/* Update the current max_rank */
		if(max_rank < z->rank)
			max_rank = z->rank;
	}

	/* Rebuild the root list from A_list */
	heap->root_list = NULL;

	/* For node(s) at each rank */
	for(i = 0; i < 2 * (max_rank + 1); i += 2) {
#ifdef TIMING
		extract_step++;
#endif
		node_t *p1 = A_list[i];
		node_t *p2 = A_list[i + 1];

		/* Add them into the root list */
		join_list(&heap->root_list, &p1);
		join_list(&heap->root_list, &p2);
	}

	/* Free the temporary lists */
	free(A_list);
	free(root_list);
}

/******************************************************/
/********** Main routines for Violation Heap **********/

/* Make a heap */
heap_t *make_heap()
{
	heap_t *heap = NULL;

	/* Allocate memory space for a new heap and initial it to all zero.
		Exit if calloc failed. */
	assert((heap = (heap_t *)calloc(1, sizeof(heap_t))) != NULL);

	return heap;
}

/* Make a node with the given key */
node_t *make_node(key_t key)
{
#ifdef TIMING
	insert_step++;
#endif
	node_t *node = NULL;

	/* Allocate memory space for a new node and initial it to all zero.
		Exit if calloc failed. */
	assert((node = (node_t *)calloc(1, sizeof(node_t))) != NULL);

	/* Assign key to the new node */
	node->key = key;

	return node;
}

/* Insert a given key to a heap */
node_t *insert(heap_t *heap, key_t key)
{
#ifdef TIMING
	insert_step++;
	join_list_flag = 0;
#endif

	/* Make a new node */
	node_t *node = make_node(key);

	/* Insert the node */
	join_list(&heap->root_list, &node);

	/* Increase the heap size */
	heap->size++;

	return node;
}

/* Find, delete and return the node with the minimum key within the heap */
node_t *extract_min(heap_t *heap)
{
#ifdef TIMING
	extract_step++;
	join_list_flag = 1;
#endif
	node_t *min_node = heap->root_list;
	node_t *child_list = min_node->child;

	if(min_node) {
		/* Remove the min_node and decrement the heap size */
		heap->root_list = min_node->next;
		heap->size--;

		/* Attach all its children to the root_list */
		join_list(&heap->root_list, &child_list);

		/* Consolidate the root_list*/
		consolidate(heap);
	}

	return min_node;
}

/* Decrease the key of a given node in the given heap to a given new_key */
void decrease_key(heap_t *heap, node_t *node, key_t new_key)
{
#ifdef TIMING
	decrease_step++;
	join_list_flag = 2;
#endif
	int or;
	node_t *curr, *parent = NULL, *prev = NULL, *head = NULL, *tail = NULL;

	/* Check if it is unnecessary to decrease key */
	if(node->key <= new_key)
		return;
	else
		node->key = new_key;

	/* If node is a root and the new key is less than the old minimum
		switch the root_list pointer and return */
	if(node->prev == NULL) {
		if(node->key < heap->root_list->key) {
		    /* Break the root_list from node */
		    prev = heap->root_list;
		    while(prev->next != node) {
#ifdef TIMING
		        decrease_step++;
#endif
		        prev = prev->next;
		    }
		    prev->next = NULL;

		    /* Point root_list to node */
		    head = heap->root_list;
		    heap->root_list = node;

		    /* Attach the list to the end of node */
		    tail = node;
		    while(tail->next != NULL) {
#ifdef TIMING
		        decrease_step++;
#endif
		        tail = tail->next;
		    }
		    tail->next = head;
		}
		return;
	}

	/* If node is active and key is not smaller than its parent, return. */
	if(is_active(node, &parent) && parent->key <= node->key)
		return;

	/* Otherwise, cut the subtree of node, prompt node as a root and 
		propagate rank updates by traversing from node's old position */
	or = update(node);
	curr = node;
	while(or > curr->rank && is_active(curr, &parent)) {
#ifdef TIMING
		decrease_step++;
#endif
		or = update(parent);
		curr = parent;

		if(curr->prev == NULL)
			break;
	}

	if(node->prev->child == node) {
		node->prev->child = node->next;
		if(node->next)
			node->next->prev = node->prev;
	} else {
		node->prev->next = node->next;
		if(node->next)
			node->next->prev = node->prev;
	}

	node->next = node->prev = NULL;

	join_list(&heap->root_list, &node);
}

/* Combine heap_a and heap_b and point heap_a to the new heap */
void meld(heap_t *heap_a, heap_t *heap_b)
{
	heap_a->size += heap_b->size;
	join_list(&heap_a->root_list, &heap_b->root_list);
	free(heap_b);
}

/* Return the minimum key within the given heap */
key_t find_min(heap_t *heap)
{
	return heap->root_list->key;
}
