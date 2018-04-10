//
//  main.c
//  compiler driver
//
//  Created by Huey Padua on 10/9/17.
//  Copyright © 2017 Huey Padua. All rights reserved.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

//global variables for flagging each argument
int lexoutput;
int codegenoutput;
int vmoutput;
int compile;

int c;
FILE* ifp;

const char *lex_symbols[] = { "", "nulsym", "identsym", "numbersym", "plussym", "minussym",
    "multsym",  "slashsym", "oddsym", "eqsym", "neqsym", "lessym", "leqsym",
    "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym",
    "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym",
    "whilesym", "dosym", "callsym", "constsym", "varsym", "procsym", "writesym",
    "readsym" , "elsesym"};

int main(int argc, char *argv[]) {
    
    char** temp=argv;
    
    /*
    -l : print the list of lexemes/tokens (scanner output) to the screen
    
    -a : print the generated assembly code (parser/codegen output) to the screen
    
    -v : print virtual machine execution trace (virtual machine output) to the screen
     */

    while (argc > 1) {
        if (strcmp(temp[1], "-l") == 0) {
            lexoutput = 1;
        }
        else if (strcmp(temp[1], "-a") == 0) {
            codegenoutput = 1;
        }
        else if (strcmp(temp[1], "-v") == 0) {
            vmoutput = 1;
        }
        else if (strcmp(temp[1], "-l-a-v") == 0) {
            lexoutput=1;
            codegenoutput=1;
            vmoutput=1;
        }
        argc--;
        temp++;
    }
    if (argc==1) {
        //lexoutput=1;
        //vmoutput=1;
        compile=1;
    }
    driver();
}

//links all files
void driver() {
    
    //run lexical, parser, and pmachine
    lexical();
    
    parser();

    pmachine();
    
    if (lexoutput == 1) {

        printf("\n");
        ifp = fopen("lexoutput.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(ifp);
        
        int i;
        ifp = fopen("lexoutput.txt", "r");
        //prints out variable names in Lexeme List
        for(i=0;i<lexListIndex;i++){
            printf("%s ", lex_symbols[lexList[i].token]);
            if(lexList[i].token==2){
                printf("%s ", lexList[i].name);
            }
            else if(lexList[i].token==3){
                printf("%d ",lexList[i].numberValue);
            }
        }
        printf("\n\n");
        fclose(ifp);
    }
    
    if (codegenoutput == 1) {
        
        ifp = fopen("factorialOut2.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n");
        fclose(ifp);
    }

    if (vmoutput == 1) {
        
        ifp = fopen("factorialOut3.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n");
        fclose(ifp);
    }
    if (compile == 1) {
        printf("\n");
        ifp = fopen("lexoutput.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(ifp);
        
        int i;
        ifp = fopen("lexoutput.txt", "r");
        //prints out variable names in Lexeme List
        for(i=0;i<lexListIndex;i++){
            printf("%s ", lex_symbols[lexList[i].token]);
            if(lexList[i].token==2){
                printf("%s ", lexList[i].name);
            }
            else if(lexList[i].token==3){
                printf("%d ",lexList[i].numberValue);
            }
        }
        printf("\n\n");
        fclose(ifp);
        
        if (errors==0) {
            printf("No errors, program is syntactically correct\n\n");
        }
        
        ifp = fopen("parserout.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(ifp);

    }
}
