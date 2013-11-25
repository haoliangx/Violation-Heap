#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "violation-heap.h"

/* Main function for testing */
int main(int argc, char *argv[])
{
	int i, j, num, new_key, delta;
	node_t *d_node;

	/* Check usage */
	if(argc != 2) {
		fprintf(stderr, "Usage: %s SIZE\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Initial random number generator */
	srand(time(0));

	/* Set the problem size */
	num = atoi(argv[1]);

	/* Allocate memory space to store all the nodes */
	node_t **current_key = calloc(num, sizeof(node_t *));

	/* Make a heap */
	heap_t *heap = make_heap();

	/* Insert num random numbers from 5000-7000
		and store them in current_key */
	for(i = 0; i < num; i++)
		current_key[i] = insert(heap, rand()%2000 + 5000);

	/* Decrease key and extract min for num/2 times */
	for(i = 0; i < num/2; i++) {
		/* Find a random non-empty node with its key > 20 */
		do {
			do {
				d_node = current_key[rand() % num];
			} while(d_node == NULL);
		} while(d_node->key <=20);

		/* Generate a random value to decrease */
		delta = rand() % 20;

		/* Decrease key for the random-chosen node */
		decrease_key(heap, d_node, d_node->key - delta);

		/* Extract mininum */
		node_t *min = extract_min(heap);

		/* Mark the corresponding slot for extracted node to be empty */
		for(j = 0; j<num; j++)
			if(current_key[j] == min)
				current_key[j] = NULL;

		/* Free the node */
		free(min);
	}

	/* Free the current_key array */
	free(current_key);

	/* Print the statistics */
	printf("OPERATION   \tCOUNT\tSTEPS\tRUNTIME(AMORTIZED)\n");
	printf("INSERT      \t%d\t%d\t%g\n",
	       num, insert_step, (double)insert_step/num);
	printf("EXTRACT_MIN \t%d\t%d\t%g\n",
	       num/2, extract_step, (double)extract_step/num*2);
	printf("DECREASE_KEY\t%d\t%d\t%g\n",
	       num/2, decrease_step, (double)decrease_step/num*2);

	return 0;
}
