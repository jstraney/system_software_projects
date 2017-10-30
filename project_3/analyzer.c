/*
L)       E)eeeeee X)    xx I)iiii   C)ccc    A)aa   L)       
L)       E)        X)  xx    I)    C)   cc  A)  aa  L)       
L)       E)eeeee    X)xx     I)   C)       A)    aa L)       
L)       E)         X)xx     I)   C)       A)aaaaaa L)       
L)       E)        X)  xx    I)    C)   cc A)    aa L)       
L)llllll E)eeeeee X)    xx I)iiii   C)ccc  A)    aa L)llllll 
                                                             
                                                             
  A)aa   N)n   nn   A)aa   L)       Y)    yy Z)zzzzzz E)eeeeee R)rrrrr  
 A)  aa  N)nn  nn  A)  aa  L)        Y)  yy        Z) E)       R)    rr 
A)    aa N) nn nn A)    aa L)         Y)yy       Z)   E)eeeee  R)  rrr  
A)aaaaaa N)  nnnn A)aaaaaa L)          Y)       Z)    E)       R) rr    
A)    aa N)   nnn A)    aa L)          Y)     Z)      E)       R)   rr  
A)    aa N)    nn A)    aa L)llllll    Y)    Z)zzzzzz E)eeeeee R)    rr 

*/
// written by Jeffrey Straney and Frank Volk
// 10-2-2017
// COP3402 HW 2
//
////////////////////////////////////////////////////////////////////////////////

#ifndef LEX_ANALYZER 
#include "analyzer.h"
#define LEX_ANALYZER 
#endif

#define FALSE            0 
#define TRUE             1

#define OPTION_QUIT      0
#define OPTION_RUN       1
#define OPTION_PRINT     2 

#define MODE_NORMAL      0
#define MODE_INTERACTIVE 1

// we will make our errors go in the opposite direction so they
// are not confused with symbol enumerations
#define NO_ERR              0
#define ERR_IDENT_START    -1 
#define ERR_NUM_TOO_LONG   -2 
#define ERR_IDENT_TOO_LONG -3 
#define ERR_INVALID_TOKEN  -4 
#define STATUS_QUIT        -5 
#define STATUS_FINISHED    -6 

char *statuses[] = {
  "everything's cool",
  "identifier must start with a letter",
  "number too long (max length of 5)",
  "identifier is too long (max length of 11)",
  "unrecognized token"
};

#define MAX_TOKEN_SIZE 11
#define MAX_IDENT_SIZE 11
#define MAX_DIGIT_SIZE  5

#define COMMAND_INVALID        -1
#define COMMAND_QUIT            0 
#define COMMAND_HELP            1 
#define COMMAND_STEP            2 
#define COMMAND_RUN             3 
#define COMMAND_PRINT_STATUS    4
#define COMMAND_PRINT_CODE      5 
#define COMMAND_PRINT_LEX_TABLE 6
#define COMMAND_PRINT_LEX_LIST  7
#define COMMAND_DO_ALL          8 


////////////////////////////////////////////////////////////////////////////////
char *token_buffer;

int buffer_index = 0;

// first symbol in the program
Symbol *head;

Symbol *Symbol_new(char *lexeme, int token_type) {

  Symbol *this;

  this = malloc(sizeof(Symbol));

  this->lexeme     = lexeme;

  this->token_type = token_type;

  this->next = NULL;

  return this;

}

// put symbol into the symbol table
int insert_symbol (char *lexeme, int token_type) {

  Symbol *current = head;

  // traverse symbol table
  while (current->next != NULL) {

    current = current->next;

  }

  Symbol *symbol = Symbol_new(lexeme, token_type);

  // set the vacant 'next' to symbol
  current->next = symbol;

  return NO_ERR;

}

void print_symbol (Symbol symbol) {

  // our implementation of the symbol table has the null symbol pointing to the
  // start of the table (like a linked list. here we will skip it.
  if (symbol.token_type == nulsym)
    return;

  printf("%-11s\t\t%2d\n", symbol.lexeme, symbol.token_type);

}

