#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compiler.h"

int getRegister(char *str)
{
    if (str[0] == 'x' || str[0] == 'X')
        return atoi(str + 1);

    return atoi(str);
}

int isRegister(char *str)
{
    return (str[0] == 'x' || str[0] == 'X');
}

void removeComment(char *line)
{
    char *comment = strchr(line, '%');

    if (comment != NULL)
        *comment = '\0';
}

char *skipSpaces(char *str)
{
    while (*str == ' ' || *str == '\t')
        str++;

    return str;
}

int isLabel(char *line)
{
    line = skipSpaces(line);

    return (line[0] == '.');
}

typedef struct
{
    char name[50];
    int instructionNumber;
} Label;

int getBranchOpcode(char *branch)
{
    if (strcmp(branch, "BEQ") == 0) return 0x10;
    if (strcmp(branch, "BNE") == 0) return 0x11;
    if (strcmp(branch, "BCS") == 0) return 0x12;
    if (strcmp(branch, "BCC") == 0) return 0x13;
    if (strcmp(branch, "BMI") == 0) return 0x14;
    if (strcmp(branch, "BPL") == 0) return 0x15;
    if (strcmp(branch, "BVS") == 0) return 0x16;
    if (strcmp(branch, "BVC") == 0) return 0x17;
    if (strcmp(branch, "BHI") == 0) return 0x18;
    if (strcmp(branch, "BLS") == 0) return 0x19;
    if (strcmp(branch, "BGE") == 0) return 0x1A;
    if (strcmp(branch, "BLT") == 0) return 0x1B;
    if (strcmp(branch, "BGT") == 0) return 0x1C;
    if (strcmp(branch, "BLE") == 0) return 0x1D;
    if (strcmp(branch, "BAL") == 0) return 0x1E;

    return -1;
}

int findLabel(Label labels[], int labelCount, char *name)
{
    int i;

    for (i = 0; i < labelCount; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].instructionNumber;
    }

    return -1;
}

