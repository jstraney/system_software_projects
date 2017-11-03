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
// Disclaimer! When we coded assignment two, we slightly misunderstood the scope
// of the scanners job and had it implemented in a tree structure to ensure
// the code file was syntactically valid as it scanned (the parsers job...)
// As a result, the code file was extra large and has made segwaying into
// this assignment challenging. In otherwords, the tokens are lexically analyzed
// as they are parsed, which are then generated into code
//
// I have emailed Dr. Montane who has advised us
// to simply have the code generator read the lexeme table (which should be
// syntactically valid already by nature of being implemented in a tree)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZER 
#include "analyzer.h"
#define ANALYZER 
#endif

#ifndef GENERATOR
#include "generator.h"
#define GENERATOR
#endif

#ifndef VM 
#include "vm.h"
#define VM 
#endif

#define ANALYZER_OPTION_QUIT      0
#define ANALYZER_OPTION_RUN       1
#define ANALYZER_OPTION_PRINT     2 

#define ANALYZER_MODE_NORMAL      0
#define ANALYZER_MODE_INTERACTIVE 1

// we will make our errors go in the opposite direction so they
// are not confused with symbol enumerations
#define ANALYZER_VALID                  0
#define ANALYZER_NO_ERR                -1 
#define ANALYZER_ERR_IDENT_START       -2 
#define ANALYZER_ERR_NUM_TOO_LONG      -3 
#define ANALYZER_ERR_IDENT_TOO_LONG    -4 
#define ANALYZER_ERR_NEEDS_BECOMES     -5 
#define ANALYZER_ERR_EQUAL_THEN_NUM    -6 
#define ANALYZER_ERR_IDENT_NEEDED      -7 
#define ANALYZER_ERR_NO_SEMICOLON      -8 
#define ANALYZER_ERR_PROC_SYMBOL       -9 
#define ANALYZER_ERR_NO_PERIOD        -10 
#define ANALYZER_ERR_UND_IDENT        -11 
#define ANALYZER_ERR_PROC_CONST_ASN   -12 
#define ANALYZER_ERR_NO_ASN           -13 
#define ANALYZER_ERR_MEANING_IDENT    -14 
#define ANALYZER_ERR_NO_THEN          -15 
#define ANALYZER_ERR_NO_DO            -16 
#define ANALYZER_ERR_NO_REL           -17 
#define ANALYZER_ERR_FORBIDDEN_PROC   -18 
#define ANALYZER_ERR_NO_RIGHT_PAREN   -19 
#define ANALYZER_ERR_INVALID_FACTOR   -20 
#define ANALYZER_ERR_INVALID_TOKEN    -21 
#define ANALYZER_ERR_NO_EQUAL         -22 
#define ANALYZER_ERR_NO_COMMA         -23 
#define ANALYZER_ERR_EXPRESSION_START -24 
#define ANALYZER_ERR_TERM_START       -25 
#define ANALYZER_ERR_NO_END           -26 
#define ANALYZER_ERR_BAD_ASSIGN       -27 
#define ANALYZER_ERR_BAD_CONST        -28 
#define ANALYZER_ERR_INVALID_READ     -29 
#define ANALYZER_ERR_INVALID_WRITE    -30 

#define ANALYZER_STATUS_QUIT         5 
#define ANALYZER_STATUS_FINISHED     6 

char *analyzer_statuses[] = {
  "scanning and parsing is finished",
  "unrecognized token",
  "identifier must start with a letter",
  "number too long (max length of 5)",
  "identifier is too long (max length of 11)",
  "expected := in var assignment",
  "number required after const assignment",
  "identifier expected",
  "semicolon expected",
  "incorect symbol after procedure declaration",
  "period expected",
  "undeclared Identifier",
  "cannot assign to constant or procedure",
  "expected assignment operator",
  "meaningless variable or constant",
  "expected then",
  "expected do",
  "expected relational operator (< > <= >= = or <>)",
  "expression cannot contain procedure identifier",
  "right parentheses missing",
  "invalid factor",
  "unrecognized token",
  "constant expects = ",
  "expected comma",
  "invalid start of expression",
  "invalid start of term",
  "end expected after begin statement",
  "invalid assignment. must assign to var only (const and proc assignment forbidden)",
  "invalid constant. symbol already used.",
  "invalid read. variable is undefined",
  "invalid write. variable is undefined"
};

#define ANALYZER_MAX_TOKEN_SIZE 11
#define ANALYZER_MAX_IDENT_SIZE 11
#define ANALYZER_MAX_DIGIT_SIZE  5

/*
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
*/

////////////////////////////////////////////////////////////////////////////////
char *token_buffer;

int buffer_index = 0;

// hash function to key the unique symbol in the table
// we used this academic page as inspiration to help randomize the result. could have used sidechaining
// to help handle collisions but did not have time. will most likely not produce collisions unless many
// long, exotic variables are used.
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

