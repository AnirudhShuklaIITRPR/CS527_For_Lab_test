#include <stdio.h>
#include "memory.h"

int Instruction[256];   // Instruction Memory
int Data[256];          // Data Memory

void initialize()
{
    FILE *fp;
    int value;
    int index = 0;

    // Initialize memories 
    for(int i=0;i<256;i++)
    {
        Instruction[i]=0;
        Data[i]=0;
    }

    // Load Instruction Memory 
    fp = fopen("program.byte","r");

    if(fp==NULL)
    {
        printf("Cannot open program.byte\n");
        return;
    }

    while(fscanf(fp,"%d",&value)==1 && index<256)
    {
        Instruction[index++] = value;
    }

    fclose(fp);

    /* Load Data Memory */
    fp = fopen("data.byte","r");

    if(fp==NULL)
    {
        printf("Cannot open data.byte\n");

        /* Create empty file */
        fp = fopen("data.byte","w");

        for(int i=0;i<256;i++)
            fprintf(fp,"0\n");

        fclose(fp);
        fp = fopen("data.byte","r");
    }

    index=0;

    while(fscanf(fp,"%d",&value)==1 && index<256)
    {
        Data[index++] = value;
    }

    fclose(fp);

    printf("Memory Initialized Successfully\n");
}

// Save Data Memory 
void finalize()
{
    FILE *fp;

    fp = fopen("data.byte","w");

    if(fp==NULL)
    {
        printf("Cannot write data.byte\n");
        return;
    }

    for(int i=0;i<256;i++)
    {
        fprintf(fp,"%d\n",Data[i]);
    }

    fclose(fp);

    printf("Memory Written Successfully\n");
}