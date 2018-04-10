Huey Padua
COP3402
Hw4
README

PL/0 Compiler

In this assignment, the functionality in homework 3 is extended to include the additional grammatical constructs, such as procedure-declaration statement and else statement. The tiny PL/0 consists of three programs, which are pmachine.c, lexical.c, and parser.c, that are all linked to one main program, named main.c, in order to create a compiler driver to combine all of the compiler parts into one single program. A header file, named data.h, is provided in order to access structures and external variables easier. The compiler driver is a program that manages the parts of the compiler. In order to run the tiny PL/0 properly, an input file must be read from lexical.c, which is named “lexinput2.txt”, contained in the zipped folder. The fopen line within lexical.c includes “lexinput2.txt”, added with path directory in order to search the files faster. Then the output from lexical should be read in the parser.c, which will indicate if the program is syntactically correct. Afterwards, the text file should contain the generated code from the intermediate code generator and the stack output from the virtual machine running the code, which is the pmachine. In order to run the compiler driver, it’s best to use the command line, with three types of commands, for printing types of output to the console, such as:

gcc -o compile main.c lexical.c parser.c pmachine.c
./compile -l-a-v
./compile -v
./compile