Symbol Symbol_new (int  type,  char *name,  int  val, int addr) {

  Symbol this;

  this.type  = type;
  this.name  = name;
  this.val   = val;
  this.level = gen_get_lex_level();
  this.addr  = addr;

  return this;

}

int Symbol_insert(Symbol symbol) {

  // derive hash from the name
  int key = hash(symbol.name);

  // put into the symbol
  symbol_table[key] = symbol;

  // debugging
  // printf("new symbol inserted\n");
  // print_symbol(symbol);

  return GENERATOR_STATUS_OK;

}

Symbol find(char *ident) {

  int key = hash(ident);

  return symbol_table[key];

}

void print_symbol(Symbol symbol) {

  int    type = symbol.type;
  char * name = symbol.name;
  int     val = symbol.val;
  int   level = symbol.level;
  int address = symbol.addr;

  printf("type : %d  name: %s  value: %d level: %d addres: %d\n", type, name, val, level, address);

}

// first token in the program
Token *head;
// last token. allows for O(1) lookup of last token
Token *tail; 

Token *Token_new(char *lexeme, int token_type) {

  Token *this;

  this = malloc(sizeof(Token));

  this->lexeme     = lexeme;

  this->token_type = token_type;

  this->next = NULL;

  return this;

}

// put symbol into the symbol table
int insert_token (char *lexeme, int token_type) {

  Token *current = head;

  // traverse symbol table
  while (current->next != NULL) {

    current = current->next;

  }

  Token *token = Token_new(lexeme, token_type);
  

  // set the vacant 'next' to symbol
  current->next = token;

  // set tail pointer to the token for constant lookup time
  tail->next = token;

  return ANALYZER_VALID;

}

Token get_last_token () {

  return *(tail->next);

}

void print_token (Token token) {

  // our implementation of the symbol table has the null symbol pointing to the
  // start of the table (like a linked list. here we will skip it.
  if (token.token_type == nulsym)
    return;

  printf("%-11s\t\t%2d\n", token.lexeme, token.token_type);

}

// starts at first token and goes to end of program.
void print_token_table () {

  Token *token;

  token = head;

  printf("Lexeme Table:\n");

  printf("lexeme\t\ttoken type\n");

  while (token->next != NULL ) {

    token = token->next;

    print_token(*token);

  }

  printf("\n");

}

void print_token_list () {

  Token *token;

  token = head;

  printf("Lexeme List:\n");

  while (token->next != NULL ) {

    token = token->next;

    if (token->token_type == 2) {

      printf("%d %s ", token->token_type, token->lexeme);

    }
    else {

      printf("%d ", token->token_type);

    }

  }

  printf("\n");

}

/*
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
*/

void append_token_buffer(char next) {

  // insert the next character into our buffer
  *(token_buffer + buffer_index) = next;  

  buffer_index++;

}

