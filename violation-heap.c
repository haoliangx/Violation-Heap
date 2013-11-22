#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "violation-heap.h"

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
}v_heap_t;

*/

v_heap_t *make_v_heap()
{
	v_heap_t *heap = (v_heap_t *)calloc(sizeof(v_heap_t));
	assert(heap);
	return heap;
}

node_t *make_node(key_t key)
{
	node_t *node = (node_t *)calloc(sizeof(node_t));
	assert(node);
	node->key = key;
	
	return node;
}

/* Combine two node lists into one */
void join_list(node_t **list_a, node_t **list_b)
{
	node_t **temp, *next, *curr, *parent;
	
	/* Let the list with greater key to be the primary list */
	if((*list_a)->key < (*list_b)->key)
	{
		temp = list_a;
		list_a = list_b;
		list_b = temp;
	}
	
	/* Clear the the child pointer of list_b's parent (if any)*/
	if((*list_b)->prev)
		(*list_b)->prev->child = NULL;
	
	/* Save the break point of the primary list */
	next = (*list_a)->next;
	
	/* Connect one end of list_b */
	(*list_a)->next = *list_b;
	(*list_b)->prev = *list_a;
	
	/* Move curr to the other end of list_b */
	*curr = *list_b;
	while(curr->next);
	
	/* Connect the other end */
	curr->next = next;
	next->prev = curr;
}

void *insert(v_heap_t *heap, key_t key)
{
	node_t *node = make_node(key);
	
	heap->size++;
	if(heap->root_list == NULL)
		heap->root_list = node;
	else
		join_list(&heap->root_list, &node);
}

void remove_root(v_heap_t *heap, node_t *root)
{
	heap->root_list = root->next;
	if(root->next)
		root->next->prev = root->prev;
	if(root->prev)
		root->prev->next = root->next;
}

node_t *extract_min(v_heap_t *heap)
{
	node_t *min_node = heap->root_list;
	
	if(min_node)
	{
		remove_root(heap, min_node);
		heap->size--;
		join_list(&heap->root_list, &min_node->child);
		consolidate(heap);
	}
	
	return min_node;
}

void link(node_t *z, node_t *z1)
{
	node_t *curr = z->child;
	while(curr->next != z->child)
		curr->
}

void consolidate(v_heap_t *heap)
{
	//int i, *A = (int *)calloc(sizeof(int) * heap->size);
	int i = 0;
	node_t *A_list = (node_t *)calloc(sizeof(node_t) * 2 * heap->size);
	node_t **root_list = (node_t **)calloc(sizeof(node_t *) * heap->size);
	node_t *curr = heap->root_list;
	while(curr->next != heap->root_list)
		root_list[i++] = curr;
	while(i-- > 0)
	{
		curr = root_list[i];
		node_t *pos = A_list + 2 * curr->rank;
		if(pos == NULL)
		 	pos = curr;
		else if(pos + 1 == NULL)
			pos + 1 = curr;
		else
		{		
			node_t *z1 = pos;
			node_t *z2 = pos + 1;
			
			if(z1->rank > z2->rank)
			{
				z1 = pos + 1;
				z2 = pos;
			}
			
			link(z, z1);
			link(z, z2);
			
			
			pos = NULL;
			pos + 1 = NULL;
			A_list + 2 * curr->rank = curr;
		}
	}
}

void decrease_key(v_heap_t *heap, node_t, node, key_t new_key)
{
	
}

v_heap_t *meld(v_heap_t *heap_a, v_heap_t *heap_b)
{

}

node_t *find_min(v_heap_t *heap)
{
	
}

