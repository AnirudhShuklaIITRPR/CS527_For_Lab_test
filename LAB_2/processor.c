#include <stdio.h>
#include <stdint.h>

#include "processor.h"
#include "memory.h"

// CPU REGISTERS
int32_t Register[256];

// PROGRAM COUNTER
int PC;

// CURRENT INSTRUCTION
unsigned char opcode;
unsigned char dest;
unsigned char src1;
unsigned char src2;

// FLAGS
int Z = 0;      // Zero Flag
int N = 0;      // Negative Flag
int C = 0;      // Carry Flag
int V = 0;      // Overflow Flag

// END FLAG
int end_of_simulation = 0;

// RESET PROCESSOR
void reset()
{
    int i;
    for(i = 0; i < 256; i++)
        Register[i] = 0;
    PC = 0;

    Z = 0;
    N = 0;
    C = 0;
    V = 0;

    end_of_simulation = 0;

    printf("Processor Reset Successfully\n");
}

// FETCH
void fetch()
{
    opcode = (unsigned char)Instruction[PC];
    dest = (unsigned char)Instruction[PC + 1];
    src1 = (unsigned char)Instruction[PC + 2];
    src2 = (unsigned char)Instruction[PC + 3];

    PC = PC + 4;
}

// DECODE
void decode()
{
    /*
    Waiting..
    */
}

// UPDATE FLAGS FOR ADDITION
void updateAddFlags(int32_t a, int32_t b, int32_t result)
{
    uint32_t ua;
    uint32_t ur;

    ua = (uint32_t)a;
    ur = (uint32_t)result;
    
    Z = (result == 0);     // Zero flag
    N = (result < 0);      // Negative flag
    C = (ur < ua);         // Carry flag

    // Signed overflow
    V = (((a >= 0) && (b >= 0) && (result < 0)) || ((a < 0) && (b < 0) && (result >= 0)));
}

// UPDATE FLAGS FOR SUBTRACTION
void updateSubFlags(int32_t a, int32_t b, int32_t result)
{
   
    Z = (result == 0);                       // Zero flag
    N = (result < 0);                        // Negative flag
    C = ((uint32_t)a >= (uint32_t)b);        // Borrow / Carry

    // Signed overflow
    V = (((a >= 0) && (b < 0) && (result < 0)) || ((a < 0) && (b >= 0) && (result >= 0)));
}