// starts at first symbol and goes to end of program.
void print_symbol_table () {

  Symbol *symbol;

  symbol = head;

  printf("Lexeme Table:\n");
  printf("lexeme\t\ttoken type\n");

  while (symbol->next != NULL ) {

    symbol = symbol->next;

    print_symbol(*symbol);

  }

  printf("\n");

}

void print_symbol_list () {

  Symbol *symbol;

  symbol = head;

  printf("Lexeme List:\n");

  while (symbol->next != NULL ) {

    symbol = symbol->next;

    if (symbol->token_type == 2) {

      printf("%d %s ", symbol->token_type, symbol->lexeme);

    }
    else {

      printf("%d ", symbol->token_type);

    }

  }

  printf("\n");

}

void print_code (FILE *file) {

  // create a new, local pointer (so we don't futz with
  // the pointer position.
  FILE *fp = file;

  fseek(fp, 0, SEEK_SET);

  char c;

  printf("Code File:\n");

  while ((c = getc(fp)) != EOF) {

    printf("%c", c);

  }

  printf("\n");

}

void append_token_buffer(char next) {

  // insert the next character into our buffer
  *(token_buffer + buffer_index) = next;  

  buffer_index++;

}

char *get_token_buffer_value() {

  // size of complete string in buffer 
  int size;

  for (int i = 0; i < MAX_TOKEN_SIZE; i++) {

    size = i;

    if (token_buffer[i] == '\0') {

      break;

    }

  }

  char * lexeme = (char *)calloc(size, sizeof(char));

  *(lexeme + size - 1) = '\0';

  for (int i = 0; i < size; i++) {

    *(lexeme + i) = token_buffer[i];

  }

  empty_token_buffer();

  return lexeme;

}

void empty_token_buffer() {

  // don't forget the buffer is max size + 1 for \0 character 
  memset(token_buffer, '\0', MAX_TOKEN_SIZE + 1);

  buffer_index = 0;

}

int main (int argc, char * argv[]) {

  // status starts off ok
  int status = NO_ERR;

  // don't forget to allocate an extra space for '\0'
  token_buffer = (char *)calloc(MAX_TOKEN_SIZE + 1, sizeof (char));

  // create an empty start symbol
  head = Symbol_new("", nulsym);

  // interactive or normal mode
  int mode;

  // name of file from argv
  char *filename;

  // running interactive mode. allow user commands
  if (argc == 3 && strcmp(argv[1], "-i") == 0) {

    mode = MODE_INTERACTIVE;
    // file containing instructions
    filename = argv[2];

  }
  else if (argc == 2) {

    mode = MODE_NORMAL;

    // file containing instructions
    filename = argv[1];

  }
  else {

    printf("the lexical parser requires a filename, or the -i flag followed by a file name.\n");

    status = ERR_INVALID_TOKEN;

    return status;

  }

  FILE *file = open_file(filename);

  if (file == NULL) {

    printf("cannot find the file %s\n", filename);

    return 1;

  }

  if (mode == MODE_INTERACTIVE) {

    status = interactive_loop(file);

  }
  else if (mode == MODE_NORMAL) {

    status = event_loop(file);

  }


  fclose(file);

  return status;

}

FILE *open_file(char *file_name) {

  FILE *file;

  file = fopen(file_name, "r");

  return file;

}


// print table on program start in interactive mode
void print_prompt () {

  // print out the prompt
  printf("==================================================\n");
  printf("=           Lexical Parser Interactive Mode      =\n");
  printf("==================================================\n");
  printf("= q - quit the parser                   ==========\n");
  printf("= h - call for help                     ==========\n");
  printf("= r - run program from start to finsish ==========\n");
  printf("= i - print status code                 ==========\n");
  printf("= c - print program file                ==========\n");
  printf("= t - print lexeme table                ==========\n");
  printf("= l - print lexeme list                 ==========\n");
  printf("= a - just give me the output, okay?    ==========\n");
  printf("==================================================\n\n");

}


