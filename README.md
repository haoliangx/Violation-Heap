Violation Heap
=====
by Haoliang 11/25/2013

A CS583 Course Project based on the paper Elmasry, A. (2010). The Violation Heap: A Relaxed Fibonacci-like Heap. *Discrete Mathematics, Algorithms and Applications*, 2(04), 493-503.

Introduction
-----

The goal is to implement the data structure proposed in the paper and empirically verify the claim that it has the same performance guarantees as *Fibonacci Heap*. Three operations – INSERT, DECREASE-KEY, EXTRACT-MIN are tested with size varying from 100 to 512000. Testing procedure and result are shown as follows.

Measurement
-----

Since time clocks are too coarse to time each separate operation and the operations have different amortized times, timing is problematic at best. Instead, "time" will be computed discretely. In other words let any sequence of simple statements (assignments, if's, etc.) be charged "one unit" of time. More importantly the body of a loop is charged one unit of time for each iteration. The code is instrumented with accumulators to compute "run times"

The testing consists of two phases:
1. Start with n=10000 keys all of which are inserted as the first phase.
2. The second phase will be alternating decrease-key's and delete-min's. The second phase will end after 5000 delete-min's.

The values of the keys will also be maintained outside the heap in the array
CurrentKey[1..n] where CurrentKey[i] is the current key of record i.
The initial key value for each record is a random integer uniformly in the range
[5000..7000]. Each delete-min will have the side effect of setting CurrentKey[i] to
zero, where record i was deleted. Each decease-key begins by choosing uniformly a
record (repeatedly if necessary until a record with CurrentKey value greater
than 20 is found) and then decreasing the CurrentKey array entry for that record by
a random integer uniformly in the range [1..20]; use the calculated key value as
the parameter to decrease-key. (Recall that decrease-key requires you to maintain for
each record a pointer into the heap to the prevent any need to search for the record.)

Compile and Execution
-----

The program is compiled and tested in a Linux 3.8.0-33 x86_64 with GCC 4.7.3. Please use the attached Makefile to compile the program. Upon successful compilation, the executable - test_out will be generated.

Start the program by:

`./test_out <SIZE>`

where the parameter SIZE is the problem size you want to test. If omitted,
the default size will be 10000. The program will make a violation heap, perform INSERT operation SIZE times and EXTRACT-MIN and DECREASE-KEY operation SIZE/2 times. Runtime of the program is calculated by several program counters and the result will be printed out when the program exits.

If you want to verify the correctness of the program, please define VERIFY in the violation_heap.h header file and re-compile so the program will compare if the result returned by EXTRACT-MIN routine is the minimum or not.

Result
-----

See the PDF report.

Conclusion
-----

From the experiment result shown above, the performance three operations in the *Violation Heap* (INSERT, EXTRACT-MIN and DECREASE-KEY) is verified to have the same performance guarantees as *Fibonacci Heap*.