// EXECUTE
void execute()
{
    switch(opcode)
    {
        // OPCODE 0 : END PROGRAM
        case 0:
            end_of_simulation = 1;
            break;

        // OPCODE 1 : ADD
        // Register[dest] = Register[src1] + Register[src2]
        case 1:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = Register[src2];
            result = a + b;
            Register[dest] = result;
            updateAddFlags(a, b, result);

            break;
        }

        // OPCODE 2 : SUB
        // Register[dest] = Register[src1] - Register[src2]
        case 2:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = Register[src2];
            result = a - b;
            Register[dest] = result;
            updateSubFlags(a, b, result);

            break;
        }

        // OPCODE 3 : MUL
        case 3:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = Register[src2];
            result = a * b;
            Register[dest] = result;
            Z = (result == 0);
            N = (result < 0);

            break;
        }

        // OPCODE 4 : DIV
        case 4:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = Register[src2];
            if(b == 0)
            {
                printf("Division by zero\n");
                end_of_simulation = 1;
                break;
            }

            result = a / b;
            Register[dest] = result;
            Z = (result == 0);
            N = (result < 0);

            break;
        }


        // OPCODE 5 : READ
        // Register[dest] = Data[src1] ,  src1 is treated as memory address.
        case 5:
            Register[dest] = read32(Register[src2]);
            
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);
            
            break;

        // OPCODE 6 : WRITE
        // Data[src1] = Register[dest]
        case 6:
            write32(Register[dest], Register[src2]);
            break;

        // OPCODE 7 : MOV
        // Register[dest] = src1
        case 7:

            Register[dest] = src1;
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;

        // OPCODE 8
        // NOT / OTHER EXISTING OPERATION
        case 8:
            Register[dest] = ~Register[src1];
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;

        // OPCODE 9 : ADD CONSTANT
        // Register[dest] = Register[src1] + signed_constant
        case 9:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = (int8_t)src2;
            result = a + b;
            Register[dest] = result;
            updateAddFlags(a, b, result);

            break;
        }

        // OPCODE 10 : SUB CONSTANT
        // Register[dest] = Register[src1] - signed_constant
        case 10:
        {
            int32_t a;
            int32_t b;
            int32_t result;

            a = Register[src1];
            b = (int8_t)src2;
            result = a - b;
            Register[dest] = result;
            updateSubFlags(a, b, result);

            break;
        }

        // OPCODE 11 : MUL CONSTANT
        case 11:
        {
            int32_t constant;
            constant = (int8_t)src2;
            Register[dest] = Register[src1] * constant;
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;
        }

        // OPCODE 12 : DIV CONSTANT
        case 12:
        {
            int32_t constant;
            constant = (int8_t)src2;
            if(constant == 0)
            {
                printf("Division by zero\n");
                end_of_simulation = 1;

                break;
            }
            Register[dest] = Register[src1] / constant;
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;
        }

        // OPCODE 13 : READ FROM CONSTANT MEMORY ADDRESS
        // Register[dest] = Data[address]
        case 13:
            Register[dest] = read32(src2);
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;

        // OPCODE 14 : WRITE TO CONSTANT MEMORY ADDRESS
        // Data[address] = Register[src1]
        case 14:
            write32(src2, Register[src1]);
            break;

        // OPCODE 15 : MOV SIGNED CONSTANT
        // Example:
        // x1 = -1
        // Compiler stores: 255
        // Processor converts 255 -> -1
        case 15:

            Register[dest] = (int8_t)src1;
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;

        // OPCODE 16 : BEQ : Branch if Zero Flag is set.
        case 16:
        {
            int offset;
            offset = (int8_t)src2;
            if(Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 17 : BNE
        case 17:
        {
            int offset;
            offset = (int8_t)src2;
            if(!Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 18 : BCS
        case 18:
        {
            int offset;
            offset = (int8_t)src2;
            if(C)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 19 : BCC
        case 19:
        {
            int offset;
            offset = (int8_t)src2;
            if(!C)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 20 : BMI
        case 20:
        {
            int offset;
            offset = (int8_t)src2;
            if(N)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 21 : BPL
        case 21:
        {
            int offset;
            offset = (int8_t)src2;
            if(!N)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 22 : BVS
        case 22:
        {
            int offset;
            offset = (int8_t)src2;
            if(V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 23 ; BVC
        case 23:
        {
            int offset;
            offset = (int8_t)src2;
            if(!V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 24 : BHI
        case 24:
        {
            int offset;
            offset = (int8_t)src2;
            if(C && !Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 25 : BLS
        case 25:
        {
            int offset;
            offset = (int8_t)src2;
            if(!C || Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 26 : BGE
        case 26:
        {
            int offset;

            offset = (int8_t)src2;

            if(N == V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 27 :BLT
        case 27:
        {
            int offset;
            offset = (int8_t)src2;
            if(N != V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 28 : BGT
        case 28:
        {
            int offset;
            offset = (int8_t)src2;
            if(!Z && (N == V))
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 29 : BLE
        case 29:
        {
            int offset;
            offset = (int8_t)src2;
            if(Z || (N != V))
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 30 : BAL : Unconditional branch.
        case 30:
        {
            int offset;
            offset = (int8_t)src2;
            PC = PC - 4 + (offset * 4);

            break;
        }

        // INVALID OPCODE
        default:
            printf("Invalid Opcode : %d\n", opcode);
            end_of_simulation = 1;

            break;
    }
}