// get command
int get_user_command () {

  // an array of commands, has as many indices 
  // as there are ascii characters.
  int commands[128];

  // quit the VM
  commands['q'] = COMMAND_QUIT;

  // print help for using these commands
  commands['h'] = COMMAND_HELP;

  // run all instruction from start to finish
  commands['r'] = COMMAND_RUN;

  // prints status code when stepping through 
  commands['i'] = COMMAND_PRINT_STATUS;

  // prints status code when stepping through 
  commands['c'] = COMMAND_PRINT_CODE;

  // prints status code when stepping through 
  commands['t'] = COMMAND_PRINT_LEX_TABLE;

  // prints status code when stepping through 
  commands['l'] = COMMAND_PRINT_LEX_LIST;

  // runs the program and prints all the output 
  commands['a'] = COMMAND_DO_ALL;

  // character entered in by user
  char user_entry;

  scanf(" %c", &user_entry);
  printf("\n");

  // check if valid using a whitelist approach
  int valid =
    user_entry == 'q' || user_entry == 'h' || user_entry == 'r' || user_entry == 'i' ||
    user_entry == 'c' || user_entry == 't' || user_entry == 'l' || user_entry == 'a'; 

  // if the character is not valid, help the user out.
  // return -1;
  if (!valid) {

    // return -1, should restart the interactive loop
    return COMMAND_INVALID;

  }

  // take the character entered by the user and use it as a key
  // in our commands. 'h' for example has an ascii value of 104,
  // so the code below looks at index 104 in the 'commands' array,
  // which should contain the COMMAND_HELP constant
  int user_command = commands[user_entry];

  return user_command;

}

// run the vm through interactive mode
int interactive_loop (FILE *file) {

  int status = NO_ERR;

  // print the prompt once on start up. print again
  // if the COMMAND_HELP is issued 
  print_prompt();

  // integer flag COMMAND_<command>
  int user_command;

  while (status == NO_ERR) {

    printf(">> ");

    user_command = get_user_command();

    if (user_command == COMMAND_INVALID) {

      printf("Please enter a valid command. use 'h' for help");
      // return to top of the loop
      continue;
      
    }

    else if (user_command == COMMAND_QUIT) {

      status = STATUS_QUIT;

      printf("Bye!\n");

      break;

    }

    else if (user_command == COMMAND_HELP) {

      // simply print the prompt again
      print_prompt();

      continue;

    }

    else if (user_command == COMMAND_RUN) {

      // call event loop as a subprocedure.
      status = event_loop(file);

      printf("finished analyzing\n");

    }
    
    else if (user_command == COMMAND_PRINT_CODE) {

      print_code(file);

    }

    // give a simple status (ok, quit, error, finished)
    else if (user_command == COMMAND_PRINT_STATUS) {

      print_status(status);

    }

    else if (user_command == COMMAND_PRINT_LEX_TABLE) {

      print_symbol_table();

    }

    else if (user_command == COMMAND_PRINT_LEX_LIST) {

      print_symbol_list();

    }

    else if (user_command == COMMAND_DO_ALL) {

      print_code(file);
      // call event loop as a subprocedure.
      status = event_loop(file);

      print_symbol_table();

      print_symbol_list();

    }

  }
  
  return status;

}

int event_loop (FILE *file) {

  int status;

  status = is_program(file);

  return status;

}

// checks if reserved word
int is_reserved_word (char *lexeme) {

  return
    strcmp("begin",     lexeme) == 0 ||
    strcmp("call" ,     lexeme) == 0 ||
    strcmp("const",     lexeme) == 0 ||
    strcmp("do",        lexeme) == 0 ||
    strcmp("else",      lexeme) == 0 ||
    strcmp("end",       lexeme) == 0 ||
    strcmp("if",        lexeme) == 0 ||
    strcmp("odd",       lexeme) == 0 ||
    strcmp("procedure", lexeme) == 0 ||
    strcmp("read",      lexeme) == 0 ||
    strcmp("then",      lexeme) == 0 ||
    strcmp("var",       lexeme) == 0 ||
    strcmp("while",     lexeme) == 0 ||
    strcmp("write",     lexeme) == 0 ;

}

