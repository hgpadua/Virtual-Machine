//
//  main.c
//  data header for easier reference
//
//  Created by Huey Padua on 10/9/17.
//  Copyright © 2017 Huey Padua. All rights reserved.
//
#define MAX_SYMBOL_TABLE_SIZE 1000
#define CODE_SIZE 1000
#define MAX_LEXI_LEVELS 3
#define MAX_STACK_HEIGHT 2000

extern int lexoutput;
extern int codegenoutput;
extern int vmoutput;
extern int compile;

extern int errors;

typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;

typedef struct {
    token_type token;
    int numberValue;
    char name[12];
}tokenStruct;

extern tokenStruct lexList[5000];
extern int lexListIndex;

typedef struct {
    int op; //Opcode
    int l;  //L
    int m;  //M
} instruction;

extern instruction code[CODE_SIZE];


//function prototypes, for linking all the programs
void driver(void);
void lexical(void);
void parser(void);
void pmachine(void);
