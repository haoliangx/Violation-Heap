#ifndef VIOLATION_HEAP_HEADER
#define VIOLATION_HEAP_HEADER

typedef int key_t;

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

#endif
