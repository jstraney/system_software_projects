#ifndef STDH 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STDH 
#endif

#ifndef ANALYZER
#include "analyzer.h"
#define ANALYZER
#endif

// generator statuses
#define GENERATOR_STATUS_OK      0
#define GENERATOR_STATUS_ERR     1 

// max symbol table size. adjust as needed
#define MAX_SYMBOL_TABLE_SIZE 2000

// hash function used for symbol table
int hash(char *str);

typedef struct {
  int  type;
  char name [10];
  int  val;
  int  level;
  int  addr;

} Symbol;


int generator_entry(Token *head);

int gen(int op, int l, int m, int reg);

int Symbol_insert(Symbol symbol);
Symbol find(char *ident);

int symboltype ();
int symbollev ();
int symboladdress();

int write_assembly_to_file(char *machine_code);