// determines if program is valid. as a side effect, loads tokens
// and their lexemes into the symbol table of the program.
// args: file pointer
// returns: integer flag to indicate status
int is_program (FILE *fp) {

  // our lexical parser loads the file into a character buffer,
  // to easily manipulate pointer location using local function
  // variables.

  // move pointer to end of file
  fseek (fp, 0, SEEK_END);

  // tell us how large the file is
  int size = ftell (fp);

  // move pointer to start of file
  fseek (fp, 0, SEEK_SET);

  // create a continuous section of memory of size + 1 for
  // null character at the end of the memory block
  char *file_text = (char *) calloc(size + 1, sizeof(char));

  // read entire file into the character buffer
  fread(file_text, size, 1, fp);

  // keep the file open for printing purposes

  // 'global' offset from the character pointer
  int  *offset = (int *)malloc(sizeof(int));

  *offset = 0;

  int status = NO_ERR;

  while(status != STATUS_FINISHED) { 
    
    // must have a block
    status = is_block(file_text, offset);

    int halt;

    // and must terminate with a period
    halt = (is_periodsym(file_text, offset) == periodsym);

    if (halt) {

      break;

    }
     
  }

  free(file_text);
  free(offset);

  return status;

}

int is_block (char *file_text, int *offset) {

  int status;

  (status = is_constdec  (file_text, offset) != ERR_INVALID_TOKEN) ||
  (status = is_vardec    (file_text, offset) != ERR_INVALID_TOKEN) ||
  (status = is_procdec   (file_text, offset) != ERR_INVALID_TOKEN) ||
  (status = is_statement (file_text, offset) != ERR_INVALID_TOKEN);

  return status;

}

