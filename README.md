CS583 - Project - Violation Heap
===============
by Haoliang 11/25/2013

-> Compilation Instructions

   Please use the attached Makefile to compile the program.
   Upon successful compilation, the executable - test_out will be generated.
   
-> Execution Instructions

   Start the program by:
   	./test_out <SIZE>
   where the parameter SIZE is the problem size you want to test. If omitted,
   the default size will be 10000.
   
   The program will make a violation heap, perform INSERT operation SIZE times
   and EXTRACT-MIN and DECREASE-KEY operation SIZE/2 times.
   
   Runtime of the program is calculated by several program counters and the
   result will be printed out when the program exits.
   
-> Testing

   The program is compiled and tested in a Linux 3.8.0-33 x86_64 with GCC 4.7.3.
   
   If you want to verify the correctness of the program, please define VERIFY
   in the violation_heap.h header file and re-compile so the program will 
   compare if the result returned by EXTRACT-MIN routine is the minimum or not.
   
   A runtime report is also attached with the source code (report.pdf)
   
-> Known Bugs
   
   None.
   
-> Reference
   
   * The Violation Heap: A relaxed Fibonacci-Like Heap by Amr Elmasry, 2010
