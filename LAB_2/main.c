#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "memory.h"
#include "processor.h"

// FILE NAMES
char programFile[100];
char dataFile[100];

// MAIN
int main()
{
    int choice;

    // MENU
    printf("\nMINI COMPUTER SIMULATOR\n");
    printf("1. Sum of N Natural Numbers\n");
    printf("2. Multiply Two Complex Numbers\n");
    printf("3. Determinant of 3x3 Matrix\n");
    printf("4. Runtime Sum of N Numbers\n");
    printf("5. Sum of Array\n");
    printf("6. FIR filter response\n");
    printf("0. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            strcpy(programFile, "Test_case/sum_of_4_numbers/program.txt");
            strcpy(dataFile, "Test_case/sum_of_4_numbers/data.byte");
            break;

        case 2:
            strcpy(programFile, "Test_case/mul_of_2_complex_numbers/program.txt");
            strcpy(dataFile, "Test_case/mul_of_2_complex_numbers/data.byte");
            break;

        case 3:
            strcpy(programFile, "Test_case/determinant/program.txt");
            strcpy(dataFile, "Test_case/determinant/data.byte");
            break;

        case 4:
            strcpy(programFile, "Test_case/runtime_sum_of_n_numbers/program.txt");
            strcpy(dataFile, "Test_case/runtime_sum_of_n_numbers/data.byte");
            break;

        case 5:
            strcpy(programFile, "Test_case/sum_of_array/program.txt");
            strcpy(dataFile, "Test_case/sum_of_array/data.byte");
            break;

        case 6:
            strcpy(programFile, "Test_case/FIR_filter_response/program.txt");
            strcpy(dataFile, "Test_case/FIR_filter_response/data.byte");
            break;    

        case 0:
            printf("Exiting...\n");
            return 0;

        default:
            printf("Invalid choice\n");
            return 0;
    }

    // DISPLAY SELECTED FILES
    printf("\nProgram File : %s\n", programFile);
    printf("Data File    : %s\n", dataFile);

    compile();
    initialize();
    reset();
    while(!end_of_simulation)
    {
        fetch();
        decode();
        execute();
    }
    finalize();

    printf("\nProgram Executed Successfully\n");
    return 0;
}