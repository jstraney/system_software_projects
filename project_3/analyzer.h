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

#ifndef BOOL
#define FALSE 0 
#define TRUE  1
#define BOOL
#endif

// max symbol table size. adjust as needed
#define MAX_SYMBOL_TABLE_SIZE 2000

// token types. 
typedef enum { 
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} TokenType;

//// File IO ////
// open file provided by argv
FILE *open_file(char * filename);

// returns true if lexeme is reserved
int is_reserved_word (char *lexeme);

// higher level grammar functions. Do not return a token type, but
// instead return a status. That's because these parts of the languages
// grammar are not being stored as tokens, but we still need to validate
// that the program file is in the language grammar.
int is_program    (FILE *file);
int is_block      (char *file_text, int *offset);
int is_constdec   (char *file_text, int *offset);
int is_vardec     (char *file_text, int *offset);
int is_procdec    (char *file_text, int *offset);
int is_statement  (char *file_text, int *offset);
int is_expression (char *file_text, int *offset);
int is_condition  (char *file_text, int *offset);
int is_relsym     (char *file_text, int *offset);
int is_term       (char *file_text, int *offset);
int is_factor     (char *file_text, int *offset);

// subprocedures of is_statement, checks for different kinds of statements
int is_callstatement  (char *file_text, int *offset);
int is_beginstatement (char *file_text, int *offset);
int is_ifstatement    (char *file_text, int *offset);
int is_whilestatement (char *file_text, int *offset);
int is_readstatement  (char *file_text, int *offset);
int is_writestatement (char *file_text, int *offset);

// token parsing functions. We've implemented our top down parse using
// a function call stack to naturally take the form of a parse tree.
// the functions below will return the enumerated type of symbol found
// and have a side effect of loading tokens into a global buffer.
//
// the functions also may return a -1 for INVALID_SYMBOL, but may
// get 'caught' by other branches of the tree. if they all return
// INVALID_SYMBOL, then the program is invalid
int is_nulsym       (char *file_text, int *offset);
int is_identsym     (char *file_text, int *offset);
int is_numbersym    (char *file_text, int *offset);
int is_plussym      (char *file_text, int *offset);
int is_minussym     (char *file_text, int *offset);
int is_multsym      (char *file_text, int *offset);
int is_slashsym     (char *file_text, int *offset);
int is_oddsym       (char *file_text, int *offset);
int is_eqsym        (char *file_text, int *offset);
int is_neqsym       (char *file_text, int *offset);
int is_lessym       (char *file_text, int *offset);
int is_leqsym       (char *file_text, int *offset);
int is_gtrsym       (char *file_text, int *offset);
int is_geqsym       (char *file_text, int *offset);
int is_lparentsym   (char *file_text, int *offset);
int is_rparentsym   (char *file_text, int *offset);
int is_commasym     (char *file_text, int *offset);
int is_semicolonsym (char *file_text, int *offset);
int is_periodsym    (char *file_text, int *offset);
int is_becomessym   (char *file_text, int *offset);
int is_beginsym     (char *file_text, int *offset);
int is_endsym       (char *file_text, int *offset);
int is_ifsym        (char *file_text, int *offset);
int is_thensym      (char *file_text, int *offset);
int is_whilesym     (char *file_text, int *offset);
int is_dosym        (char *file_text, int *offset);
int is_callsym      (char *file_text, int *offset);
int is_constsym     (char *file_text, int *offset);
int is_varsym       (char *file_text, int *offset);
int is_procsym      (char *file_text, int *offset);
int is_writesym     (char *file_text, int *offset);
int is_readsym      (char *file_text, int *offset);
int is_elsesym      (char *file_text, int *offset);

// helper function for various terminals. returns the 'if_valid' token type if
// *(file_text + offset + [0..n-1]) = symbol[0] ..symbol[n-1]. otherwise, returns
// a INVALID_SYMBOL constant, which may be changed in the parse tree. 
int is_terminal (char *file_text, int *offset, char symbol[], int if_valid);

// trim functions. called throughout the parse tree. these do not store
// tokens in any way, but rather continue to increase the
// offset while we are encountering white space or are in
// a comment. because they do not parse tokens, there is no
// return value

// trim file input of whitespace and comments
int  trim         (char *file_text, int *offset);

// helper functions which check ascii values of individual characters
// returns TRUE or FALSE because letters, digits etc. are not stored
// as symbols. Additionally, these will not modify the global offset
// as this should happen higher in the call stack.
int is_letter     (char c);
int is_digit      (char c);
int is_whitespace (char c);

// function appends the next character to the token buffer
void append_token_buffer(char next);

char *get_token_buffer_value();

// reset the token buffer to '\0'
void empty_token_buffer();

// renamed to token since assignment 2: 
typedef struct Token {
  char   *lexeme;
  int    token_type;
  struct Token *next;
} Token;

//// Structures ////
// take the buffer we've been appending to, as well as the token
// returned by the function call stack. Then insert into the symbol table
Token *Token_new(char *lexeme, int token_type);

int insert_token(char *lexeme, int token_type);

void print_token(Token token);

void print_token_table();

void print_token_list();

int analyzer_event_loop(FILE *file);

int analyzer_entry(char *file_name);

int hash(char *str);

typedef enum {
  var_ident = 1, proc_ident, const_ident
} SymbolType;

typedef struct {
  SymbolType   type;
  char         *name;
  int          val;
  int          level;
  int          addr;
} Symbol;

Token * get_token_table();

Symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

int Symbol_insert(Symbol symbol);

int Symbol_remove(Symbol symbol);

Symbol find(char *ident);

int symboltype ();

int symbollev ();

int symboladdress();

void print_symbol (Symbol symbol);

//// Event Loop ////
// provides a list of options for user.
/*
int interactive_loop(FILE *file);

// runs the parser until finished

int get_user_command();
void print_code  (FILE *file);

void print_status(int status);

*/
/*
void print_prompt();
*/

