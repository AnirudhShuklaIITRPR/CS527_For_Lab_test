#include <stdio.h>
#include <stdint.h>

#include "processor.h"
#include "memory.h"

int32_t Register[NUM_REGISTERS];                    // CPU REGISTERS
int32_t VRegister[NUM_VREGISTERS][VREG_ELEMENTS];   // VECTOR REGISTERS
int PC;                                             // PROGRAM COUNTER

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
    int i, j;
    for(i = 0; i < NUM_REGISTERS; i++)
        Register[i] = 0;

    for(i = 0; i < NUM_VREGISTERS; i++)
        for(j = 0; j < VREG_ELEMENTS; j++)
            VRegister[i][j] = 0;

    PC = 0;

    Z = 0;
    N = 0;
    C = 0;
    V = 0;

    end_of_simulation = 0;

    printf("\nPROCCESSOR RESET SUCCESSFULLY\n");
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


        // OPCODE 5 : READ (register indexed)
        // Register[dest] = Data[Register[src2]]
        case 5:
            Register[dest] = read32(Register[src2]);
            
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);
            
            break;

        // OPCODE 6 : WRITE (register indexed)
        // Data[Register[dest]] = Register[src2]
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
        // Register[dest] = Data[address]        (address = src2, 0-255)
        case 13:
            Register[dest] = read32(src2);
            Z = (Register[dest] == 0);
            N = (Register[dest] < 0);

            break;

        // OPCODE 14 : WRITE TO CONSTANT MEMORY ADDRESS
        // Data[address] = Register[src1]         (address = src2, 0-255)
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

        // OPCODE 21 : BPL (renamed to avoid clash — kept only if
        // opcode range didn't overlap: see note below on vector opcodes)
        case 0x15:
        {
            int offset;
            offset = (int8_t)src2;
            if(!N)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x16 : BVS
        case 0x16:
        {
            int offset;
            offset = (int8_t)src2;
            if(V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x17 ; BVC
        case 0x17:
        {
            int offset;
            offset = (int8_t)src2;
            if(!V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x18 : BHI
        case 0x18:
        {
            int offset;
            offset = (int8_t)src2;
            if(C && !Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x19 : BLS
        case 0x19:
        {
            int offset;
            offset = (int8_t)src2;
            if(!C || Z)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x1A : BGE
        case 0x1A:
        {
            int offset;

            offset = (int8_t)src2;

            if(N == V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x1B : BLT
        case 0x1B:
        {
            int offset;
            offset = (int8_t)src2;
            if(N != V)
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x1C : BGT
        case 0x1C:
        {
            int offset;
            offset = (int8_t)src2;
            if(!Z && (N == V))
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x1D : BLE
        case 0x1D:
        {
            int offset;
            offset = (int8_t)src2;
            if(Z || (N != V))
            {
                PC = PC - 4 + (offset * 4);
            }

            break;
        }

        // OPCODE 0x1E : BAL : Unconditional branch.
        case 0x1E:
        {
            int offset;
            offset = (int8_t)src2;
            PC = PC - 4 + (offset * 4);

            break;
        }

        // VECTOR INSTRUCTIONS
        // OPCODE 0x21 : VADD  (v_dest = v_src1 + v_src2, lane-wise)
        case 0x21:
        {
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] + VRegister[src2][i];
            break;
        }

        // OPCODE 0x22 : VSUB
        case 0x22:
        {
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] - VRegister[src2][i];
            break;
        }

        // OPCODE 0x23 : VMUL
        case 0x23:
        {
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] * VRegister[src2][i];
            break;
        }

        // OPCODE 0x25 : VLOAD (register indexed)
        // v_dest[i] = Data[Register[src2] + 4*i], then Register[src2] += 32
        case 0x25:
        {
            int addr = Register[src2];
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
            {
                VRegister[dest][i] = read32(addr);
                addr += 4;
            }
            Register[src2] = addr;
            break;
        }

        // OPCODE 0x26 : VSTORE (register indexed)
        // Data[Register[src2] + 4*i] = v_src1[i], then Register[src2] += 32
        case 0x26:
        {
            int addr = Register[src2];
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
            {
                write32(addr, VRegister[src1][i]);
                addr += 4;
            }
            Register[src2] = addr;
            break;
        }

        // OPCODE 0x29 : VADDI (v_dest = v_src1 + constant, broadcast)
        case 0x29:
        {
            int32_t constant = (int8_t)src2;
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] + constant;
            break;
        }

        // OPCODE 0x2A : VSUBI
        case 0x2A:
        {
            int32_t constant = (int8_t)src2;
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] - constant;
            break;
        }

        // OPCODE 0x2B : VMULI
        case 0x2B:
        {
            int32_t constant = (int8_t)src2;
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] * constant;
            break;
        }

        // OPCODE 0x2D : VLOADI (constant address, 0-255)
        case 0x2D:
        {
            int addr = src2;
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
            {
                VRegister[dest][i] = read32(addr);
                addr += 4;
            }
            break;
        }

        // OPCODE 0x2E : VSTOREI (constant address, 0-255)
        case 0x2E:
        {
            int addr = src2;
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
            {
                write32(addr, VRegister[src1][i]);
                addr += 4;
            }
            break;
        }

        // OPCODE 0x2F : VADDS (v_dest = v_src1 + Register[src2], broadcast scalar register)
        case 0x2F:
        {
            int32_t scalar = Register[src2];
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] + scalar;
            break;
        }

        // OPCODE 0x30 : VSUBS
        case 0x30:
        {
            int32_t scalar = Register[src2];
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] - scalar;
            break;
        }

        // OPCODE 0x31 : VMULS
        case 0x31:
        {
            int32_t scalar = Register[src2];
            int i;
            for(i = 0; i < VREG_ELEMENTS; i++)
                VRegister[dest][i] = VRegister[src1][i] * scalar;
            break;
        }

        // INVALID OPCODE
        default:
            printf("Invalid Opcode : %d\n", opcode);
            end_of_simulation = 1;

            break;
    }
}