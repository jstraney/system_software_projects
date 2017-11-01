#ifndef STDH 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define STDH 
#endif

#ifndef VM
#include "vm.h"
#define VM 
#endif

#ifndef GENERATOR
#include "generator.h"
#define GENERATOR
#endif

Symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

// lexical level tracked by the generator. used when generating assembly with
// parse tree
int gen_lex_level = 0;

// generators tracking of pc. should increase linearly
int gen_pc = 0;

// array of string codes to be written to a file
char *gen_code[VM_MAX_CODE_LENGTH];

// hash function to key the unique symbol in the table
// we used this academic page as insperation to help randomize the result
// http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)HashTables.html?highlight=%28CategoryAlgorithmNotes%29
int hash(char *str) {

  // index of string being hashed
  int i = 0;

  // start at beginning of string
  int c = str[i];

  // starting with an odd, prime constant
  int result = 0;

  do {

    // multiply the result by a prime and add c
    result = ((result * 31) + c)  % MAX_SYMBOL_TABLE_SIZE;

    // go to next character
    c = str[++i];

    // exit on null
  } while (c != '\0');

  // return the hash
  return result; 

}

// lexical level in the generator
int generator_lexi_level = 0;

Instruction code[VM_MAX_CODE_LENGTH];

int write_assembly_to_file (char *code) {

  FILE *fp;

  fp = fopen("assembly.code", "w");

  if (fp == NULL) {

    printf("something went wrong writing assembly code");

  }

  // fwrite or some equivalent...
  // fwrite(code);

  fclose(fp);

  return 0;

}

void gen_inc_lex_lev() {

  gen_lex_level++;

}

void gen_dec_lex_lev() {

  gen_lex_level--;

}

int Symbol_insert(Symbol symbol) {

  // derive hash from the name
  int key = hash(symbol.name);

  // put into the symbol
  symbol_table[key] = symbol;


  return GENERATOR_STATUS_OK;

}

Symbol find(char *ident) {

  int key = hash(ident);

  return symbol_table[key];

}

// helper function to find length of number in digits
int num_length (int num) {

  int length = 1;

  while ((num /= 10) > 0) {

    length++;

  }

  return length;

}

int gen(int op, int l, int m, int reg) {

  // length of numbers plus 3 spaces, newline and \0
  int instruction_len = num_length(op) + num_length(l) + num_length(m) + 5;

  // string should be length of instruction
  char instruction_str[instruction_len];

  // print string to instruction character buffer
  sprintf(instruction_str, "%d %d %d\n", op, l, m);   

  // insert strin instruction in code array
  gen_code[gen_pc++] = instruction_str;

  return 0;

}


