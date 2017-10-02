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

//// Proposed Lookup Path : In Pseudo-Code ////
// 
// let symbol_value = 'a'
//
// found_symbol = lookup_symbol(char symbol_value, SymbolTable symbol_table)
//
// return found_symbol.next = {'b', 'c', 'd'} 
//
// let next_symbol_value = 'c'
//
// if next_symbol_value in found_symbol.next:
//   lookup_symbol(next_symbol_value) etc. repeat steps above until we reach \0
//
// else throw 'symbol not found.'

//// Structure Declarations ////

//// Symbol Struct ////
typedef struct Symbol {

  // each symbol has a pointer to its symbol table
  SymbolTable *table;

  // each symbol may have many symbols which follow it
  Symbol *next;

  // but each symbol has exactly one character
  char value;

} Symbol;

// creates a symbol struct. takes a list of symbols
// that it points to and a character which is its value
Symbol Symbol_new(Symbol *next, char value);

//// Symbol Table Struct ////
typedef struct SymbolTable {

  // points to first symbol in table. initalized by SymbolTable_new.
  Symbol *symbols;

  // the number of symbols
  int size;

} SymbolTable;

// creates a symbol table. size doesn't describe how many characters there are
// but rather the range of the enumerated values (e.g. an alphabet of all ascii
// characters would be of size 256.) This is so we may key the symbols by
// their character value for faster lookup time ('a' would be found at
// *(this.symbols + 97), 'b' is found at *(this.symbols + 98).
SymbolTable SymbolTable_new (int size);

//// Symbol/SymbolTable functions ////
// find symbol in table and return it
Symbol lookup_symbol(char symbol_value, SymbolTable table);

//// File IO ////
// open file provided by argv
FILE *open_file(char * filename);

// open the file using file pointer
int read_file(FILE *file);