int is_constdec (char *file_text, int *offset) {

  TokenType token;

  token = is_constsym(file_text, offset);

  if (token != constsym) {

    return ERR_INVALID_TOKEN;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  trim(file_text, offset);

  if (token != identsym) {

    return ERR_INVALID_TOKEN;

  }

  token = is_eqsym (file_text, offset);
  
  trim(file_text, offset);

  if (token != eqsym) {

    return ERR_INVALID_TOKEN;
    
  }

  token = is_numbersym(file_text, offset);

  trim(file_text, offset);

  if (token != numbersym) {

    return ERR_INVALID_TOKEN;

  }

  // optionally check for commas
  token = is_commasym(file_text, offset);

  trim(file_text, offset);

  // loop while we have a comma followed by identifier
  while (token == commasym) {

    token = is_identsym(file_text, offset);

    trim(file_text, offset);

    if (token != identsym) {

      return ERR_INVALID_TOKEN;

    }

    token = is_eqsym (file_text, offset);
    
    trim(file_text, offset);

    if (token != eqsym) {

      return ERR_INVALID_TOKEN;

    }

    token = is_numbersym(file_text, offset);

    trim(file_text, offset);

    if (token != numbersym) {

      return ERR_INVALID_TOKEN;

    }


    token = is_commasym(file_text, offset);

    trim(file_text, offset);

  }

  token = is_semicolonsym(file_text, offset);

  if (token != semicolonsym) {

    return ERR_INVALID_TOKEN;

  }

  return NO_ERR;

}

int is_vardec (char *file_text, int *offset) {

  TokenType token;

  token = is_varsym(file_text, offset);

  if (token != varsym) {

    return ERR_INVALID_TOKEN; 

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != identsym) {

    return ERR_INVALID_TOKEN; 

  }

  trim(file_text, offset);

  int halt;

  // check for commas followed by identifiers until we
  // reach a semicolon
  do {

    // check if next token is comma
    token = is_commasym(file_text, offset); 
    
    if (token == commasym) { 

      trim (file_text, offset);
    
      // now see if next token is an identifier
      token = is_identsym(file_text, offset); 
      
      // comma must be followed by an identifier
      if (token != identsym) {

        return ERR_INVALID_TOKEN;

      }

      trim(file_text, offset);

    }
    // if there is no comma, we must find a semicolon
    else {

      // check if next symbol is a semicolon
      token = is_semicolonsym(file_text, offset);

      if (token != semicolonsym) {

        return ERR_INVALID_TOKEN;

      }

    }

    if (token == nulsym) {

      return ERR_INVALID_TOKEN;

    }

    // stop this if we reach a semicolon or something horrible
    halt = token == semicolonsym;

  } while (!halt);

  // trim just in case
  trim(file_text, offset);

  return NO_ERR;

}


int is_identsym (char *file_text, int *offset) {

  int local_offset = *offset;

  // check first character to see if it is alpha 
  char next;

  next = *(file_text + local_offset);

  int err = NO_ERR;

  if (!is_letter(next)) {

    printf("identifier at position %d must start with a letter\n", local_offset);

    return ERR_IDENT_START;

  }

  // since first symbol is a letter, this is a valid
  append_token_buffer(next); 
  
  // identifier (e.g. 'x'). the following code just checks for
  // additial letters and digits and moves the offset accordingly
  local_offset++;

  next = *(file_text + local_offset);

  int ident_len = local_offset - *offset;

  // if the char is letter or digit, increase local offset
  while (is_letter(next) || is_digit(next)) {

    ident_len = (local_offset - *offset);
     
    if (ident_len < MAX_TOKEN_SIZE) {

      append_token_buffer(next);

    }

    local_offset++;

    // fetch current char 
    next = *(file_text + local_offset);

  } 

  char *lexeme = get_token_buffer_value();

  if (ident_len > MAX_TOKEN_SIZE) { 

    printf("token of length %d exceeds max length of 11 at position %d\n", ident_len, *offset);
    printf("storing truncated value of %s instead\n\n", lexeme);

  }

  // identifier cannot be reserved.
  if (is_reserved_word(lexeme)) {

    return FALSE;

  }

  insert_symbol(lexeme, identsym);

  // we stopped. reset the global offset
  *offset = local_offset;

  return identsym;

}

int is_procdec (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  next = (file_text + local_offset);

  int status = NO_ERR;

  TokenType token;

  int halt = 0;

  //loops until it doesn't see "procedure".
  do {

    // checks for "procedure"
    token = is_procsym(file_text, offset);

    if (token != procsym) {

      break;

    }

    trim(file_text, offset);

    //expect identifier here
    token = is_identsym(file_text, offset);

    if (token != identsym) {

      break;

    }

    trim(file_text, offset);

    //expect a semicolon here
    token = is_semicolonsym(file_text, offset);


    if (token != semicolonsym){

      break;

    }

    trim(file_text, offset);

    // check for block
    if (is_block(file_text, offset) != NO_ERR){

      break;

    }

    trim(file_text, offset);

    //expect another semicolon here
    token = is_semicolonsym(file_text, offset);

    if (token != semicolonsym){

      break;

    }

    trim(file_text, offset);

    token = is_procsym(file_text, offset);

  } while (!halt);

  //Statement
  trim(file_text, offset);

  if (is_statement(file_text, offset)){

    return FALSE;

  }

  return TRUE;

}

int is_statement (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  next = (file_text + local_offset);

  int status = NO_ERR;

  // call <identifier>
  if (is_callstatement(file_text, offset)) {

    return TRUE;

  }
  // begin <statement> {; <statement>} end
  else if (is_beginstatement(file_text, offset)) {

    return TRUE;

  }
  // if <condition> then <statement> [else <statement>]
  else if (is_ifstatement(file_text, offset)) {

    return TRUE;

  }
  // while <condition> do
  else if (is_whilestatement(file_text, offset)) {

    return TRUE;

  }
  // read <identifier>
  else if (is_readstatement(file_text, offset)) {

    return TRUE;

  }
  // write <identifier>
  else if (is_writestatement(file_text, offset)) {

    return TRUE;

  }

  // check if it is a becomes statement. reserved words take precedence, so
  // other statementexpressions listed above
  
  // <ident> := <expression>
  TokenType token;

  token = is_identsym(file_text, offset);

  if (token != identsym) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_becomessym(file_text, offset);

  if (token != becomessym) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_expression(file_text, offset)) {

    return FALSE;

  }

  return TRUE;

}

