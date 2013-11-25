.c.o:
	gcc -g -c $?

all: violation-heap.o violation-heap-test.o
	gcc -g -o test_violation_heap violation-heap.o violation-heap-test.o	
	rm *.o	

clean:
	rm *.o test_violation_heap -f
