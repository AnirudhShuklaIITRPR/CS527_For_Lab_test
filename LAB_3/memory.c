#include <stdio.h>
#include <stdint.h>

#include "memory.h"

unsigned char Instruction[256];
unsigned char Data[4096];

void initialize(){
    FILE *fp;
    int value;
    int index;

    for(index = 0; index < 256; index++)
        Instruction[index] = 0;

    for(index = 0; index < 4096; index++)
        Data[index] = 0;

    fp = fopen("program.byte", "r");

    if(fp == NULL){
        printf("Cannot open program.byte\n");
        return;
    }

    index = 0;

    while(fscanf(fp, "%x", &value) == 1){
        if(index >= 256){
            printf("Instruction memory overflow\n");
            fclose(fp);
            return;
        }

        if(value < 0 || value > 255){
            printf("Invalid instruction byte: %X\n", value);
            fclose(fp);
            return;
        }

        Instruction[index] = (unsigned char)value;
        index++;
    }

    fclose(fp);

    extern char dataFile[];

    fp = fopen(dataFile, "r");

    if(fp == NULL){
        printf("Cannot open %s\n", dataFile);
        return;
    }

    index = 0;

    while(fscanf(fp, "%x", &value) == 1){
        if(index >= 4096){
            printf("Data memory overflow\n");
            fclose(fp);
            return;
        }

        if(value < 0 || value > 255){
            printf("Invalid data byte: %X\n", value);
            fclose(fp);
            return;
        }

        Data[index] = (unsigned char)value;
        index++;
    }

    fclose(fp);

    printf("\nMEMORY INTIALIZED SUCCESSFULLY\n");
}

int read32(int address){
    uint32_t value;

    if(address < 0 || address + 3 >= 4096){
        printf("Invalid memory address: %d\n", address);
        return 0;
    }

    value = ((uint32_t)Data[address]) | ((uint32_t)Data[address + 1] << 8) | 
    ((uint32_t)Data[address + 2] << 16) | ((uint32_t)Data[address + 3] << 24);

    return (int32_t)value;
}

void write32(int address, int value){
    uint32_t v;

    if(address < 0 || address + 3 >= 4096){
        printf("Invalid memory address: %d\n", address);
        return;
    }

    v = (uint32_t)value;

    Data[address]     = (unsigned char)(v & 0xFF);
    Data[address + 1] = (unsigned char)((v >> 8) & 0xFF);
    Data[address + 2] = (unsigned char)((v >> 16) & 0xFF);
    Data[address + 3] = (unsigned char)((v >> 24) & 0xFF);
}

void finalize(){
    FILE *fp;
    int i;

    extern char dataFile[];

    fp = fopen(dataFile, "w");

    if(fp == NULL){
        printf("Cannot write %s\n", dataFile);
        return;
    }

    for(i = 0; i < 4096; i += 4){
        fprintf(fp, "%02X %02X %02X %02X\n", Data[i], Data[i + 1], Data[i + 2], Data[i + 3]);
    }

    fclose(fp);

    printf("\nMEMORY FINALIZED SUCCESSFULLY\n");
}