void compile()
{
    FILE *src;
    FILE *out;

    char line[200];
    extern char programFile[];

    Label labels[100];

    int labelCount = 0;
    int instructionCount = 0;

    src = fopen(programFile, "r");

    if (src == NULL)
    {
        printf("Cannot open %s\n", programFile);
        return;
    }

    // PASS 1
    while (fgets(line, sizeof(line), src))
    {
        char temp[200];

        strcpy(temp, line);

        removeComment(temp);
        temp[strcspn(temp, "\n")] = '\0';

        char *ptr = skipSpaces(temp);

        if (strlen(ptr) == 0)
            continue;

        if (isLabel(ptr))
        {
            if (labelCount >= 100)
            {
                printf("Too many labels\n");
                fclose(src);
                return;
            }

            strcpy(labels[labelCount].name, ptr);
            labels[labelCount].instructionNumber = instructionCount;
            labelCount++;
            continue;
        }
        instructionCount++;
    }

    fclose(src);

    // PASS 2
    src = fopen(programFile, "r");

    if (src == NULL)
    {
        printf("Cannot open %s\n", programFile);
        return;
    }

    out = fopen("program.byte", "w");

    if (out == NULL)
    {
        printf("Cannot create program.byte\n");
        fclose(src);
        return;
    }

    int currentInstruction = 0;

    while (fgets(line, sizeof(line), src))
    {
        char dest[30];
        char op1[30];
        char op2[30];

        char op;

        char branch[30];
        char labelName[50];

        removeComment(line);
        line[strcspn(line, "\n")] = '\0';

        char *ptr = skipSpaces(line);

        if (strlen(ptr) == 0)
            continue;

        if (isLabel(ptr))
            continue;

        // MEMORY READ
        if (sscanf(ptr, "%s = [%[^]]]", dest, op1) == 2)
        {
            if (isRegister(op1))
            {
                fprintf(out, "%X %X %X %X\n", 0x05, getRegister(dest), 0, getRegister(op1));
            }
            else
            {
                int address = atoi(op1);

                if (address < 0 || address > 4095)
                {
                    printf("Invalid memory address: %d\n", address);
                    fclose(src);
                    fclose(out);
                    return;
                }

                fprintf(out, "%X %X %X %X\n", 0x0D, getRegister(dest), 0, address);
            }
            currentInstruction++;
            continue;
        }

        // MEMORY WRITE
        if (sscanf(ptr, "[%[^]]] = %s", op1, op2) == 2)
        {
            if (isRegister(op1) && isRegister(op2))
            {
                fprintf(out, "%X %X %X %X\n", 0x06, getRegister(op1), 0, getRegister(op2));
            }
            else
            {
                int address = atoi(op1);

                if (address < 0 || address > 4095)
                {
                    printf("Invalid memory address: %d\n", address);
                    fclose(src);
                    fclose(out);
                    return;
                }

                fprintf(out, "%X %X %X %X\n", 0x0E, 0, getRegister(op2), address);
            }

            currentInstruction++;
            continue;
        }

        // BRANCH
        if (sscanf(ptr, "%s %s", branch, labelName) == 2)
        {
            int branchOpcode = getBranchOpcode(branch);

            if (branchOpcode != -1)
            {
                int targetInstruction;
                int offset;
                int branchOffset;

                targetInstruction = findLabel(labels, labelCount, labelName);

                if (targetInstruction == -1)
                {
                    printf("Unknown label: %s\n", labelName);
                    fclose(src);
                    fclose(out);
                    return;
                }

                offset = targetInstruction - currentInstruction;

                if (offset < -128 || offset > 127)
                {
                    printf(
                        "Branch offset out of range: %d\n",
                        offset
                    );

                    fclose(src);
                    fclose(out);
                    return;
                }

                branchOffset = offset & 0xFF;
                fprintf(out, "%X %X %X %02X\n", branchOpcode, 0, 0, branchOffset);

                currentInstruction++;
                continue;
            }
        }

        // READ
        if (sscanf(ptr, "Read %[^,], %s", dest, op1) == 2)
        {
            fprintf(out, "%X %X %X %X\n", 0x05, getRegister(dest), atoi(op1), 0);

            currentInstruction++;
            continue;
        }

        // WRITE
        if (sscanf(ptr, "Write %[^,], %s", dest, op1) == 2)
        {
            fprintf(out, "%X %X %X %X\n", 0x06, getRegister(dest), atoi(op1), 0);

            currentInstruction++;
            continue;
        }

        // ARITHMETIC
        if (sscanf(ptr, "%s = %s %c %s", dest, op1, &op, op2) == 4)
        {
            int opcode = 0;

            if (isRegister(op1) && isRegister(op2))
            {
                switch (op)
                {
                    case '+':
                        opcode = 0x01;
                        break;

                    case '-':
                        opcode = 0x02;
                        break;

                    case '*':
                        opcode = 0x03;
                        break;

                    case '/':
                        opcode = 0x04;
                        break;
                }

                if (opcode != 0)
                {
                    fprintf(out, "%X %X %X %X\n", opcode, getRegister(dest), getRegister(op1), getRegister(op2));
                }
            }
            else if (isRegister(op1) && !isRegister(op2))
            {
                int constant = atoi(op2);

                switch (op)
                {
                    case '+':
                        opcode = 0x09;
                        break;

                    case '-':
                        opcode = 0x0A;
                        break;

                    case '*':
                        opcode = 0x0B;
                        break;

                    case '/':
                        opcode = 0x0C;
                        break;
                }

                if (opcode != 0)
                {
                    if (constant < -128 || constant > 255)
                    {
                        printf("Invalid constant: %d\n", constant);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %02X\n", opcode, getRegister(dest), getRegister(op1), constant & 0xFF);
                }
            }

            currentInstruction++;
            continue;
        }

        // MOV CONSTANT
        if (sscanf(ptr, "%s = %s", dest, op1) == 2)
        {
            if (!isRegister(op1))
            {
                int constant = atoi(op1);

                if (constant < -128 || constant > 255)
                {
                    printf("Invalid constant: %d\n", constant);

                    fclose(src);
                    fclose(out);
                    return;
                }

                fprintf(out, "%X %X %X %X\n", 0x0F, getRegister(dest), constant & 0xFF, 0);
            }

            currentInstruction++;
            continue;
        }

        printf("Cannot understand line: %s\n", ptr);
    }

    fprintf(out, "0 0 0 0\n");

    fclose(src);
    fclose(out);

    printf("Compilation Done Successfully\n");
}