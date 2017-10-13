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

#ifndef HEADER
#include "main.h"
#define HEADER
#endif

#define FALSE            0 
#define TRUE             1

#define OPTION_QUIT      0
#define OPTION_RUN       1
#define OPTION_PRINT     2 

#define MODE_NORMAL      0
#define MODE_INTERACTIVE 1

#define STATUS_OK        0
#define STATUS_ERR       1
#define STATUS_QUIT      2 
#define STATUS_FINISHED  3 

char *statuses[] = {
  "everything's cool",
  "there was an error",
  "program has quit",
  "program has finished",
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

#define INVALID_SYMBOL         -1

////////////////////////////////////////////////////////////////////////////////
char *token_buffer;
int buffer_index = 0;

// first symbol in the program
Symbol *head;

Symbol Symbol_new(char *lexeme, int token_type) {

  Symbol this;

  this.lexeme     = lexeme;

  this.token_type = token_type;

  this.next = NULL;

  return this;

}

int insert_symbol (char *lexeme, int token_type) {

  Symbol symbol = Symbol_new(lexeme, token_type);

  while (head->next != NULL) {

    head = head->next;

  }

  *head = symbol;

  print_symbol(symbol);

  return STATUS_OK;

}

void print_symbol (Symbol symbol) {

  printf("%s     %d\n", symbol.lexeme, symbol.token_type);

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
  int status = STATUS_OK;

  // don't forget to allocate an extra space for '\0'
  token_buffer = (char *)calloc(MAX_TOKEN_SIZE + 1, sizeof (char));

  // create an empty start symbol
  Symbol start_symbol;
  start_symbol.next = NULL;
  head = &start_symbol;

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

    status = STATUS_ERR;

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

  // character entered in by user
  char user_entry;

  scanf(" %c", &user_entry);
  printf("\n");

  // check if valid using a whitelist approach
  int valid =
    user_entry == 'q' || user_entry == 'h' || user_entry == 'r' || user_entry == 'i'; 

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

  int status = STATUS_OK;

  // print the prompt once on start up. print again
  // if the COMMAND_HELP is issued 
  print_prompt();

  // integer flag COMMAND_<command>
  int user_command;

  while (status == STATUS_OK) {

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

    else if (user_command == COMMAND_STEP) {


    }
    
    else if (user_command == COMMAND_RUN) {

      // call event loop as a subprocedure.
      status = event_loop(file);

      print_status(status);

    }

    // give a simple status (ok, quit, error, finished)
    else if (user_command == COMMAND_PRINT_STATUS) {

      print_status(status);

    }

  }
  
  return status;

}

int is_reserved_word (char *lexeme) {

  return
    strcmp("begin",     lexeme) == 0 ||
    strcmp("call" ,     lexeme) == 0 ||
    strcmp("const",     lexeme) == 0 ||
    strcmp("do",        lexeme) == 0 ||
    strcmp("else",      lexeme) == 0 ||
    strcmp("end",       lexeme) == 0 ||
    strcmp("if",        lexeme) == 0 ||
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

  int status = STATUS_OK;

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

  fclose(fp);

  // 'global' offset from the character pointer
  int  *offset = (int *)malloc(sizeof(int));

  *offset = 0;

  while(status != STATUS_ERR) { 
    
    // must have a block
    ((status = is_block(file_text, offset)) == STATUS_OK) ||
    (status = STATUS_ERR);

    if (status == STATUS_ERR)
      break;

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

  (status = is_constdec  (file_text, offset) != STATUS_ERR) ||
  (status = is_vardec    (file_text, offset) != STATUS_ERR) ||
  (status = is_procdec   (file_text, offset) != STATUS_ERR) ||
  (status = is_statement (file_text, offset) != STATUS_ERR) ||
  (status = STATUS_ERR);

  return status == STATUS_ERR? STATUS_OK: STATUS_ERR;

}

int is_constdec (char *file_text, int *offset) {

  TokenType token;

  token = is_constsym(file_text, offset);

  if (token == constsym) {

    // create Symbol struct
    //
    // put into symbol table
    // empty token buffer

  } 
  else {

    return STATUS_ERR;

  }

  return STATUS_OK;

}

int is_vardec (char *file_text, int *offset) {

  TokenType token;

  token = is_varsym(file_text, offset);

  if (token == varsym) {

    // create Symbol struct
    //
    // put into symbol table
    // empty token buffer

  }
  else {

    return STATUS_ERR; 

  }

  trim(file_text, offset);

  token = is_identsym(file_text, offset);

  if (token == identsym) {

    // create Symbol struct
     
    // put into symbol table
    // empty token buffer

  }
  else {

    return STATUS_ERR; 

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
      
      if (token == identsym) { 

        // create Symbol struct
           
        // put into symbol table
        // empty token buffer
        
        // trim
        trim (file_text, offset);

      }
      // comma must be followed by an identifier
      else {

        return INVALID_SYMBOL;

      }

    }
    // if there is no comma, we must find a semicolon
    else {

      // check if next symbol is a semicolon
      token = is_semicolonsym(file_text, offset);

      // if it is a semicolon, trim any whitespace
      if (token == semicolonsym) {
        // create Symbol struct
           
        // put into symbol table
        // empty token buffer
         
        // trim
        trim (file_text, offset);
       
      } 
      else {

        return STATUS_ERR;

      }

    }

    if (token == nulsym) {

      return STATUS_ERR;

    }

    // stop this if we reach a semicolon or something horrible
    halt = token == semicolonsym;

  } while (!halt);

  // trim just in case
  trim(file_text, offset);


  return STATUS_OK;

}


int is_identsym (char *file_text, int *offset) {

  int local_offset = *offset;

  // check first character to see if it is alpha 
  char next;

  next = *(file_text + local_offset);

  if (!is_letter(next)) {

    return INVALID_SYMBOL;

  }

  // since first symbol is a letter, this is a valid
  append_token_buffer(next); 
  
  // identifier (e.g. 'x'). the following code just checks for
  // additial letters and digits and moves the offset accordingly
  local_offset++;

  next = *(file_text + local_offset);

  // if the char is letter or digit, increase local offset
  while (is_letter(next) || is_digit(next)) {
     
    append_token_buffer(next);

    local_offset++;

    // fetch current char 
    next = *(file_text + local_offset);

  } 

  char *lexeme = get_token_buffer_value();

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

  return STATUS_ERR;

}

int is_statement (char *file_text, int *offset) {

  char *next;

  int local_offset = *offset;

  next = (file_text + local_offset);

  int status = STATUS_OK;

  // call <identifier>
  if (is_callstatement(file_text, offset)) {

    return STATUS_OK;

  }
  // begin <statement> {; <statement>} end
  else if (is_beginstatement(file_text, offset)) {

    return STATUS_OK;

  }
  // if <condition> then <statement> [else <statement>]
  else if (is_ifstatement(file_text, offset)) {

    return STATUS_OK;

  }
  // while <condition> do
  else if (is_whilestatement(file_text, offset)) {

    return STATUS_OK;

  }
  // read <identifier>
  else if (is_readstatement(file_text, offset)) {

    return STATUS_OK;

  }
  // write <identifier>
  else if (is_writestatement(file_text, offset)) {

    return STATUS_OK;

  }

  // check if it is an identifier. reserved words take precedence, so
  // other expressions listed above
  // <ident> := <expression>
  TokenType token;

  token = is_identsym(file_text, offset);

  if (token != identsym) {

    return STATUS_ERR;

  }

  trim(file_text, offset);

  token = is_becomessym(file_text, offset);

  if (token != becomessym) {

    return STATUS_ERR;

  }

  trim(file_text, offset);

  if (is_expression(file_text, offset)) {

    return STATUS_ERR;

  }

  return STATUS_ERR;

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

  if (!is_statement(file_text, offset)) {

    return FALSE;

  }

  trim(file_text, offset);

  token = is_semicolonsym(file_text, offset);

  trim(file_text, offset);

  while (token == semicolonsym) {

    if (!is_statement(file_text, offset)) {

      return FALSE;

    }
  
    token = is_semicolonsym(file_text, offset);

    trim(file_text, offset);

  }

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

  return FALSE;

}

int is_readstatement (char *file_text, int *offset) {

  return FALSE;

}

int is_writestatement (char *file_text, int *offset) {

  return FALSE;

}

int is_condition (char *file_text, int *offset) {

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

  if (is_lessym(file_text, offset)) {

    return TRUE;

  }
  else if (is_leqsym(file_text, offset)) {

    return TRUE;

  }
  else if (is_gtrsym(file_text, offset)) {

    return TRUE;

  }
  else if (is_geqsym(file_text, offset)) {

    return TRUE;

  }
  else if (is_eqsym(file_text, offset)) {

    return TRUE;

  }
  else if (is_neqsym(file_text, offset)) {

    return TRUE;

  }

  return FALSE;

}

int is_expression (char *file_text, int *offset) {

  return FALSE;
  
}

int is_term (char *file_text, int *offset) {

  return FALSE;

}

int is_factor (char *file_text, int *offset) {

  return FALSE;

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

    return INVALID_SYMBOL;

  }

  local_offset++;

  do {

    // get current character
    next = *(file_text + local_offset);

  // if it is a digit, increase the offset.
  }while(is_digit(next) && local_offset++);

  *offset = local_offset;

  printf("NERMBER\n");

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

/*
int is_oddsym (char *file_text, int *offset) {

  return is_terminal(file_text, offset, "odd", semicolonsym);

}
*/

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

  return is_terminal(file_text, offset, "(", rparentsym);

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

      return INVALID_SYMBOL;

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

int event_loop (FILE *file) {

  int status;

  status = is_program(file);

  return status;

}

void print_status (int status) {

  char *status_str = statuses[status];

  printf("STATUS: %s\n", status_str);

}

void print_lexemes () {

}
