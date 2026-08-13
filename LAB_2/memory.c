#include <stdio.h>
#include "memory.h"

// MEMORY
unsigned char Instruction[256];  // Instruction Memory
unsigned char Data[4096];        // Data Memory

// INITIALIZE MEMORY
void initialize()
{
    FILE *fp;
    int value;
    int index;

    for(index = 0; index < 256; index++)
    {
        Instruction[index] = 0;
    }

    for(index = 0; index < 4096; index++)
    {
        Data[index] = 0;
    }

    fp = fopen("program.byte", "r");

    if(fp == NULL)
    {
        printf("Cannot open program.byte\n");
        return;
    }

    index = 0;
    while(fscanf(fp, "%x", &value) == 1)
    {
        if(index >= 256)
        {
            printf("Instruction memory overflow\n");
            break;
        }

        // Every value in program.byte represents one byte.
        if(value < 0 || value > 255)
        {
            printf("Invalid instruction byte: %d\n", value);

            fclose(fp);
            return;
        }
        Instruction[index] = (unsigned char)value;
        index++;
    }

    fclose(fp);

    extern char dataFile[];
    fp = fopen(dataFile, "r");
    if(fp == NULL)
    {
        printf("Cannot open %s\n", dataFile);
        // Create empty data memory file
        fp = fopen(dataFile, "w");
        if(fp == NULL)
        {
            printf("Cannot create %s\n", dataFile);
            return;
        }

        for(index = 0; index < 4096; index++)
        {
            fprintf(fp, "0\n");
        }

        fclose(fp);

        fp = fopen(dataFile, "r");
        if(fp == NULL)
        {
            printf("Cannot open %s\n", dataFile);
            return;
        }
    }


    index = 0;
    while(fscanf(fp, "%x", &value) == 1)
    {
        if(index >= 4096)
        {
            printf("Data memory overflow\n");
            break;
        }


        if(value < 0 || value > 255)
        {
            printf("Invalid data byte: %d\n", value);
            fclose(fp);
            return;
        }


        Data[index] = (unsigned char)value;
        index++;
    }
    fclose(fp);
    printf("Memory Initialized Successfully\n");
}

int read32(int address)
{
    if(address < 0 || address + 3 >= 4096)
    {
        printf("Invalid memory address: %d\n", address);
        return 0;
    }
    return Data[address]|(Data[address + 1] << 8)|(Data[address + 2] << 16)|(Data[address + 3] << 24);
}

void write32(int address, int value)
{
    if(address < 0 || address + 3 >= 4096)
    {
        printf("Invalid memory address: %d\n", address);
        return;
    }
    Data[address] = value & 0xFF;
    Data[address + 1] = (value >> 8) & 0xFF;
    Data[address + 2] = (value >> 16) & 0xFF;
    Data[address + 3] = (value >> 24) & 0xFF;
}

// FINALIZE
// Save Data Memory back to data.byte
void finalize(){
    FILE *fp;
    extern char dataFile[];
    fp = fopen(dataFile, "w");

    if(fp == NULL){
        printf("Cannot write %s\n", dataFile);

        return;
    }

    for(int i = 0; i < 4096; i++){
        fprintf(fp, "%02X\n", Data[i]);
    }

    fclose(fp);
    printf("Memory Written Successfully\n");

}