int is_callstatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_callsym(file_text, offset);

  if (token != callsym) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != identsym) {

    return FALSE;

  }

  return TRUE;

}

int is_beginstatement (char *file_text, int *offset) {

  TokenType token;

  token = is_beginsym(file_text, offset);

  if (token != beginsym) {

    return FALSE;

  }

  trim (file_text, offset);

  // okay, so I read the assignment rubric and was tracing the grammar closely.
  // based on the grammar and the code examples, the regular expression
  // for the begin statement does not appear to be:
  //
  // begin <statement> (; <statement>)* end
  //
  // but rather:
  //
  // begin (<statement>;)+ end
  //
  // under the first regex, this would be valid:
  // begin
  // write x <- no semi-colon
  // end
  //
  // in many examples the last statement of the begin statement
  // is terminated with a semi-colon. The exception is 'while' statements,
  // though im unsure how to proceed with that.

  trim(file_text, offset);

  int halt = 0;

  if(!is_statement(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  do {

    token = is_semicolonsym(file_text, offset);

    if (token != semicolonsym) {

      return FALSE;

    }

    trim (file_text, offset);

    if (is_statement(file_text, offset)) {

      halt = 0;

    }
    else {

      halt = 1;

    }

    trim(file_text, offset);

  } while(!halt);

  trim(file_text, offset);

  token = is_endsym(file_text, offset);

  if (token != endsym) {

    return FALSE;

  }

  return TRUE;

}

int is_whilestatement (char *file_text, int *offset) {

  TokenType token;

  token = is_whilesym(file_text, offset);

  if (token != whilesym) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_condition(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_dosym(file_text, offset);

  if (token != dosym) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_statement(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  return TRUE;

}

int is_ifstatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_ifsym(file_text, offset);

  if (token != ifsym) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_condition(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_thensym(file_text, offset);

  if (token != thensym) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_statement(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  // optionally accept an else statement

  token = is_elsesym(file_text, offset);

  // there is an else
  if (token == elsesym) {

    trim(file_text, offset);

    // now look for additional statement
    if (is_statement(file_text, offset)) {

      return TRUE;

    }
    else {

      // statement must follow else
      return FALSE;

    }

  }
  else {

    // else is not required
    return TRUE;

  }

}

int is_readstatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_readsym(file_text, offset);

  if (token != readsym) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token !=  identsym) {

    return FALSE;

  }

  trim(file_text, offset);

  return TRUE;

}

int is_writestatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_writesym(file_text, offset);

  if (token != writesym) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token !=  identsym) {

    return FALSE;

  }

  trim(file_text, offset);

  return TRUE;

}

int is_condition (char *file_text, int *offset) {

  TokenType token;

  token = is_oddsym(file_text, offset);

  trim(file_text, offset);

  // check if odd symbol
  if (token == oddsym) {

    // now see if there is an expression
    if (is_expression(file_text, offset)) {

      // this is a condition
      return TRUE;

    }

    trim(file_text, offset);

    // condition not of form:
    // odd <expression>
    //
    // now see if conidtion follows the form:
    // <expression> <relsym> <expression>

  }

  if (!is_expression(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_relsym(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  if (!is_expression(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  return TRUE;

}

int is_relsym (char *file_text, int *offset) {

  if (is_lessym(file_text, offset) == lessym) {

    return TRUE;

  }
  else if (is_leqsym(file_text, offset) == leqsym) {

    return TRUE;

  }
  else if (is_gtrsym(file_text, offset) == gtrsym) {

    return TRUE;

  }
  else if (is_geqsym(file_text, offset) == geqsym) {

    return TRUE;

  }
  else if (is_eqsym(file_text, offset)  == eqsym) {

    return TRUE;

  }
  else if (is_neqsym(file_text, offset) == neqsym) {

    return TRUE;

  }

  return FALSE;

}

int is_expression (char *file_text, int *offset) {

  TokenType token;

  token = is_plussym(file_text, offset);

  if (token != plussym) {

    token = is_minussym(file_text, offset);

    // ^ important to remember the is_[token]
    // functions have side effect of: 
    //   * moving offset
    //     pointer IFF the correct token is found,
    //   * inserting found token into table 
    //
    // that said, no 'check' is really necessary
    // here because the '-' was optional anyways
    //

  }

  // last token is a plus symbol or minus symbol
  trim(file_text, offset);

  // next expression must be a term
  if (!is_term(file_text, offset)) {

    return FALSE;

  }  

  trim(file_text, offset);

  int halt = 1;

  do {

    token = is_plussym(file_text, offset);

    if (token != plussym) {

      token = is_minussym(file_text, offset);

      // we are looking for (+ | -) after last term.
      // none found, so we break
      if (token != minussym) {

        break;

      }

    }

    // there was a plus or minus symbol, find the term
    trim(file_text, offset);

    if (!is_term(file_text, offset)) {

      // since the last symbol was (+ | -), a term is
      // manditory in the grammar
      return FALSE; 

    }

    // do another round
    halt = 0;

  } while(!halt);

  return TRUE;
  
}

int is_term (char *file_text, int *offset) {

  if (!is_factor(file_text, offset)) {

    return FALSE;

  }

  // factor found
  trim(file_text, offset);

  TokenType token;

  int halt = 1;

  // {( * | / ) <factor> }
  do {

    token = is_multsym(file_text, offset);

    if (token != multsym) {

      token = is_slashsym(file_text, offset);

      trim(file_text, offset);

      if (token != slashsym) {

        // {(* | / ) <factor>} is optional
        return TRUE;

      }

    }

    trim(file_text, offset);

    if (!is_factor(file_text, offset)) {

      // once we've scanned a mult or slash, a
      // factor is required
      return FALSE;

    }

    halt = 0;

  } while(!halt);

  return TRUE;

}

int is_factor (char *file_text, int *offset) {

  TokenType token;

  token = is_numbersym(file_text, offset);

  if (token == numbersym) {

    // a number is a factor 
    return TRUE;

  }

  token = is_identsym(file_text, offset);

  if (token == identsym) {

    // an identifier is a factor 
    return TRUE;

  }

  // check for left parentheses
  token = is_lparentsym(file_text, offset);

  if (token != lparentsym) {

    return FALSE;

  }

  trim(file_text, offset);

  // check for expression
  if (!is_expression(file_text, offset)) {

    return FALSE;

  }

  // check for right parentheses
  token = is_rparentsym(file_text, offset);

  trim(file_text, offset);

  if (token != rparentsym) {

    return FALSE;

  }

  return TRUE;

}

int is_commasym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ",", commasym);

}

int is_periodsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ".", periodsym);

}

int is_semicolonsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ";", semicolonsym);

}

