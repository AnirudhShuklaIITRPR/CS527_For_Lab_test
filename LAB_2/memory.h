#ifndef MEMORY_H
#define MEMORY_H

extern unsigned char Instruction[256];
extern unsigned char Data[4096];

void initialize();
void finalize();

int read32(int address);
void write32(int address, int value);

#endif