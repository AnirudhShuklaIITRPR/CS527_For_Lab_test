#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>

#define NUM_REGISTERS 256
#define NUM_VREGISTERS 32
#define VREG_ELEMENTS 8                                  // 256-bit vector register = 8 x 32-bit lanes

extern int32_t Register[NUM_REGISTERS];                  // CPU REGISTERS
extern int32_t VRegister[NUM_VREGISTERS][VREG_ELEMENTS]; // VECTOR REGISTERS: 32 registers x 8 lanes x 32-bit
extern int PC;                                           // PROGRAM COUNTER
extern int Z, N, C, V;                                   // FLAGS
extern int end_of_simulation;                            // END FLAG

void reset(void);
void fetch(void);
void decode(void);
void execute(void);

#endif
