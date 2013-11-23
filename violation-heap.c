#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "violation-heap.h"

#define SWAP_NODE(a, b) 	\
do {				\
	node_t *_temp = a;	\
	a = b;			\
	b = _temp;		\
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
void decrease_key(heap_t *heap, node_t *node, key_t new_key)
{

}

heap_t *meld(heap_t *heap_a, heap_t *heap_b)
{

}

node_t *find_min(heap_t *heap)
{

}

*/

void print_heap(heap_t *heap)
{
    return;
    printf("Heap size = %d\n", heap->size);
    node_t *node = heap->root_list;
    if(!node)
        printf("Heap has no child\n");
    else
    {
        printf("Heap Child: ");
        for(; node!=NULL; node = node->next)
        {
            printf("%d", node->key);
            if(node->child)
                printf("(C)");
            printf(" ");
        }
        printf("\n");
    }
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

/* Combine two node lists into one */
void join_list(node_t **list_a, node_t **list_b)
{
    node_t *temp, *next, *curr, *parent;

    //printf("%p, %p\n", *list_a, *list_b);
    if(*list_a == NULL && *list_b == NULL)
        return;
    else if(*list_a == NULL)
    {
        *list_a = *list_b;
        return;
    }
    else if(*list_b == NULL)
    {
        *list_b = *list_a;
        return;
    }

    /* Let the list with greater key to be the primary list */
    if((*list_a)->key > (*list_b)->key)
    {
        temp = *list_a;
        *list_a = *list_b;
        *list_b = temp;
    }
    //printf(">%p, %p\n", *list_a, *list_b);
    /* Clear the the child pointer of list_b's parent (if any)*/
    if((*list_b)->prev)
        (*list_b)->prev->child = NULL;
    //printf(">%p, %p\n", *list_a, *list_b);

    /* Save the break point of the primary list */
    next = (*list_a)->next;
    //printf(">%p, %p\n", *list_a, *list_b);

    /* Connect one end of list_b */
    (*list_a)->next = *list_b;
    //printf(">%p, %p\n", *list_a, *list_b);
    (*list_b)->prev = *list_a;

    /* Move curr to the other end of list_b */
    curr = *list_b;
    while(curr->next)
        curr = curr->next;

    /* Connect the other end */
    curr->next = next;
    if(next)
        next->prev = curr;
}

void *insert(heap_t *heap, key_t key)
{
    //printf(">>>Before insert %d\n", key);
    print_heap(heap);
    node_t *node = make_node(key);
    heap->size++;
    if(heap->root_list == NULL)
        heap->root_list = node;
    else
        join_list(&heap->root_list, &node);
    //printf(">>>After insert %d\n", key);
    print_heap(heap);
}

void link(node_t *z, node_t *z1)
{
    node_t *child = z->child;

    /* Attach z1 to the last position in z's children list */
    z->child = z1;

    /* Pointer to its parent */
    z1->prev = z;

    /* Attach other children to z1 (if any)*/
    if(child)
        child->prev = z1;
    z1->next = child;
}

void consolidate(heap_t *heap)
{
    int i = 0;

    /* Allocate memory space to record nodes with the same rank */
    node_t **A_list = (node_t **)calloc(2 * heap->size, sizeof(node_t *));

    /* Temporary list to store all the nodes in the heap */
    node_t **root_list = (node_t **)calloc(heap->size, sizeof(node_t *));

    /* Put all roots in the temporary list */
    node_t *curr = heap->root_list;
    if(curr == NULL)
        return;
    while(curr)
    {
        root_list[i++] = curr;
        curr = curr->next;
    }

    /* Iterate through the temporary list */
    while(i-- > 0)
    {
        /* Current node */
        node_t *z = root_list[i];

        /* Put it in its designated postion if it is free */
        node_t *pos = A_list[2 * z->rank];
        node_t *pos_2 = A_list[2 * z->rank + 1];
        while(pos != NULL && pos_2 != NULL)
        {
            node_t *z1 = pos;
            node_t *z2 = pos_2;

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

            A_list[2 * z->rank] = NULL;
            A_list[2 * z->rank + 1] = NULL;

            /* Increment rank */
            z->rank++;

            /* Update new pos */
            pos = A_list[2 * z->rank];
            pos_2 = A_list[2 * z->rank + 1];
        }

        z->prev = NULL;
        z->next = NULL;

        /* Put z back into the list */
        if(pos == NULL)
            A_list[2 * z->rank] = z;
        else if(pos_2 == NULL)
            A_list[2 * z->rank + 1] = z;
    }

    heap->root_list = NULL;
    node_t **ic;
    for(i = 0; i < 2 * heap->size; i+=2)
    {
        node_t *p1 = A_list[i];
        node_t *p2 = A_list[i + 1];
        if(p1)
        {
            if(heap->root_list)
                join_list(&heap->root_list, &p1);
            else
                heap->root_list = p1;
            if(p2)
                join_list(&heap->root_list, &p2);
        }
    }
    free(A_list);
    free(root_list);
}

node_t *extract_min(heap_t *heap)
{
    node_t *min_node = heap->root_list;
    print_heap(heap);
    if(min_node)
    {
        heap->root_list = min_node->next;
        heap->size--;
        node_t *child = min_node->child;
        if(child)
            join_list(&heap->root_list, &child);
        consolidate(heap);
    }
    print_heap(heap);
    return min_node;
}

#include <time.h>

int main()
{
    heap_t *heap = make_heap();
    int i;
    srand(time(0));
    printf("Insert: \n");
    for(i = 0; i < 20; i++)
    {
        int n = rand()%2000;
        insert(heap, n);
        printf("%d, ", n);
        fflush(stdout);
    }
    printf("\nExtract\n");
    for(i = 0; i < 20; i++)
    {
        node_t *min = extract_min(heap);
        if(min)
            printf("%d, ", min->key);
        else
            printf("NULL, ");
    }
    printf("\n");
}