char *get_token_buffer_value() {

  // size of complete string in buffer 
  int size;

  for (int i = 0; i < ANALYZER_MAX_TOKEN_SIZE; i++) {

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
  memset(token_buffer, '\0', ANALYZER_MAX_TOKEN_SIZE + 1);

  buffer_index = 0;

}

int analyzer_entry (char *file_name) {

  // status starts off ok
  int status = ANALYZER_VALID;

  // don't forget to allocate an extra space for '\0'
  token_buffer = (char *)calloc(ANALYZER_MAX_TOKEN_SIZE + 1, sizeof (char));

  // create an empty start symbol
  head = Token_new("", nulsym);
  tail = Token_new("", nulsym);
  // always point to end of list
  tail->next = head;
  /*
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

    status = ANALYZER_NO_ERR;

    return status;

  }

  */

  FILE *file = open_file(file_name);

  if (file == NULL) {

    printf("cannot find the file %s\n", file_name);

    return 1;

  }

  /*
  if (mode == MODE_INTERACTIVE) {

    status = interactive_loop(file);

  }
  else if (mode == MODE_NORMAL) {

    status = event_loop(file);

  }
  */

  status = analyzer_event_loop(file);

  // printf("%d\n", status);

  fclose(file);

  return status;

}

FILE *open_file(char *file_name) {

  FILE *file;

  file = fopen(file_name, "r");

  return file;

}

/*
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
*/

/*
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
*/

/*
// run the vm through interactive mode
int interactive_loop (FILE *file) {

  int status = ANALYZER_VALID;

  // print the prompt once on start up. print again
  // if the COMMAND_HELP is issued 
  print_prompt();

  // integer flag COMMAND_<command>
  int user_command;

  while (status == ANALYZER_VALID) {

    printf(">> ");

    user_command = get_user_command();

    if (user_command == COMMAND_INVALID) {

      printf("Please enter a valid command. use 'h' for help");
      // return to top of the loop
      continue;
      
    }

    else if (user_command == COMMAND_QUIT) {

      status = ANALYZER_STATUS_QUIT;

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
*/

int analyzer_event_loop (FILE *file) {

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

  // our lexical-analyzer/parser loads the file into a character buffer,
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

  int status = ANALYZER_VALID;

  // do an initial trim for any proceeding whitespace or comments
  trim(file_text, offset);

  while(status == ANALYZER_VALID) { 
    
    // must have a block
    status = is_block(file_text, offset);

    int halt;

    halt = (status != ANALYZER_VALID);

    if (halt) {

      break;

    }
     
  }

  trim(file_text, offset);

  if (status == ANALYZER_NO_ERR) {

    status = is_periodsym(file_text, offset);

    if (status != ANALYZER_VALID) {

      status = ANALYZER_ERR_NO_PERIOD;

    }

  }

  free(file_text);

  free(offset);

  // add SIO command if analyzer valid
  if (status == ANALYZER_VALID) {

    // properly halt program (we're assuming not to generate command if
    // period is not provided).
    gen(VM_OP_SIO, 0, 0, 3);

  }

  // negative statuses are errors. 0 is no error
  if (status <= 0) {

    // flip the status to get the error code
    printf("LEXER/PARSER STATUS:%s\n\n", analyzer_statuses[-status]);

  }

  return status;

}

int is_block (char *file_text, int *offset) {

  int status;

  // if any status besides no error is returned, the process halts
  status = is_constdec (file_text, offset); 
  // if the analyzer is valid or returned error, return from block procedure. if it was NO_ERR, simply
  // continue and see what kind of block it is. repeat for each block pattern
  ((status == ANALYZER_VALID || status < ANALYZER_NO_ERR))|| (status = is_vardec   (file_text, offset));
  ((status == ANALYZER_VALID || status < ANALYZER_NO_ERR))|| (status = is_procdec  (file_text, offset));
  ((status == ANALYZER_VALID || status < ANALYZER_NO_ERR))|| (status = is_statement (file_text, offset));

  return status;

}

int is_constdec (char *file_text, int *offset) {

  TokenType token;

  token = is_constsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    // no error, perse as it could still be a vardec or some
    // other valid expression
    return token;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  trim(file_text, offset);

  if (token != ANALYZER_VALID) {

    // already used 'const'. must be a valid token
    return ANALYZER_ERR_IDENT_NEEDED;

  }

  Token constant = get_last_token();

  // check if constant lexeme is used in symbol table
  Symbol existing_const = find(constant.lexeme);

  // the symbol should not exist (even as a constant!)
  if (existing_const.name != NULL) {

    return ANALYZER_ERR_BAD_CONST;

  }

  token = is_eqsym (file_text, offset);
  
  trim(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_NO_EQUAL;
    
  }

  token = is_numbersym(file_text, offset);

  trim(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_EQUAL_THEN_NUM;

  }

  Token number = get_last_token();

  // get constant integer value from string
  int const_value  = atoi(number.lexeme);

  // put constant into the symbol table
  Symbol const_sym = Symbol_new(const_ident, constant.lexeme, const_value, gen_get_bp_offset());

  // put symbol into the symbol table
  Symbol_insert(const_sym);

  // increase gen's abstraction of base_pointer
  gen_inc_bp_offset();

  // increase room for constant to be stored in memory
  gen(VM_OP_INC, 0, 0, 1);

  // get current register. note that due to the nature of the constant
  // assignment, we can write over the register if we are making multiple
  // constant assignments (no need to call gen_inc_reg_offset)
  int const_reg = gen_get_reg_offset();

  // create code command to load immediate value into register
  gen(VM_OP_LIT, const_reg, 0, const_value);
  
  //  get register with immediate value and store in stack 
  gen(VM_OP_STO, const_reg, const_sym.level, const_sym.addr);

  // optionally check for commas
  token = is_commasym(file_text, offset);

  trim(file_text, offset);

  // loop while we have a comma followed by identifier
  while (token == ANALYZER_VALID) {

    token = is_identsym(file_text, offset);

    trim(file_text, offset);

    if (token != ANALYZER_VALID) {

      return ANALYZER_ERR_IDENT_NEEDED;

    }

    // get the constant token recently lexed
    constant = get_last_token();

    // look for existing constant. re-using the variable above
    existing_const = find(constant.lexeme);

    if (existing_const.name != NULL) {

      // constant lexeme cannot exist already!
      return ANALYZER_ERR_BAD_CONST;

    }

    token = is_eqsym (file_text, offset);
    
    trim(file_text, offset);

    if (token != ANALYZER_VALID) {

      return ANALYZER_NO_ERR;

    }

    token = is_numbersym(file_text, offset);

    trim(file_text, offset);

    if (token != ANALYZER_VALID) {

      return token;

    }

    // fetch the last_token into our number token
    number = get_last_token();

    const_value = atoi(number.lexeme);

    // lexeme not used! produce a symbol
    const_sym = Symbol_new(const_ident, constant.lexeme, const_value, gen_get_bp_offset()); 

    Symbol_insert(const_sym);

    gen_inc_bp_offset();

    // increase space for constant
    gen(VM_OP_INC, 0, 0, 1);

    int const_reg = gen_get_reg_offset();

    gen(VM_OP_LIT, const_reg, 0, const_value);

    gen(VM_OP_STO, const_reg, const_sym.level, const_sym.addr);

    token = is_commasym(file_text, offset);

    trim(file_text, offset);

  }

  token = is_semicolonsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_NO_SEMICOLON;

  }

  trim(file_text, offset);

  // no problem
  return ANALYZER_VALID;

}

