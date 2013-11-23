#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "violation-heap.h"

#define SWAP_NODE(a, b) 		\
do {					\
	node_t *_temp = a;		\
	a = b;				\
	b = _temp;			\
}while(0)

/*

typedef struct _node
{
	key_t key;
	int rank;
	struct _node *next;
	struct _node *prev;
	struct _node *child;
}node_t;

typedef struct _v_heap
{
	int size;
	struct _node *root_list;
}heap_t;

*/

/*
heap_t *meld(heap_t *heap_a, heap_t *heap_b)
{

}

node_t *find_min(heap_t *heap)
{
	
}
*/

/* Check if this node is active or not */
int is_active(node_t *node, node_t **p)
{
	node_t *parent = *p;
	int is_active = 3;
	while(parent->prev->child != parent)
	{
		parent = parent->prev;
		is_active--;
	}
	return active;
}

void decrease_key(heap_t *heap, node_t *node, key_t new_key)
{
	node_t *parent = NULL;
	
	/* Check if it is unnecessary to decrease key */
	if(node->key <= new_key)
		return;
	else
		node->key = new_key;	
	
	/* If node is a root and the new key is less than the old minimum
		switch the root_list pointer and return */
	if(node->prev == NULL && node->key < heap->root_list->key)
	{
		join_list(&heap->root_list, &node);
		return;
	}
	
	/* If node is non-active or if node is active and the key is not 
		smaller than its parent, return. Otherwise, modify forest 
		strcuture and propagate rank update to upper levels */
	if(is_active(node, &parent))
	{
		if(parent->key <= node->key)
			return;
		else
		{
			
		}
	}
	
}

void print_heap(heap_t *heap)
{
	int level = 0, len = 1, tlen = 0, i;
	
	node_t **list = (node_t **)calloc(heap->size, sizeof(node_t *));
	node_t **tlist = (node_t **)calloc(heap->size, sizeof(node_t *));
	node_t *node = NULL;
	
	printf("\n***** Heap size = %d *****\n", heap->size);
	list[0] = heap->root_list;
	while(len > 0)
	{
		printf(">>> %d Level node\n", level++);
		for(i = 0; i < len; i++)
		{
			for(node = list[i]; node != NULL; node = node->next) {
				printf("\t%d", node->key);
				if(node->prev == NULL)
					printf("(R)");
				else if(node->prev->child == node)
					printf("(FC-%d)", node->prev->key);
				else
				{
					node_t *temp = node;
					while(temp->prev->child != temp)
						temp = temp->prev;
					printf("(C-%d)", temp->prev->key);
				}
				if(node->child)
				{
					printf("(HC)");
					tlist[tlen++] = node->child;
				}
				printf("\n");
			}
		}
		memcpy(list, tlist, tlen * sizeof(node_t *));
		len = tlen;
		tlen = 0;
	}
	printf("*************************\n");
}

/* Make a heap and return its pointer */
heap_t *make_heap()
{
	heap_t *heap = NULL;

	/* Allocate memory space for a new heap and initial it to all zero.
		Exit if calloc failed. */
	assert((heap = (heap_t *)calloc(1, sizeof(heap_t))) != NULL);

	return heap;
}

/* Make a node with the given key and return its pointer */
node_t *make_node(key_t key)
{
	node_t *node = NULL;

	/* Allocate memory space for a new node and initial it to all zero.
		Exit if calloc failed. */
	assert((node = (node_t *)calloc(1, sizeof(node_t))) != NULL);

	/* Assign key to the new node */
	node->key = key;

	return node;
}

/* Combine list_b into list_a */
void join_list(node_t **list_a, node_t **list_b)
{
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
	while(curr->next)
		curr = curr->next;

	/* Connect the other end */
	if(next) {
		curr->next = next;
		if(flag)
			next->prev = curr;
	}
}

/* Insert a new key into the heap */
void *insert(heap_t *heap, key_t key)
{
	/* Make a new node */
	node_t *node = make_node(key);

	/* Insert the node */
	join_list(&heap->root_list, &node);

	/* Increase the heap size */
	heap->size++;
}

/* Link z1 to the child list of z */
void link(node_t *z, node_t *z1)
{
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

void consolidate(heap_t *heap)
{
	int num_nodes = 0, i;

	/* Allocate memory space to record nodes with the same rank */
	node_t **A_list = (node_t **)calloc(2 * heap->size, sizeof(node_t *));

	/* Temporary list to store all the nodes in the heap */
	node_t **root_list = (node_t **)calloc(heap->size, sizeof(node_t *));

	/* Put all roots in the temporary list */
	node_t *curr = heap->root_list;
	if(curr == NULL)
		return;
	while(curr) {
		root_list[num_nodes++] = curr;
		curr = curr->next;
	}

	/* Iterate through the temporary list */
	for(i = 0; i < num_nodes; i++) {
		/* Unlink the current node z */
		node_t *z = root_list[i];
		z->prev = NULL;
		z->next = NULL;

		/* If there are two nodes with the same rank
			perform 3-way-join(z, z1, z2) */
		node_t *z1 = A_list[2 * z->rank];
		node_t *z2 = A_list[2 * z->rank + 1];
		while(z1 != NULL && z2 != NULL) {
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

			/* Clear old position */
			A_list[2 * z->rank] = NULL;
			A_list[2 * z->rank + 1] = NULL;

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
	}

	/* Rebuild the root list from A_list */
	heap->root_list = NULL;
	
	/* For node(s) at each rank */
	for(i = 0; i < 2 * heap->size; i+=2) {
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

node_t *extract_min(heap_t *heap)
{
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

#include <time.h>

#define NUM 10

int main()
{
	int i;
	srand(time(0));
	
	heap_t *heap = make_heap();
	
	printf("Insert: \n");
	for(i = 0; i < NUM; i++) {
		int n = rand()%2000;
		insert(heap, n);
		printf("%d\n", n);
		fflush(stdout);
		print_heap(heap);
	}
	printf("\nExtract\n");
	for(i = 0; i < NUM; i++) {
		node_t *min = extract_min(heap);
		if(min)
			printf("%d\n", min->key);
		else
			printf("NULL\n");
		print_heap(heap);
	}
	printf("\n");
}
