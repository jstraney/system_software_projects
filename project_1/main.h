#ifndef IO
#include <stdio.h>
#endif

//// Structs ////
typedef struct Instruction {
  int op;
  int r;
  int l;
  int m;
} Instruction;

typedef struct ActivationRecord {
  int parameters;
  int locals;
  int return_address;   
  int static_link;   
  int dynamic_link;   
  int functional_value;   
} ActivationRecord;

// prompt for interactive mode
int print_prompt();

// fetches character from input, returns integer flag
int get_user_command();

// opens file with instruction code
int open_file();

// calls vm proceedures
int event_loop();

// calls vm procedures in an interactive context
int interactive_loop();

// read instructions from file and load into code register
int read_instructions_from_file(FILE *file);

// first stage of vm
Instruction fetch_instruction();

// branches into various instructions
int execute_instruction(Instruction instruction);