int is_nulsym (char *file_text, int *offset) {

  // return is_terminal(file_text, offset, "const", semicolonsym);
  return 1;

}

int is_numbersym (char *file_text, int *offset) {

  // set the local offset to global offset
  int local_offset = *offset;

  char next;
  
  next = *(file_text + local_offset);

  if (!is_digit(next)) {

    return nulsym;

  }

  append_token_buffer(next);

  local_offset++;

  // get current character
  next = *(file_text + local_offset);

  int num_len = local_offset - *offset;

  while(is_digit(next)) {

    if (num_len < MAX_DIGIT_SIZE) {

      append_token_buffer(next);

    }

    local_offset++;

    num_len = local_offset - *offset;

    // get current character
    next = *(file_text + local_offset);

  }

  char *lexeme = get_token_buffer_value();

  if (num_len > MAX_DIGIT_SIZE) {

    printf("max number length of %d exceeded at position %d\n", MAX_DIGIT_SIZE, *offset);

    printf("storing truncated value of %s\n\n", lexeme);

  }

  insert_symbol(lexeme, numbersym);

  *offset = local_offset;


  // it's a number (e.g. "110082")
  return numbersym;

}

int is_plussym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "+", plussym);

}

int is_minussym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "-", minussym);

}

int is_multsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "*", multsym);

}