int is_vardec (char *file_text, int *offset) {

  TokenType token;

  token = is_varsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    // could still be a statement etc.
    return token; 

  }

  // number of vars to store in this lexical space
  int num_m = 1;

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_IDENT_NEEDED; 

  }

  Token ident = get_last_token();

  // use the bp offset from start of current activation record 
  Symbol symbol = Symbol_new(var_ident, ident.lexeme, 0, gen_get_bp_offset());

  gen_inc_bp_offset();

  Symbol_insert(symbol);

  trim(file_text, offset);

  int halt = 0;

  // check for commas followed by identifiers until we
  // reach a semicolon
  do {

    // check if next token is comma
    token = is_commasym(file_text, offset); 
    
    if (token == ANALYZER_VALID) { 

      trim (file_text, offset);

      // now see if next token is an identifier
      token = is_identsym(file_text, offset); 
      
      trim(file_text, offset);

      // comma must be followed by an identifier
      if (token != ANALYZER_VALID) {

        return token;

      }

      // increase number of m to use in INC
      num_m ++;

      Token ident = get_last_token();

      Symbol symbol = Symbol_new(var_ident, ident.lexeme, 0, gen_get_bp_offset());

      gen_inc_bp_offset();

      Symbol_insert(symbol);

    }
    // if there is no comma, we must find a semicolon
    else {

      // trim(file_text, offset);
      // check if next symbol is a semicolon
      token = is_semicolonsym(file_text, offset);

      if (token != ANALYZER_VALID) {

        return ANALYZER_ERR_NO_SEMICOLON;

      }
      else {

        halt = 1;

      }

    }

  } while (!halt);
  
  // allocate m spaces for m variables
  gen(VM_OP_INC, 0, 0, num_m);

  // trim just in case
  trim(file_text, offset);

  return ANALYZER_VALID;

}


int is_identsym (char *file_text, int *offset) {

  int local_offset = *offset;

  // check first character to see if it is alpha 
  char next;

  next = *(file_text + local_offset);

  int err = ANALYZER_VALID;

  if (!is_letter(next)) {

    // printf("identifier at position %d must start with a letter\n", local_offset);
    
    return ANALYZER_ERR_IDENT_START;

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
     
    if (ident_len < ANALYZER_MAX_TOKEN_SIZE) {

      append_token_buffer(next);

    }

    local_offset++;

    // fetch current char 
    next = *(file_text + local_offset);

  } 

  char *lexeme = get_token_buffer_value();

  if (ident_len > ANALYZER_MAX_TOKEN_SIZE) { 

    // printf("token of length %d exceeds max length of 11 at position %d\n", ident_len, *offset);
    // printf("storing truncated value of %s instead\n\n", lexeme);

  }

  // identifier cannot be reserved.
  if (is_reserved_word(lexeme)) {

    return ANALYZER_NO_ERR;

  }

  insert_token(lexeme, identsym);

  // we stopped. reset the global offset
  *offset = local_offset;

  return ANALYZER_VALID;

}

