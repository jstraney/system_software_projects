////// MAIN_H //////

//// Includes ////
#ifndef STDIO_H
#include <stdio.h>
#define STDIO_H
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#define STDLIB_H 
#endif

#ifndef STR_H
#include <string.h>
#define STR_H
#endif

// token types
typedef enum { 
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

//// File IO ////
// open file provided by argv
FILE *open_file(char * filename);

// open the file using file pointer
int read_file(FILE *file);

// simply grabs the character following the current one 
int lookahead(FILE *file);
