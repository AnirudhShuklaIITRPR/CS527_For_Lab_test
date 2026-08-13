#include <stdio.h>

#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main()
{
    // Step 1 : Compile source program 
    compile();

    // Step 2 : Load Memory 
    initialize();

    // Step 3 : Reset CPU 
    reset();

    // Step 4 : Execute Program 
    while(!end_of_simulation)   
    {
        fetch();
        decode();
        execute();
    }

    // Step 5 : Save Memory
    finalize();
    printf("\nProgram Executed Successfully\n");
    return 0;
}