int is_procdec (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  next = (file_text + local_offset);

  int status = ANALYZER_VALID;

  TokenType token;

  int halt = 0;

  //loops until it doesn't see "procedure".
  do {

    // checks for "procedure"
    token = is_procsym(file_text, offset);

    if (token != ANALYZER_VALID) {

      break;

    }

    trim(file_text, offset);

    //expect identifier here
    token = is_identsym(file_text, offset);

    if (token != ANALYZER_VALID) {

      break;

    }

    // insert identifier as a procedure into symbol table
    Token procedure = get_last_token();

    // create proc identifier symbol in symbol table
    trim(file_text, offset);

    //expect a semicolon here
    token = is_semicolonsym(file_text, offset);


    if (token != ANALYZER_VALID){

      break;

    }

    trim(file_text, offset);

    // gets program counter befor following block is created.
    int proc_address = gen_get_pc();

    // address holds address in code memory
    Symbol symbol = Symbol_new(proc_ident, procedure.lexeme, 0, gen_get_pc());

    Symbol_insert(symbol);


    // check for block
    token = is_block(file_text, offset);

    if (token != ANALYZER_VALID){

      break;

    }

    trim(file_text, offset);

    //expect another semicolon here
    token = is_semicolonsym(file_text, offset);

    if (token != ANALYZER_VALID){

      break;

    }

    trim(file_text, offset);

    token = is_procsym(file_text, offset);

  } while (!halt);

  //Statement
  trim(file_text, offset);

  token = is_statement(file_text, offset);

  if (token != ANALYZER_VALID){

    // could still be a statement
    return token;

  }

  return ANALYZER_VALID;

}

int is_statement (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  next = (file_text + local_offset);

  int status = ANALYZER_VALID;

  status = is_callstatement(file_text, offset);

  // call <identifier>
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return ANALYZER_VALID;

  }
  else {

    status = is_beginstatement(file_text, offset);

  }

  // begin <statement> {; <statement>} end
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return status;

  }
  else {

    status = is_ifstatement(file_text, offset);

  }

  // if <condition> then <statement> [else <statement>]
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return status;

  }
  else {

    // while <condition> do
    status = is_whilestatement(file_text, offset);

  }
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return status;

  }
  else {

    status = is_readstatement(file_text, offset);

  }
  // read <identifier>
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return status;

  }
  else {
    // write <identifier>
    status = is_writestatement(file_text, offset);
  }
  if (status == ANALYZER_VALID || status < ANALYZER_NO_ERR) {

    return status;

  }

  // check if it is a becomes statement. reserved words take precedence, so
  // other statementexpressions listed above
  
  // <ident> := <expression>
  int token;

  token = is_identsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  Token ident = get_last_token();

  Symbol symbol = find(ident.lexeme);

  if (symbol.name == 0) {

    return ANALYZER_ERR_UND_IDENT;

  }
  else if (symbol.type != var_ident) {

    return ANALYZER_ERR_BAD_ASSIGN;

  }

  trim(file_text, offset);

  token = is_becomessym(file_text, offset);

  if (token != ANALYZER_VALID) {

    
    return ANALYZER_ERR_NEEDS_BECOMES;

  }

  trim(file_text, offset);

  token = is_expression(file_text, offset);

  if (token != ANALYZER_VALID) {

    return token;

  }

  // expression value should be in the last used register (we hope!)
  int result_reg = gen_get_reg_offset(); 

  // get ident address.
  int ident_address = symbol.addr;

  // get ident lexical level
  int sym_lex_level = symbol.level;
  // take result in reg and store result in memory address. let
  // the VM actually handle the base pointer
  
  // store value 
  gen(VM_OP_STO, result_reg, sym_lex_level, ident_address);

  return ANALYZER_VALID;

}

int is_callstatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_callsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  // increase the lexical level
  gen_inc_lex_lev();

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  return ANALYZER_VALID;

}

int is_beginstatement (char *file_text, int *offset) {

  int token;

  token = is_beginsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  trim (file_text, offset);

  // increase lexical level now that we are in a new block
  gen_inc_lex_lev();

  trim(file_text, offset);

  int halt = 0;

  token = is_statement(file_text, offset);

  if(token != ANALYZER_VALID) {

    return token;

  }

  trim(file_text, offset);

  do {

    token = is_semicolonsym(file_text, offset);

    if (token != ANALYZER_VALID || token < ANALYZER_NO_ERR) {

      return ANALYZER_ERR_NO_SEMICOLON;

    }

    trim (file_text, offset);

    token = is_statement(file_text, offset);
    
    if (token == ANALYZER_VALID) {

      halt = 0;

    }
    else {

      halt = 1;

    }

    trim(file_text, offset);

  } while(!halt);

  if (token < ANALYZER_NO_ERR) {

    return token;

  }

  trim(file_text, offset);

  token = is_endsym(file_text, offset);

  if (token != ANALYZER_VALID || token < ANALYZER_NO_ERR) {

    return ANALYZER_ERR_NO_END;

  }

  gen_dec_lex_lev();

  return ANALYZER_VALID;

}

