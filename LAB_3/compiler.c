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

int isVectorRegister(char *str)
{
    return (str[0] == 'v' || str[0] == 'V');
}

int getVecRegister(char *str)
{
    return atoi(str + 1);
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

int getVectorArithOpcode(char op, int kind)
{
    switch (op)
    {
        case '+':

            if (kind == 0)
                return 0x21;

            if (kind == 1)
                return 0x29;

            if (kind == 2)
                return 0x2F;

            break;

        case '-':

            if (kind == 0)
                return 0x22;

            if (kind == 1)
                return 0x2A;

            if (kind == 2)
                return 0x30;

            break;

        case '*':

            if (kind == 0)
                return 0x23;

            if (kind == 1)
                return 0x2B;

            if (kind == 2)
                return 0x31;

            break;
    }

    return 0;
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


int validImmediate(int value)
{
    return (value >= -128 && value <= 127);
}

void compile(void)
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
    while (fgets(line, sizeof(line), src)){
        char temp[200];
        char *ptr;

        strcpy(temp, line);

        removeComment(temp);

        temp[strcspn(temp, "\n")] = '\0';

        ptr = skipSpaces(temp);

        /* Empty line */
        if (strlen(ptr) == 0)
            continue;

        /* Label */
        if (isLabel(ptr)){
            if (labelCount >= 100){
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

        char branch[30];
        char labelName[50];
        char op;
        char *ptr;

        removeComment(line);
        line[strcspn(line, "\n")] = '\0';
        ptr = skipSpaces(line);

        if (strlen(ptr) == 0)
            continue;

        if (isLabel(ptr))
            continue;

        if (sscanf(ptr, "%s = [%[^]]]", dest, op1) == 2){
            if (isVectorRegister(dest)){

                if (isRegister(op1)){
                    fprintf(out, "%X %X %X %X\n", 0x25, getVecRegister(dest), 0, getRegister(op1));
                }
                else{

                    int address = atoi(op1);
                    if (address < 0 || address > 255){
                        printf("Invalid constant memory address (must be 0-255): %d\n", address);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %X\n", 0x2D, getVecRegister(dest), 0, address);
                }

                currentInstruction++;

                continue;
            }

            if (isRegister(op1)){
                fprintf(out, "%X %X %X %X\n", 0x05, getRegister(dest), 0, getRegister(op1));
            }
            else{
                int address = atoi(op1);

                if (address < 0 || address > 255){
                    printf("Invalid constant memory address " "(must be 0-255): %d\n", address);

                    fclose(src);
                    fclose(out);
                    return;
                }

                fprintf(out, "%X %X %X %X\n", 0x0D, getRegister(dest), 0, address);
            }
            currentInstruction++;

            continue;
        }

        if (sscanf(ptr, "[%[^]]] = %s", op1, op2) == 2){

            if (isVectorRegister(op2)){
                if (isRegister(op1)){

                    fprintf(out, "%X %X %X %X\n", 0x26, 0, getVecRegister(op2), getRegister(op1));
                }
                else{
                    int address = atoi(op1);

                    if (address < 0 || address > 255){
                        printf("Invalid constant memory address (must be 0-255): %d\n", address);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %X\n", 0x2E, 0, getVecRegister(op2), address);
                }

                currentInstruction++;

                continue;
            }

            if (isRegister(op1) && isRegister(op2)){

                fprintf(out, "%X %X %X %X\n", 0x06, getRegister(op1), 0, getRegister(op2));
            }
            else{
                int address = atoi(op1);

                if (address < 0 || address > 255){
                    printf("Invalid constant memory address (must be 0-255): %d\n", address);

                    fclose(src);
                    fclose(out);
                    return;
                }

                fprintf(
                    out,
                    "%X %X %X %X\n",
                    0x0E,
                    0,
                    getRegister(op2),
                    address
                );
            }

            currentInstruction++;

            continue;
        }

        if (sscanf(ptr, "%s %s", branch, labelName) == 2){
            int branchOpcode;
            branchOpcode = getBranchOpcode(branch);
            if (branchOpcode != -1){
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

                if (offset < -128 || offset > 127){
                    printf("Branch offset out of range: %d\n", offset);

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

        if (sscanf(ptr, "Read %[^,], %s", dest, op1) == 2){
            fprintf(out, "%X %X %X %X\n", 0x05, getRegister(dest), atoi(op1), 0);

            currentInstruction++;
            continue;
        }

        if (sscanf(ptr, "Write %[^,], %s", dest, op1) == 2){
            fprintf(out, "%X %X %X %X\n", 0x06, getRegister(dest), atoi(op1), 0);

            currentInstruction++;

            continue;
        }


        if (sscanf(ptr, "%s = %s %c %s", dest, op1, &op, op2) == 4)
{
            int opcode = 0;


            if (isVectorRegister(dest))
            {

                if (!isVectorRegister(op1))
                {
                    printf("Invalid vector operation (first operand must be a vector register): %s\n",ptr);

                    fclose(src);
                    fclose(out);
                    return;
                }

                if (isVectorRegister(op2)){
                    opcode = getVectorArithOpcode(op, 0);

                    if (opcode == 0){
                        printf( "Invalid vector operator: %c\n", op);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %X\n", opcode, getVecRegister(dest), getVecRegister(op1), getVecRegister(op2));
                }

                else if (isRegister(op2)){
                    opcode = getVectorArithOpcode(op, 2);

                    if (opcode == 0){
                        printf("Invalid vector operator: %c\n", op);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %X\n", opcode, getVecRegister(dest), getVecRegister(op1), getRegister(op2));
                }

                else{
                    int constant;

                    constant = atoi(op2);

                    if (!validImmediate(constant))
                    {
                        printf( "Invalid vector constant (must be -128 to 127): %d\n", constant);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    opcode = getVectorArithOpcode(op, 1);

                    if (opcode == 0)
                    {
                        printf("Invalid vector operator: %c\n", op);

                        fclose(src);
                        fclose(out);
                        return;
                    }

                    fprintf(out, "%X %X %X %02X\n", opcode, getVecRegister(dest), getVecRegister(op1), constant & 0xFF);
                }


                currentInstruction++;

                continue;
            }

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

                    default:
                        printf("Invalid scalar operator: %c\n", op);

                        fclose(src);
                        fclose(out);
                        return;
                }

                fprintf(out, "%X %X %X %X\n", opcode, getRegister(dest), getRegister(op1), getRegister(op2));
            }

            else if (isRegister(op1) && !isRegister(op2)){
                int constant;

                constant = atoi(op2);


                if (!validImmediate(constant)){
                    printf("Invalid scalar constant " "(must be -128 to 127): %d\n", constant);

                    fclose(src);
                    fclose(out);
                    return;
                }


                switch (op){
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

                    default:
                        printf(
                            "Invalid scalar operator: %c\n",
                            op
                        );

                        fclose(src);
                        fclose(out);
                        return;
                }

                fprintf(out, "%X %X %X %02X\n", opcode, getRegister(dest), getRegister(op1), constant & 0xFF);
            }

            else{
                printf("Invalid scalar arithmetic: %s\n", ptr);

                fclose(src);
                fclose(out);
                return;
            }


            currentInstruction++;

            continue;
        }

        if (sscanf(ptr, "%s = %s", dest, op1) == 2){
            if (isVectorRegister(dest)){
                printf("Vector registers cannot be initialized with scalar constants: %s\n", ptr);

                fclose(src);
                fclose(out);
                return;
            }


            if (!isRegister(op1) && !isVectorRegister(op1)){
                int constant;
                constant = atoi(op1);


                if (!validImmediate(constant)){
                    printf("Invalid constant (must be -128 to 127): %d\n", constant);

                    fclose(src);
                    fclose(out);
                    return;
                }


                fprintf(out, "%X %X %X %X\n", 0x0F, getRegister(dest), constant & 0xFF, 0);
                currentInstruction++;

                continue;
            }

            // Register-to-register MOV is not supported by the current instruction set.
            if (isRegister(op1))
            {
                printf("Register-to-register MOV is not supported: %s\n", ptr);

                fclose(src);
                fclose(out);
                return;
            }

            printf("Unsupported MOV instruction: %s\n", ptr);

            fclose(src);
            fclose(out);
            return;
        }

        printf("Cannot understand line: %s\n", ptr);

        fclose(src);
        fclose(out);

        return;
    }

    fprintf(out, "0 0 0 0\n");


    fclose(src);
    fclose(out);

    printf("\nCOMPILATION DONE SUCCESSFULLY\n");
}