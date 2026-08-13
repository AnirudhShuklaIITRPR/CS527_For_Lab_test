#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compiler.h"

// atoi: It is a function from stdlib.h that converts a string into an integer (ASCII to Integer).

int getRegister(char *str)  // This function converts register names into register numbers.
{
    if(str[0]=='x' || str[0]=='X')
        return atoi(str+1);   
        
    return atoi(str);
}

void compile()   // program.txt -> Compiler -> program.byte
{
    FILE *src;
    FILE *out;

    char line[100];

    src = fopen("program.txt","r");

    if(src==NULL)
    {
        printf("Cannot open program.txt\n");
        return;
    }

    out = fopen("program.byte","w");

    while(fgets(line,sizeof(line),src))   // Reading one line at a time
    {
        char dest[20];
        char op1[20];
        char op2[20];
        char op;

        // ---------- READ ----------
        if(sscanf(line,"Read %[^,], %s",dest,op1)==2)
        {
            fprintf(out,"5 %d %d 0\n",
                    getRegister(dest),
                    atoi(op1));
        }

        // ---------- WRITE ---------- 
        else if(sscanf(line,"Write %[^,], %s",dest,op1)==2)
        {
            fprintf(out,"6 %d %d 0\n",
                    getRegister(dest),
                    atoi(op1));
        }

        // ---------- DATA MOVEMENT ---------- 
        else if(sscanf(line,"%s = %s",dest,op1)==2)
        {
            if(op1[0]!='x')
            {
                fprintf(out,"7 %d %d 0\n",
                        getRegister(dest),
                        atoi(op1));
            }
        }

        // ---------- ARITHMETIC ----------
        if(sscanf(line,"%s = %s %c %s",dest,op1,&op,op2)==4)
        {
            int opcode=0;

            switch(op)
            {
                case '+':
                    opcode=1;
                    break;

                case '-':
                    opcode=2;
                    break;

                case '*':
                    opcode=3;
                    break;

                case '/':
                    opcode=4;
                    break;
            }

            if(opcode!=0)
            {
                fprintf(out,"%d %d %d %d\n",
                        opcode,
                        getRegister(dest),
                        getRegister(op1),
                        getRegister(op2));
            }
        }
    }

    // End of Program 
    fprintf(out,"0 0 0 0\n");

    fclose(src);
    fclose(out);

    printf("Compilation Successful\n");
}