int is_whilestatement (char *file_text, int *offset) {

  int token;

  token = is_whilesym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  // grab the address of conditional for JMP at end of loop.
  int check_addr = gen_get_pc();

  trim(file_text, offset);

  token = is_condition(file_text, offset);

  if (token != ANALYZER_VALID || token < ANALYZER_NO_ERR) {

    return token;

  }

  // get register for result of condition above
  int condition_result_reg = gen_get_reg_offset();

  // address of conditional jump. used to modify m at end of loop
  int jpc_addr = gen_get_pc();
  
  // set conditional jump m to 0 for now. will set
  // later when we are sure where the statement ends
  gen(VM_OP_JPC, condition_result_reg, 0, 0);

  trim(file_text, offset);

  token = is_dosym(file_text, offset);

  if (token != ANALYZER_VALID || token < ANALYZER_NO_ERR) {

    return ANALYZER_ERR_NO_DO;

  }

  trim(file_text, offset);

  token = is_statement(file_text, offset);

  if (token != ANALYZER_VALID || token < ANALYZER_NO_ERR) {

    return token;

  }

  // insert JMP to conditional check of loop
  gen(VM_OP_JMP, 0, 0, check_addr);

  // get address after statement in while loop.
  int break_addr = gen_get_pc();
  
  // change JPC at top of while loop to go to instr. address after
  // loop instruction
  gen_alt_instr_m(jpc_addr, break_addr);

  trim(file_text, offset);

  return ANALYZER_VALID;

}

int is_ifstatement (char *file_text, int *offset) {

  int token; 

  token = is_ifsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  trim(file_text, offset);

  token = is_condition(file_text, offset);

  if (token != ANALYZER_VALID) {

    return token;

  }

  // get reg of conditional check result 
  int cond_result_reg = gen_get_reg_offset();

  trim(file_text, offset);

  token = is_thensym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_NO_THEN;

  }

  // address of jump command
  int jmp_addr = gen_get_pc();

  // insert conditional. leave m at 0 to be change later
  gen(VM_OP_JPC, cond_result_reg, 0, 0);

  trim(file_text, offset);

  token = is_statement(file_text, offset);

  if (token != ANALYZER_VALID) {

    return token;

  }

  // address to jump to
  int jumpto = gen_get_pc();

  // get jpc command inserted earlier and change m
  gen_alt_instr_m(jmp_addr, jumpto);

  // get pc from gen to handle (gen_pc was was moved when
  // generating code in statement above)

  trim(file_text, offset);

  // optionally accept an else statement

  token = is_elsesym(file_text, offset);

  // there is an else
  if (token == ANALYZER_VALID) {

    trim(file_text, offset);

    token = is_statement(file_text, offset);
    // now look for additional statement
    if (token == ANALYZER_VALID) {

      return token;

    }
    else {

      // statement must follow else
      return ANALYZER_NO_ERR;

    }

  }

  // else is not required
  

  return ANALYZER_VALID;

}

int is_readstatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_readsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  Token identifier = get_last_token();

  Symbol ident_sym = find(identifier.lexeme);

  if (ident_sym.name == NULL) {

    return ANALYZER_ERR_INVALID_READ;

  }

  gen_inc_reg_offset();

  int read_reg = gen_get_reg_offset();

  // generate read code
  gen(VM_OP_SIO, read_reg, 0, 2);

  // store read in value into variable 
  gen(VM_OP_STO, read_reg, ident_sym.level, ident_sym.addr);

  trim(file_text, offset);

  return ANALYZER_VALID;

}

int is_writestatement (char *file_text, int *offset) {

  TokenType token; 

  token = is_writesym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  Token identifier = get_last_token();

  Symbol ident_sym = find(identifier.lexeme);

  if (ident_sym.name == NULL) {

    return ANALYZER_ERR_INVALID_WRITE; 

  }

  gen_inc_reg_offset();

  // generate write code
  int write_reg = gen_get_reg_offset();

  // store variable in register to write out to screen 
  gen(VM_OP_LOD, write_reg, ident_sym.level, ident_sym.addr);

  gen(VM_OP_SIO, write_reg, 0, 1);

  trim(file_text, offset);

  return ANALYZER_VALID;

}

