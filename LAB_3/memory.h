#ifndef MEMORY_H
#define MEMORY_H

// MEMORY SIZES
#define INSTRUCTION_MEM_SIZE 256
#define DATA_MEM_SIZE 4096

extern unsigned char Instruction[INSTRUCTION_MEM_SIZE];
extern unsigned char Data[DATA_MEM_SIZE];

void initialize(void);
int  read32(int address);
void write32(int address, int value);
void finalize(void);

#endif
