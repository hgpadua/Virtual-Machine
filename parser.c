//
//  main.c
//  parser
//
//  Created by Huey Padua on 10/9/17.
//  Copyright © 2017 Huey Padua. All rights reserved.
//HW #4 implemented in parser
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data.h"

typedef struct {
    int kind; // const = 1, var = 2, proc = 3
    char name[10];	// name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
} symbol;

FILE* ifp;
FILE* ofp;

void block(int lvl, int tx, symbol* table, FILE* ifp, instruction* code);
void constdeclaration(int lvl, int *ptx, int *pdx, FILE* ifp, symbol* table);
void vardeclaration(int lvl, int *ptx, int *pdx, FILE* ifp, symbol* table);
void statement(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table);
void condition(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table);
void expression(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table);
void term(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table);
void factor(int lvl, int *ptx, symbol* table, FILE* ifp, instruction* code);
void produce(int op, int l, int m, instruction* code);
void enter(int k, int *ptx, int *pdx, int lvl, symbol* table);
void error(int errorCase);
int getNextToken(FILE* ifp);
int place(char *id, int *ptx, symbol* table, int lvl);
void program(FILE* ifp, symbol* table, instruction* code);

int cx, token, num, kind, lexemeListIndex=0, errors=0, difference, previousDifference=0;
char id[12];

