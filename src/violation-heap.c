#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "violation-heap.h"

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
	*p = parent;
	return active > 0 ? 1 : 0;
}

/* Re-calculate node's rank */
int rank_update(node_t *node)
{
#ifdef TIMING
	decrease_step++;
#endif
	int old_rank = node->rank;

	/* Get node's active children */
	node_t *z1 = NULL, *z2 = NULL;
	if(node->child) {
		z1 = node->child;
		z2 = node->child->next;
	}

	/* Update the rank */
	float rz1 = z1 ? z1->rank : -1;
	float rz2 = z2 ? z2->rank : -1;
	node->rank = ceiling((rz1 + rz2) / 2) + 1;

	/* Return the old rank */
	return old_rank;
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
	int flag = 0;

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

	/* Clear the the child pointer of list_b's parent (if any)*/
	if((*list_b)->prev)
		(*list_b)->prev->child = NULL;

	/* Save the break point of the primary list */
	next = (*list_a)->next;

	/* Connect one end of list_b */
	(*list_a)->next = *list_b;
	if(flag)
		(*list_b)->prev = *list_a;

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
	if(next) {
		curr->next = next;
		if(flag)
			next->prev = curr;
	}
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
			/* Sort z, z1, z2 */
			if(z->key > z1->key)
				SWAP_NODE(z, z1);
			if(z->key > z2->key)
				SWAP_NODE(z, z2);
			if(z1->key > z2->key)
				SWAP_NODE(z1, z2);

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

#ifdef DEBUG
	printf("Before inserting %d\n", key);
	print_heap(heap);
#endif
	/* Make a new node */
	node_t *node = make_node(key);

	/* Insert the node */
	join_list(&heap->root_list, &node);

	/* Increase the heap size */
	heap->size++;

#ifdef DEBUG
	printf("After inserting %d\n", key);
	print_heap(heap);
#endif

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

#ifdef DEBUG
	printf("Extracted node: %d\n", min_node->key);
	print_heap(heap);
#endif
	if(min_node) {
		/* Remove the min_node and decrement the heap size */
		heap->root_list = min_node->next;
		heap->size--;

		/* Attach all its children to the root_list */
		join_list(&heap->root_list, &child_list);

		/* Consolidate the root_list*/
		consolidate(heap);
	}

#ifdef DEBUG
	printf("After consolidate \n");
	print_heap(heap);
#endif

	return min_node;
}

/* Decrease the key of a given node in the given heap to a given new_key */
void decrease_key(heap_t *heap, node_t *node, key_t new_key)
{
#ifdef TIMING
	decrease_step++;
	join_list_flag = 2;
#endif
	int old_rank;
	node_t *curr, *parent = NULL, *prev = NULL, *head = NULL, *tail = NULL;

#ifdef DEBUG
	printf("Before decrease %d to %d: \n", node->key, new_key);
	print_heap(heap);
#endif

	/* Check if it is unnecessary to decrease key */
	if(node->key <= new_key)
		return;
	else
		node->key = new_key;

	/* If node is a root and the new key is less than the old minimum
		switch the root_list pointer and return */
	if(node->prev == NULL && node->key < heap->root_list->key) {
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

		return;
	}

	/* If node is non-active, or, if node is active and the key is not
		smaller than its parent, return. Otherwise, modify forest
		strcuture and propagate rank update to upper levels */
	if(is_active(node, &parent))
		if(parent->key <= node->key)
			return;
		else {
			/* Recalculate rank and propagate the update */
			old_rank = rank_update(node);
			curr = node;
			while(old_rank > curr->rank && is_active(curr, &parent)) {
#ifdef TIMING
				decrease_step++;
#endif
				old_rank = rank_update(parent);
				curr = parent;

				/* Stop if reached the root */
				if(curr->prev == NULL)
					break;
			}

			/* Cut node from the tree */
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

			/* Attach node to the root_list */
			join_list(&heap->root_list, &node);
		}
#ifdef DEBUG
	printf("After decrease key \n");
	print_heap(heap);
#endif
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

/* Print out the structure of the heap */
void print_heap(heap_t *heap)
{
	int i, level = 0, len_a = 1, len_b = 0;

	node_t *node = NULL;
	node_t **list_a = (node_t **)calloc(heap->size, sizeof(node_t *));
	node_t **list_b = (node_t **)calloc(heap->size, sizeof(node_t *));
	assert(list_a != NULL && list_b != NULL);

	printf("\n******** Heap size = %-5d ********\n", heap->size);

	/* Iterate all nodes level by level */
	list_a[0] = heap->root_list;
	while(len_a > 0) {
		printf(">>> %d Level node\n", level++);
		for(i = 0; i < len_a; i++) {
			for(node = list_a[i]; node != NULL; node = node->next) {
				/* Print its key first */
				printf("\t%-5d ", node->key);

				/* Check the type of the node
					(R) - Root node
					(F-X) - First child of X node
					(C-X) - Other child of X node */
				if(node->prev == NULL)
					printf("(R)\t");
				else if(node->prev->child == node)
					printf("(F-%d)\t", node->prev->key);
				else {
					node_t *temp = node;
					while(temp->prev->child != temp)
						temp = temp->prev;
					printf("(C-%d)\t", temp->prev->key);
				}

				/* Check if the node has child
					print out (H) if it has child
					and push the child into the tlist
					for furthur processing */
				if(node->child) {
					printf("(H)\t");
					list_b[len_b++] = node->child;
				} else
					printf("  \t");

				/* Print the rank of the node */
				printf("(%d)\n", node->rank);
			}
		}

		/* Update iteration parameters */
		memcpy(list_a, list_b, len_b * sizeof(node_t *));
		len_a = len_b;
		len_b = 0;
	}

	/* Clean up */
	free(list_a);
	free(list_b);

	printf("***********************************\n");
}
