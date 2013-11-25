#ifndef VIOLATION_HEAP_HEADER
#define VIOLATION_HEAP_HEADER

#define TIMING

#define SWAP_NODE(a, b) 		\
do {					\
	node_t *_temp = a;		\
	a = b;				\
	b = _temp;			\
}while(0)

#ifdef TIMING
/* Variables to record the "runtime" */
extern int insert_step;
extern int extract_step;
extern int decrease_step;
#endif

/* Define the type of key to be int */
typedef int key_t;

/* Define the node struct */
typedef struct _node {
	key_t key;
	int rank;
	struct _node *next;
	struct _node *prev;
	struct _node *child;
} node_t;

/* Define the heap struct */
typedef struct _v_heap {
	int size;
	struct _node *root_list;
} heap_t;

/* Make a heap */
heap_t *make_heap();

/* Make a node with the given key */
node_t *make_node(key_t key);

/* Insert a given key to a heap */
node_t *insert(heap_t *heap, key_t key);

/* Find, delete and return the node with the minimum key within the heap */
node_t *extract_min(heap_t *heap);

/* Return the minimum key within the given heap */
key_t find_min(heap_t *heap);

/* Decrease the key of a given node in the given heap to a given new_key */
void decrease_key(heap_t *heap, node_t *node, key_t new_key);

/* Combine heap_a and heap_b and point heap_a to the new heap */
void meld(heap_t *heap_a, heap_t *heap_b);

/* Print out the structure of the heap for debug*/
void print_heap(heap_t *heap);

#endif
