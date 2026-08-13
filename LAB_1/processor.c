#include <stdio.h>
#include "processor.h"
#include "memory.h"

// CPU Registers 
int Register[256];

// Program Counter
int PC;

// Current Instruction 
int opcode;
int dest;
int src1;
int src2;

// End Flag 
int end_of_simulation = 0;

// Reset Processor 
void reset()
{
    int i;

    for(i=0;i<256;i++)
        Register[i]=0;

    PC=0;

    end_of_simulation=0;

    printf("Processor Reset Successful\n");
}

// Fetch 
void fetch()
{
    opcode = Instruction[PC];
    dest   = Instruction[PC+1];
    src1   = Instruction[PC+2];
    src2   = Instruction[PC+3];

    PC = PC + 4;
}

// Decode
void decode()
{
    /* Empty as per lab specification */
}

// Execute
void execute()
{
    switch(opcode)
    {
        // ADD
        case 1:

            Register[dest] =
            Register[src1] +
            Register[src2];

            break;

        // SUB
        case 2:

            Register[dest] =
            Register[src1] -
            Register[src2];

            break;

        // MUL
        case 3:

            Register[dest] =
            Register[src1] *
            Register[src2];

            break;

       // DIV
        case 4:

            if(Register[src2]!=0)
            {
                Register[dest] =
                Register[src1] /
                Register[src2];
            }

            break;

        // READ
        case 5:

            Register[dest] = Data[src1];

            break;

       // WRITE
        case 6:

            Data[src1] = Register[dest];

            break;

        // MOV 
        case 7:

            Register[dest] = src1;

            break;

        // END 

        case 0:

            end_of_simulation = 1;

            break;

        default:

            printf("Invalid Opcode : %d\n",opcode);
            end_of_simulation = 1;
    }
}