int is_condition (char *file_text, int *offset) {

  TokenType token;

  token = is_oddsym(file_text, offset);

  trim(file_text, offset);

  // check if odd symbol
  if (token == ANALYZER_VALID) {

    trim(file_text, offset);

    // if valid expression, the code will be generated as a side effect.
    token = is_expression(file_text, offset);

    // now see if there is an expression
    if (token == ANALYZER_VALID) {

      // get register of expression result
      int expression_result_reg = gen_get_reg_offset();

      gen_inc_reg_offset();

      int imm_two_reg = gen_get_reg_offset();

      // load immediate value of 2 in next register
      gen(VM_OP_LIT, imm_two_reg, 0, 2);

      gen_inc_reg_offset();

      int mod_result_reg = gen_get_reg_offset();

      // take the mod of the expression result and an immediate value of 2
      // if it is 0 (or even) the ODD check is deasserted 
      gen(VM_OP_MOD, mod_result_reg, expression_result_reg, imm_two_reg);

      // generate a MOD command and put result in next reg

      return ANALYZER_VALID;

    }

    trim(file_text, offset);

    // condition not of form:
    // odd <expression>
    //
    // now see if conidtion follows the form:
    // <expression> <relsym> <expression>

  }

  token = is_expression(file_text, offset);

  if (token != ANALYZER_VALID) {

    return token;

  }

  // get result from expression (should be in last register used)
  int expr1_reg = gen_get_reg_offset();

  trim(file_text, offset);

  token = is_relsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return token;

  }

  // get last token to see the kind of rel symbol
  Token rel = get_last_token();

  // there are 6 operations
  int rel_ops[] = {
    VM_OP_EQL, VM_OP_NEQ, VM_OP_LSS,
    VM_OP_LEQ, VM_OP_GTR, VM_OP_GEQ
  };

  // eqsym, the enum value of 9, minus 9 = 0
  // neqsym, the enum value of 10, minus 9 = 1... etc
  int rel_op = rel_ops[rel.token_type - 9];

  trim(file_text, offset);

  token = is_expression(file_text, offset);

  if ( token != ANALYZER_VALID) {

    return token;

  }

  // expression should be evaluated and placed into a register
  int expr2_reg = gen_get_reg_offset(); 

  // go to next reg for result
  gen_inc_reg_offset();

  int condition_result_reg = gen_get_reg_offset();

  // generate EQL, NEQ, LSS, w/e was determined from the rel token
  // take result from expression 1, expression 2 and put in a new register
  gen(rel_op, condition_result_reg, expr1_reg, expr2_reg);

  // don't increment reg, because we will need result for any jumps 

  trim(file_text, offset);

  return ANALYZER_VALID;

}

int is_relsym (char *file_text, int *offset) {

  TokenType token;

  // trying <> first so it is not confused with <
  token = is_neqsym(file_text, offset);

  if (token == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }
  else {

    // must try <= before < to prevent a false, preliminary detection
    token = is_leqsym(file_text, offset);

  }
  if (token == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }
  else {

    token = is_lessym(file_text, offset);

  }
  if (token == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }
  else {

    token = is_geqsym(file_text, offset);

  }
  if (token == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }
  else {

    token = is_gtrsym(file_text, offset);

  }
  if (token == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }

  else {

    token = is_eqsym(file_text, offset);

  }
  if (token  == ANALYZER_VALID) {

    return ANALYZER_VALID;

  }
  

  return ANALYZER_ERR_NO_REL;

}

int is_expression (char *file_text, int *offset) {

  TokenType token;

  token = is_plussym(file_text, offset);

  int is_plus_or_minus;

  if (token != ANALYZER_VALID) {

    token = is_minussym(file_text, offset);

    if (token == ANALYZER_VALID) {

      // negative 1 indicages -
      is_plus_or_minus = -1;

    }
    else {

      // no unary operator
      is_plus_or_minus = 0;

    }

  }
  else {

    // is positive
    is_plus_or_minus = 1;

  }

  // last token is a plus symbol or minus symbol
  trim(file_text, offset);

  token = is_term(file_text, offset);

  // next expression must be a term
  if (token != ANALYZER_VALID) {

    return token;

  }  

  // if negative negate the term
  if (is_plus_or_minus == -1) {

    // result term should be in last register used
    gen(VM_OP_NEG, gen_get_reg_offset(), gen_get_last_reg(), 0 );

  }


  trim(file_text, offset);

  int halt = 1;

  do {

    // -1 == sub, 0 == invalid, 1 == add
    int add_or_sub = 0; 

    int fst_term_reg = gen_get_reg_offset();

    token = is_plussym(file_text, offset);

    if (token != ANALYZER_VALID) {

      token = is_minussym(file_text, offset);

      // we are looking for (+ | -) after last term.
      // none found, so we break
      if (token != ANALYZER_VALID) {

        break;

      }
      else {

        add_or_sub = -1;

      }

    }
    else {

      add_or_sub = 1;

    }

    // there was a plus or minus symbol, find the term
    trim(file_text, offset);

    token = is_term(file_text, offset);

    if (token != ANALYZER_VALID) {

      // since the last symbol was (+ | -), a term is
      // manditory in the grammar
      return token; 

    }

    int snd_term_reg = gen_get_reg_offset();

    // increase reg for new result
    
    gen_inc_reg_offset();

    int result_reg = gen_get_reg_offset();

    if (add_or_sub == 1) {

      gen(VM_OP_ADD, result_reg, fst_term_reg, snd_term_reg);

    }
    else if(add_or_sub == -1) {

      gen(VM_OP_SUB, result_reg, fst_term_reg, snd_term_reg);

    }

    // do another round
    halt = 0;

  } while(!halt);


  return ANALYZER_VALID;
  
}

