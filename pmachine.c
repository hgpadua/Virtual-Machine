//
//  main.c
//  pmachine
//
//  Created by Huey Padua on 10/9/17.
//  Copyright © 2017 Huey Padua. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

const char *opcodes[] = {"ILLEGAL", "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio"};

void stackTrace(int* stack, int SP, int BP, FILE* ofp);
void execute(instruction* irStruct, int* stack, int* sp, int* bp, int* pc);
void OPR (int *sp, int* bp, int *pc, int *stack, instruction* irStruct);
int base(int* bp, instruction* irStruct, int* stack);

FILE* ofp2;
FILE* ofp3;

void pmachine(void) {
    
    FILE* ifp;
    //FILE* ofp;
    
    ifp = fopen("parserout.txt", "r");
    //ofp = fopen("vmout.txt", "w");
    ofp2 = fopen("factorialOut2.txt", "w");
    ofp3 = fopen("factorialOut3.txt", "w");
    

    int i=0;
    int stack[MAX_STACK_HEIGHT] = {0};
    
    //specifications based on the homework
    int SP = 0;
    int BP = 1;
    int PC = 0;
    int IR = 0;
    
    //initializing array of structs
    instruction arrayStruct[CODE_SIZE];
    instruction *irStruct;
    
    while (fscanf(ifp, "%d %d %d", &arrayStruct[i].op, &arrayStruct[i].l, &arrayStruct[i].m) != EOF) {
        //fprintf(ofp, "%d\t%s\t%d\t%d\n", i, opcodes[arrayStruct[i].op], arrayStruct[i].l, arrayStruct[i].m);
        fprintf(ofp2, "%d\t%s\t%d\t%d\n", i, opcodes[arrayStruct[i].op], arrayStruct[i].l, arrayStruct[i].m);
        i++;
    }
    /*
    fprintf(ofp, "\n\n");
    fprintf(ofp, "\t\t\t\tpc \tbp \tsp \tstack\n");
    fprintf(ofp, "Initial values\t\t\t%d  \t%d \t%d \n", PC, BP, SP);
     */
    fprintf(ofp3, "Initial values\t\t\t%d  \t%d \t%d \n", PC, BP, SP);
    
    while (BP!=0) {
        
        irStruct=&arrayStruct[PC];

        fprintf(ofp3, "%d\t%s \t%d \t%d",PC, opcodes[irStruct->op], irStruct->l, irStruct->m);
        PC++;
        
        execute(irStruct, stack, &SP, &BP, &PC);
        
        fprintf(ofp3, "\t%d  \t%d \t%d \t", PC, BP, SP);
        fprintf(ofp3, "\n");
    }
    
    fclose(ifp);
    //fclose(ofp);
    fclose(ofp2);
    fclose(ofp3);
}
//function to print stack trace, recursively
void stackTrace(int* stack, int SP, int BP, FILE* ofp) {
    
    int i=0;

    if (BP==0) {
        return;
    }

    else if (BP==1) {
        
        for(i=1;i<=SP;i++){
           // fprintf(ofp, "%d ",stack[i]);
            fprintf(ofp3, "%d ",stack[i]);
        }
        return;
    }
    else {
        stackTrace(stack, BP-1, stack[BP+2], ofp);
        
        if (SP<BP) {
            fprintf(ofp, "| ");
            
            fprintf(ofp3, "| ");
            
            for (i=0;i<4;i++) {
                fprintf(ofp, "%d ", stack[BP+i]);
                
                fprintf(ofp3, "%d ", stack[BP+i]);
            }
        }
        else {
            fprintf(ofp, "| ");
            
            fprintf(ofp3, "| ");
            for (i=BP;i<=SP;i++) {
                fprintf(ofp, "%d ", stack[i]);
                
                fprintf(ofp3, "%d ", stack[i]);
            }
        }
        return;
    }
}

//execute cycle function
void execute(instruction* irStruct, int* stack, int* sp, int* bp, int* pc) {
    
    switch(irStruct->op) {
        case 1: //LIT
            *sp=*sp+1;
            stack[*sp]=irStruct->m;
            break;
        case 2: //OPR function
            OPR(sp, bp, pc, stack, irStruct);
            break;
        case 3: //LOD
            *sp=*sp+1;
            stack[*sp]=stack[base(bp, irStruct, stack)+irStruct->m];
            break;
        case 4: //STO
            stack[base(bp, irStruct, stack)+irStruct->m]=stack[*sp];
            *sp=*sp-1;
            break;
        case 5: //CAL
            stack[*sp+1]=0; //space to return value
            stack[*sp+2]=base(bp, irStruct, stack); //static link (SL)
            stack[*sp+3]=*bp; //dynamic link (DL)
            stack[*sp+4]=*pc; //return address (RA)
            *bp=*sp+1;
            *pc=irStruct->m;
            break;
        case 6: //INC
            *sp=*sp+irStruct->m;
            break;
        case 7: //JMP
            *pc=irStruct->m;
            break;
        case 8: //JPC
            if (stack[*sp]==0) {
                *pc=irStruct->m;
            }
            *sp=*sp-1;
            break;
        case 9: //SIO1
            printf("%d\n", stack[*sp]);
            *sp=*sp-1;
            break;
        case 10: //SIO2
            *sp=*sp+1;
            scanf("%d", &stack[*sp]);
            break;
        default:
            printf("Illegal OPR!\n");
    }
}

// OPR to pass to execute
void OPR (int *sp, int* bp, int *pc, int *stack, instruction* irStruct) {
    
    switch (irStruct->m) {
            
        case 0: //RET
            *sp=*bp-1;
            *pc=stack[*sp+4];
            *bp=stack[*sp+3];
            break;
        case 1: //NEG
            stack[*sp]=-stack[*sp];
            break;
        case 2: //ADD
            *sp=*sp-1;
            stack[*sp]=stack[*sp]+stack[*sp+1];
            break;
        case 3: //SUB
            *sp=*sp-1;
            stack[*sp]=stack[*sp]-stack[*sp+1];
            break;
        case 4: //MUL
            *sp=*sp-1;
            stack[*sp]=stack[*sp]*stack[*sp+1];
            break;
        case 5: //DIV
            *sp=*sp-1;
            stack[*sp]=stack[*sp]/stack[*sp+1];
            break;
        case 6: //ODD
            stack[*sp]=stack[*sp]%2;
            break;
        case 7: //MOD
            *sp=*sp-1;
            stack[*sp]=stack[*sp]%stack[*sp+1];
            break;
        case 8: //EQL
            *sp=*sp-1;
            stack[*sp]=stack[*sp]==stack[*sp+1];
            break;
        case 9: //NEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]!=stack[*sp+1];
            break;
        case 10: //LSS
            *sp=*sp-1;
            stack[*sp]=stack[*sp]<stack[*sp+1];
            break;
        case 11: //LEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]<=stack[*sp+1];
            break;
        case 12: //GTR
            *sp=*sp-1;
            stack[*sp]=stack[*sp]>stack[*sp+1];
            break;
        case 13: //GEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]>=stack[*sp+1];
            break;
    }
}
// function to find Base levels down
int base(int* bp, instruction* irStruct, int* stack) {
    int l = irStruct->l;
    int b1; //find base L levels down
    b1 = *bp;
    while (l>0) {
        b1=stack[b1+1];
        l--;
    }
    return b1;
}