void parser(void) {
    
    ifp = fopen("lexoutput.txt", "r");
    ofp = fopen("parserout.txt", "w");
    
    int i;
    int lvl = 0; //Lexigraphical level
    int dx = 0; //data index
    int tx = 0; //table index
    
    symbol table[MAX_SYMBOL_TABLE_SIZE] = {0};
    
    instruction code[CODE_SIZE];
    
    program(ifp, table, code);
    
    //prints instructions to file
    for (i=0; i<cx;i++) {
        fprintf(ofp, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }
    fclose(ifp);
    fclose(ofp);
}

void program(FILE* ifp, symbol* table, instruction* code) {
    
    token = getNextToken(ifp);
    block(0,0, table, ifp, code);
    
    if (token!=periodsym) {
        error(9); //Period expected.
    }
}

void block(int lvl, int tx, symbol* table, FILE* ifp, instruction* code) {
    
    
    int dx, tx0, cx0;
    dx=4;
    tx0=tx;
    table[tx].addr=cx;
    produce(7,0,0, code); // 7 is JMP for op, 0 is for L and 0 for M
    
    do {
        if (token==constsym) {
            token = getNextToken(ifp);
            do {
                constdeclaration(lvl,&tx,&dx, ifp, table);
                while(token==commasym) {
                    token = getNextToken(ifp);
                    constdeclaration(lvl,&tx,&dx, ifp, table);
                }
                if(token==semicolonsym) {
                    token = getNextToken(ifp);
                }
                else {
                    error(5); //Semicolon or comma missing.
                }
            } while (token==identsym);
        }
        if (token==varsym) {
            token = getNextToken(ifp);
            do {
                vardeclaration(lvl,&tx,&dx, ifp, table);
                while (token==commasym) {
                    token = getNextToken(ifp);
                    vardeclaration(lvl,&tx,&dx, ifp, table);
                }
                if(token==semicolonsym) {
                    token = getNextToken(ifp);
                }
                else {
                    error(5); //Semicolon or comma missing.
                }
            } while(token==identsym);
        }
        
        //IMPLEMENTING PROCEDURES
        while(token==procsym) {
            token = getNextToken(ifp);

            if(token==identsym) {
                enter(3, &tx, &dx, lvl, table); //procedure
                token = getNextToken(ifp);
            }
            else {
                error(4); //const, var, procedure must be followed by identifier.
            }
            if(token==semicolonsym) {
                token = getNextToken(ifp);
            }
            else {
                error(5); //Semicolon or comma missing.
            }
            //lev++; call lev in block after incrementing
            block(lvl+1, tx, table, ifp, code); //Go to a block one level higher
            if(token==semicolonsym) {
                token = getNextToken(ifp);
            }
            else {
                error(5); //Semicolon or comma missing.
            }
        }
    }while((token==constsym) || (token==varsym) || (token==procsym));
    
    
    code[table[tx0].addr].m=cx;
    table[tx0].addr=cx;
    //inc 0, dx is generated
    cx0=cx;
    produce(6, 0, dx, code); // 6 is INC for op, 0 is for L, and dx is M
    statement(lvl, &tx, ifp, code, table);
    produce(2, 0, 0, code); // 2 is OPR for op, 0 is RET for M inside OPR
}

void constdeclaration(int lvl, int *ptx, int *pdx, FILE* ifp, symbol* table) {
    
    if (token==identsym) {
        token = getNextToken(ifp);
        if ((token==eqsym) || (token==becomessym)) {
            if (token==becomessym) {
                error(1); //Use = instead of :=
            }
            token = getNextToken(ifp);
            if (token==numbersym) {
                enter(1,ptx,pdx,lvl, table);
                token = getNextToken(ifp);
            }
        }
    }
}

void vardeclaration(int lvl, int *ptx, int *pdx, FILE* ifp, symbol* table) {
    
    if (token==identsym) {
        enter(2,ptx,pdx,lvl, table);//var
        token = getNextToken(ifp);
    }
    else error(4); //const, var, procedure must be followed by identifier.
}

void statement(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int i, cx1, cx2;
    
    if (token==identsym){
        i=place(id,ptx, table, lvl);
        if(i==0) {
            error(11); //Undeclared identifier.
        }
        else if (table[i].kind!=2) {
            error(12); //Assignment to constant or procedure is not allowed
            i=0;
        }
        token = getNextToken(ifp);
        if (token==becomessym) {
            token = getNextToken(ifp);
        }
        else {
            error(13); //Assignment operator expected.
        }
        expression(lvl, ptx, ifp, code, table);
        if (i!=0) {
            produce(4, lvl-table[i].level, table[i].addr, code);
        }
    }
    //IMPLEMENTING CALLSYM IN STATEMENT/ IF TOKEN!="IDENTSYM"
    else if (token==callsym) {
        token = getNextToken(ifp);
        if (token!=identsym) {
            error(14);
        }
        else {
            i=place(id, ptx, table, lvl);
            if(i==0) {
                error(11); //Undeclared identifier.
            }
            else if (table[i].kind==3) {//proc
                produce(5,lvl-table[i].level, table[i].addr, code);
            }
            else {
                error(15); //Call of a constant or variable is meaningless
            }
            token = getNextToken(ifp);
        }
    }
    //IF statement implemented for hw4 requirements
    else if (token == ifsym) {
        token = getNextToken(ifp);
        condition(lvl, ptx, ifp, code, table);
        if(token == thensym) {
            token = getNextToken(ifp);
        }
        else {
            error(16);  // then expected
        }
        
        cx1 = cx;
        produce(8, 0, 0, code); // 8 is JPC for op, 0 is for L and 0 for M
        statement(lvl, ptx, ifp, code, table);
        
        // ELSE implemented for hw4 requirements
        if(token == elsesym) {
            token = getNextToken(ifp);
            
            // jumps pas if statement
            code[cx1].m = cx+1;
            cx1 = cx;
            produce(7, 0, 0, code); // 7 is JMP for op, 0 is for L and cx1 for M
            statement(lvl, ptx, ifp, code, table);
        }
        // jumps past else if there's an else statement
        code[cx1].m = cx;
    }

    else if (token == beginsym) {
        token = getNextToken(ifp);
        statement(lvl, ptx, ifp, code, table);

        while (token == semicolonsym) {
            token = getNextToken(ifp);
            statement(lvl, ptx, ifp, code, table);
        }

        if (token == endsym) {
            token = getNextToken(ifp);
        }
        else {
            error(17);  //Semicolon or } expected.
        }
    }
    
    //while <condition> do <statement>
    else if (token == whilesym) {
        cx1 =cx;
        token = getNextToken(ifp);
        condition(lvl,ptx, ifp, code, table);
        cx2 = cx;
        produce(8, 0, 0, code); // 8 is JPC for op, 0 is for L and 0 for M
        if(token == dosym) {
            token = getNextToken(ifp);
        }
        else {
            error(18);  // do expected
        }
        statement(lvl, ptx, ifp, code, table);
        produce(7, 0, cx1, code); // 7 is JMP for op, 0 is for L and cx1 for M, jump to instruction cx1
        code[cx2].m = cx;
    }
    
    //write needs to write
    else if (token == writesym) {
        token = getNextToken(ifp);
        expression(lvl, ptx, ifp, code, table);
        produce(9,0,1, code);
    }
    
    //read needs to read and STO
    else if (token == readsym) {
        token = getNextToken(ifp);
        produce(10,0,2, code);
        i=place(id,ptx, table, lvl);
        if(i==0) {
            error(11); //Undeclared identifier.
        }
        else if (table[i].kind!=2) { //var
            error(12); //Assignment to constant or procedure is not allowed
            i=0;
        }
        if (i!=0) {
            produce(4, lvl-table[i].level, table[i].addr, code);
        }
        token = getNextToken(ifp);
    }
    
}

void condition(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int relationSwitch;
    
    if (token == oddsym) {
        token = getNextToken(ifp);
        expression(lvl,ptx, ifp, code, table);
        produce(2, 0, 6, code);
        
    }
    else {
        expression(lvl,ptx, ifp, code, table);
        if ((token!=eqsym) && (token!=neqsym) && (token!=lessym) && (token!=leqsym) && (token!=gtrsym) && (token!=geqsym)) {
            error(20); //Relational operator expected.
        }
        else { //for relational operators
            relationSwitch = token;
            token = getNextToken(ifp);
            expression(lvl, ptx, ifp, code, table);
            switch(relationSwitch) {
                case 9:
                    produce(2,0,8, code);
                    break;
                case 10:
                    produce(2,0,9, code);
                    break;
                case 11:
                    produce(2,0,10, code);
                    break;
                case 12:
                    produce(2,0,11, code);
                    break;
                case 13:
                    produce(2,0,12, code);
                    break;
                case 14:
                    produce(2,0,13, code);
                    break;
            }
        }
    }
}

void expression(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int addop;
    if (token == plussym || token == minussym) {
        addop = token;
        token = getNextToken(ifp);
        term(lvl, ptx, ifp, code, table);
        if(addop == minussym) {
            produce(2, 0, 1, code); // 2 is OPR for op, 1 is NEG for M inside OPR
        }
    }
    else {
        term (lvl, ptx, ifp, code, table);
    }
    while (token == plussym || token == minussym) {
        addop = token;
        token = getNextToken(ifp);
        term(lvl, ptx, ifp, code, table);
        if (addop == plussym) {
            produce(2, 0, 2, code); // 2 is OPR for op, 2 is ADD for M inside OPR
        }
        else {
            produce(2, 0, 3, code); // 2 is OPR for op, 3 is SUB for M inside OPR
        }
    }
}

void term(int lvl, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int mulop;
    factor(lvl, ptx, table, ifp, code);
    while(token == multsym || token == slashsym) {
        mulop = token;
        token = getNextToken(ifp);
        factor(lvl, ptx, table, ifp, code);
        if(mulop == multsym) {
            produce(2, 0, 4, code); // 2 is OPR for op, 4 is MUL for M inside OPR
        }
        else {
            produce(2, 0, 5, code); // 2 is OPR for op, 5 is DIV for M inside OPR
        }
    }
}

void factor(int lvl, int *ptx, symbol* table, FILE* ifp, instruction* code) {
    
    int i, level, adr, val;
    
    while ((token==identsym)||(token==numbersym)||(token==lparentsym)){
        if (token==identsym) {
            i=place(id,ptx, table, lvl);
            if (i==0) {
                error(11); // undeclared identifier
            }
            else {
                kind=table[i].kind;
                level=table[i].level;
                adr=table[i].addr;
                val=table[i].val;
                if (kind==1) {//const
                    produce(1,0,val, code); // 1 is LIT for op, val is for M inside LIT
                }
                else if (kind==2) {//var
                    produce(3,lvl-level,adr, code); // 3 is LOD for op, lvl-level is L inside LOD, adr is for M inside LOD
                }
                else {
                    error(21); // Expression must not contain a procedure identifier
                }
            }
            token = getNextToken(ifp);
        }
        else if(token==numbersym) {
            if (num>2047) { //maximum address
                error(25);
                num=0;
            }
            produce(1,0,num, code); // 1 is LIT for op, num is for M inside LIT
            token = getNextToken(ifp);
        }
        else if(token==lparentsym) {
            token = getNextToken(ifp);
            expression(lvl,ptx, ifp, code, table);
            if (token==rparentsym) {
                token = getNextToken(ifp);
            }
            else {
                error(22); // Right parenthesis missing.
            }
        }
    }
}

void produce(int op, int l, int m, instruction* code) {
    
    if (cx > CODE_SIZE)
        printf("Program too long\n");
    else {
        code[cx].op = op;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
}

//This enters a symbol into the table
void enter(int k, int *ptx, int *pdx, int lvl, symbol* table) {
    
    char *id1;
    int ii, len;
    (*ptx)++; //table index tx is increased by 1
    id1=id; //last identifier read
    len=strlen(id);
    for (ii=0;ii<=len;ii++) {
        table[*ptx].name[ii]=*id1; //id is recorded in .name
        id1++;
    }
    //updates kind
    table[*ptx].kind=k;
    //for constants: updates value
    if (k==1) {//const
        table[*ptx].val=num;
    }
    else if (k==2) {//var
        table[*ptx].level=lvl;
        table[*ptx].addr=*pdx;
        (*pdx)++;
    }
}


int getNextToken(FILE* ifp) {
    
    token = lexList[lexemeListIndex].token;
    
    if(token==2){
        strcpy(id, lexList[lexemeListIndex].name);
    }
    else if(token==3){
        num = lexList[lexemeListIndex].numberValue;
    }
    
    lexemeListIndex++;
    return token;
}

int place(char *id, int *ptx, symbol* table, int lvl) {
    
    int s;
    s=*ptx;
    
    int currentS;
    int differenceCount = 0;
    
    while(s!=0) {
        if (strcmp(table[s].name,id) == 0) {
            if(table[s].level<=lvl) {
                
                if (differenceCount!=0) {
                    previousDifference = difference;
                }
                
                difference = lvl-table[s].level;
                
                if(differenceCount==0) {
                    currentS=s;
                }
                
                if (difference<previousDifference) {
                    currentS=s;
                }
                differenceCount++;
            }
        }
        s--;
    }
    return currentS;
}

//prints out errors
void error(int errorCase) {
    
    errors++;
    
    switch (errorCase) {
        case 1:
            printf("Error 1: ");
            printf("Use = instead of :=.\n");
            break;
        case 2:
            printf("Error 2: ");
            printf("= must be followed by a number.\n");
            break;
        case 3:
            printf("Error 3: ");
            printf("Identifier must be followed by =.\n");
            break;
        case 4:
            printf("Error 4: ");
            printf("const, var, procedure must be followed by identifier.\n");
            break;
        case 5:
            printf("Error 5: ");
            printf("Semicolon or comma missing.\n");
            break;
        case 6:
            printf("Error 6: ");
            printf("Incorrect symbol after procedure declaration.\n");
            break;
        case 7:
            printf("Error 7: ");
            printf("Statement expected\n");
            break;
        case 8:
            printf("Error 8: ");
            printf("Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Error 9: ");
            printf("Period expected.\n");
            break;
        case 10:
            printf("Error 10: ");
            printf("Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Error 11: ");
            printf("Undeclared identifier.\n");
            break;
        case 12:
            printf("Error 12: ");
            printf("Assignment to constant or procedure is not allowed.\n");
            break;
        case 13:
            printf("Error 13: ");
            printf("Assignment operator expected.\n");
            break;
        case 14:
            printf("Error 14: ");
            printf("call must be followed by an identifier\n");
            break;
        case 15:
            printf("Error 15: ");
            printf("Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("Error 16: ");
            printf("then expected\n");
            break;
        case 17:
            printf("Error 17: ");
            printf("Semicolon or } expected\n");
            break;
        case 18:
            printf("Error 18: ");
            printf("do expected\n");
            break;
        case 19:
            printf("Error 19: ");
            printf("Incorrect symbol following statement.\n");
            break;
        case 20:
            printf("Error 20: ");
            printf("Relational operator expected.\n");
            break;
        case 21:
            printf("Error 21: ");
            printf("Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("Error 22: ");
            printf("Right parenthesis missing.\n");
            break;
        case 23:
            printf("Error 23: ");
            printf("The preceding factor cannot begin with this symbol.\n");
            break;
        case 24:
            printf("Error 24: ");
            printf("An expression cannot begin with this symbol.\n");
            break;
        case 25:
            printf("Error 25: ");
            printf("This number is too large.\n");
            break;

        default:
            break;
    }
    //commented out the exit(1) in order for parser to keep running even after encountering errors
    //stops program when error occurs
   // exit(1);
}
