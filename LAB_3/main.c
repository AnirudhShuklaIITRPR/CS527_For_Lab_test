#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "memory.h"
#include "processor.h"

// FILE NAMES
char programFile[100];
char dataFile[100];

static void runMenu(void)
{
    int choice;

    printf("\nMINI COMPUTER SIMULATOR\n");
    printf("1. Sum of N Natural Numbers\n");
    printf("2. Multiply Two Complex Numbers\n");
    printf("3. Determinant of 3x3 Matrix\n");
    printf("4. Runtime Sum of N Numbers\n");
    printf("5. Sum of Array\n");
    printf("6. FIR filter response\n");
    printf("7. Array Add (vector)\n");
    printf("8. FIR filter response (vector)\n");
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

        case 7:
            strcpy(programFile, "Test_case/array_add_vector/program.txt");
            strcpy(dataFile, "Test_case/array_add_vector/data.byte");
            break;

        case 8:
            strcpy(programFile, "Test_case/fir_filter_vector/program.txt");
            strcpy(dataFile, "Test_case/fir_filter_vector/data.byte");
            break;

        case 0:
            printf("Exiting...\n");
            programFile[0] = '\0';
            return;

        default:
            printf("Invalid choice\n");
            programFile[0] = '\0';
            return;
    }
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        // ./simulator <program_source.txt> <data.byte>
        strncpy(programFile, argv[1], sizeof(programFile) - 1);
        programFile[sizeof(programFile) - 1] = '\0';

        strncpy(dataFile, argv[2], sizeof(dataFile) - 1);
        dataFile[sizeof(dataFile) - 1] = '\0';
    }
    else if (argc == 1)
    {
        runMenu();

        if (programFile[0] == '\0')
            return 0;
    }
    else
    {
        printf("Usage: %s <program_source.txt> <data.byte>\n", argv[0]);
        printf("       %s                         (interactive menu)\n", argv[0]);
        return 1;
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

    printf("\nPROGRAM EXECUTED SUCCESSFULLY\n");
    return 0;
}