int is_term (char *file_text, int *offset) {

  TokenType token;

  token = is_factor(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_TERM_START;

  }

  // factor found
  trim(file_text, offset);

  int halt = 1;

  // {( * | / ) <factor> }
  do {

    token = is_multsym(file_text, offset);

    // 0 == neither, 1 == mult, -1 == div
    int is_mult_or_div = 0;

    if (token != ANALYZER_VALID) {

      trim(file_text, offset);

      token = is_slashsym(file_text, offset);

      trim(file_text, offset);

      if (token != ANALYZER_VALID) {

        is_mult_or_div = 0;

        // {(* | / ) <factor>} is optional
        return ANALYZER_VALID;

      }
      else {

        is_mult_or_div = -1;

      }

    }
    else {

      is_mult_or_div = 1;

    }

    // get reg of first factor
    int fst_factor_reg = gen_get_reg_offset();

    trim(file_text, offset);

    token = is_factor(file_text, offset);

    if (token != ANALYZER_VALID) {

      // once we've scanned a mult or slash, a
      // factor is required
      return token;

    }

    // get reg of second factor
    int snd_factor_reg = gen_get_reg_offset();

    // store result in new reg
    gen_inc_reg_offset();

    // operation fst and snd factors
    int op;

    if (is_mult_or_div == 1) {

      op = VM_OP_MUL;

    }
    else {

      op = VM_OP_DIV;

    }

    gen(op, gen_get_reg_offset(), fst_factor_reg, snd_factor_reg);

    // will continue to loop, increasing the register number round robin style
    // (which should work in sequence)

    halt = 0;

  } while(!halt);

  return ANALYZER_VALID;

}

// because factors are always used in assignment,
// here is where we are using our register allocation
// strategy.
int is_factor (char *file_text, int *offset) {

  TokenType token;

  token = is_numbersym(file_text, offset);

  if (token == ANALYZER_VALID) {

    Token number = get_last_token(); 

    int value = atoi(number.lexeme);

    // go to next reg for round robin
    gen_inc_reg_offset();

    // use LIT to put value in a register
    gen(VM_OP_LIT, gen_get_reg_offset(), 0, value);

    // a number is a factor 
    return ANALYZER_VALID;

  }

  token = is_identsym(file_text, offset);

  if (token == ANALYZER_VALID) {

    // load token
    Token identifier = get_last_token();

    Symbol ident_sym = find(identifier.lexeme);

    int ident_address   = ident_sym.addr;
    int ident_lex_scope = ident_sym.level;

    gen_inc_reg_offset();

    gen(VM_OP_LOD, gen_get_reg_offset(), ident_lex_scope, ident_address);

    // an identifier is a factor 
    return ANALYZER_VALID;

  }

  // check for left parentheses
  token = is_lparentsym(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_INVALID_TOKEN;

  }

  trim(file_text, offset);

  // check for expression
  token = is_expression(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_NO_ERR;

  }

  trim(file_text, offset);

  // check for right parentheses
  token = is_rparentsym(file_text, offset);

  trim(file_text, offset);

  if (token != ANALYZER_VALID) {

    return ANALYZER_ERR_NO_RIGHT_PAREN;

  }

  return ANALYZER_VALID;

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

    // can be 'caught' later in the tree
    return ANALYZER_ERR_INVALID_TOKEN;

  }

  append_token_buffer(next);

  local_offset++;

  // get current character
  next = *(file_text + local_offset);

  int num_len = local_offset - *offset;

  while(is_digit(next)) {

    if (num_len < ANALYZER_MAX_DIGIT_SIZE) {

      append_token_buffer(next);

    }

    local_offset++;

    num_len = local_offset - *offset;

    // get current character
    next = *(file_text + local_offset);

  }

  char *lexeme = get_token_buffer_value();

  if (num_len > ANALYZER_MAX_DIGIT_SIZE) {

    // printf("max number length of %d exceeded at position %d\n", ANALYZER_MAX_DIGIT_SIZE, *offset);

    // printf("storing truncated value of %s\n\n", lexeme);

  }

  insert_token(lexeme, numbersym);

  *offset = local_offset;

  // it's a number (e.g. "110082")
  return ANALYZER_VALID;

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
int is_terminal (char *file_text, int *offset, char token[], int token_type) {

  int local_offset = *offset;

  // find size of token we are checking against.
  int token_size = strlen(token);  

  char next; 

  for (int i = 0; i < token_size; i++) {

    next = *(file_text + local_offset);

    if (next == '\0' || next != token[i]) {

      // not valid, but no error yet
      return ANALYZER_NO_ERR;

    }

    local_offset++;

  } 

  // put in the token 
  insert_token(token, token_type);

  *offset = local_offset;

  return ANALYZER_VALID;

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

  char *status_str = analyzer_statuses[status];

}

void print_lexemes () {

}

Token * get_token_table() {

  return head;

} 
