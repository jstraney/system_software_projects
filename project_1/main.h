#ifndef IOH
#include <stdio.h>
#define IOH
#endif

#ifndef LIBH
#include <stdlib.h>
#define LIBH
#endif

#ifndef STRH
#include <string.h>
#define STRH
#endif

//// Structs ////
typedef struct Instruction {
  int op;
  int r;
  int l;
  int m;
} Instruction;

// constructor for Instruction
Instruction Instruction_new (int op, int r, int l, int m); 

int Instruction_empty (Instruction this);

int Instruction_halt (Instruction this);

// prints status of the VM (ok, error, quit, finished)
void print_status(int status);

// prompt for interactive mode
void print_prompt();

// prints decimal values of machine code 
void print_code();

// prints code with stringified op names 
void print_op();

// prints full trace. returns status, as the
// program is run as a side-effect
int print_trace();

// print SP, BP, PC, R
void print_registers();

// fetches character from input, returns integer flag
int get_user_command();

// opens file with instruction code
int open_file();

// calls vm proceedures
int event_loop();

// calls vm procedures in an interactive context
int interactive_loop();

int get_base(int l, int base);

// read instructions from file and load into code register
int read_instructions_from_file(FILE *file);

// first stage of vm
Instruction fetch_instruction();

// branches into various instructions
int execute_instruction(Instruction instruction);