int is_slashsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "/", slashsym);

}

int is_oddsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "odd", oddsym);

}

int is_eqsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "=", eqsym);

}

int is_neqsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "<>", neqsym);

}

int is_lessym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "<", lessym);

}

int is_leqsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "<=", leqsym);

}

int is_gtrsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ">", gtrsym);

}

int is_geqsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ">=", geqsym);

}

int is_lparentsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "(", lparentsym);

}

int is_rparentsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ")", rparentsym);

}

int is_becomessym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, ":=", becomessym);

}

int is_beginsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "begin", beginsym);

}

int is_endsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "end", endsym);

}

int is_ifsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "if", ifsym);

}

int is_thensym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "then", thensym);

}

int is_whilesym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "while", whilesym);

}

int is_dosym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "do", dosym);

}

int is_callsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "call", callsym);

}

int is_constsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "const", constsym);

}

int is_varsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "var", varsym);

}

int is_procsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "procedure", procsym);

}

int is_writesym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "write", writesym);

}

int is_readsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "read", readsym);

}

int is_elsesym (char *file_text, int *offset) {
  
  return is_terminal(file_text, offset, "else", elsesym);

}

// trim file character buffer for whitespace and comments
int trim (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  int in_comment;

  next = (file_text + local_offset);

  // check if current and next character indicate a comment
  in_comment = (*next != '\0' && *next == '/' && *(next + 1) == '*');

  // check if current character is white space
  while(is_whitespace(*next) || in_comment) {

    if (in_comment && *next == '*' && *(next + 1) == '/') {

      in_comment = 0;

      // skip ahead of closing '/'
      local_offset++;

    }

    local_offset++;

    next = (file_text + local_offset);

    if (!in_comment) {

      in_comment = (*next != '\0' && *next == '/' && *(next + 1) == '*');

    }

  }

  *offset = local_offset;

  // finished
  return TRUE;

}


int is_whitespace (char c) {

  return (c == ' ' || c == '\n' || c == '\r' || c == '\t');

}

// helper function which takes a constant char* and checks the file_text
// + global_offset against it. if it's valid, the global offset is modified by
// a local offset. This only works for constant symbols.
int is_terminal (char *file_text, int *offset, char symbol[], int token_type) {

  int local_offset = *offset;

  // find size of symbol we are checking against.
  int symbol_size = strlen(symbol);  

  char next; 

  for (int i = 0; i < symbol_size; i++) {

    next = *(file_text + local_offset);

    if (next == '\0' || next != symbol[i]) {

      return NO_ERR;

    }

    local_offset++;

  } 

  // put in the symbol
  insert_symbol(symbol, token_type);

  *offset = local_offset;

  return token_type;

}

// since individual letters are not stored and are more of a grammatical feature,
// this function returns TRUE or FALSE

int is_letter (char c ) {

  return ((c >= 'A' &&  c <= 'Z') || (c >= 'a' && c <= 'z'));

}

int is_digit (char c) {

  return ( c >= '0' && c <= '9');

}

void print_status (int status) {

  char *status_str = statuses[status];

  printf("STATUS: %s\n", status_str);

}

void print_lexemes () {

}
