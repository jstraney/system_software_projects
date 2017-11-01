#ifndef STDH 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define STDH 
#endif

//// Initial Pointer Values ////
////////////////////////////////////////////////////////////////////////////////
// these are just the starting values, actual stack pointer, base pointer etc.
// are defined lower in code.
#define SP 0 
#define BP 1 
#define PC 0 
#define IR 0 

//// Constants ////
////////////////////////////////////////////////////////////////////////////////
#define VM_MAX_STACK_HEIGHT        2000
#define VM_MAX_CODE_LENGTH          500
#define VM_REG_FILE_SIZE              8 
#define VM_MAX_LEXI_LEVEL             3
#define VM_INSTRUCTION_REGISTER_SIZE 16

// lexical levels. used in printing trace, mainly.
// each level is initialized to -1, until a call is made to that lexigraphical
// level. at that point, the index (lexical level) is set to the current
// base pointer. this is really used for printing, mainly.
//

//// VM Statuses ////
////////////////////////////////////////////////////////////////////////////////
#define VM_STATUS_OK        0
#define VM_STATUS_ERR       1
#define VM_STATUS_QUIT      2 
#define VM_STATUS_FINISHED  3 


/*
//// Interactive Mode User Commands ////
////////////////////////////////////////////////////////////////////////////////
#define COMMAND_INVALID        -1
#define COMMAND_QUIT            0 
#define COMMAND_HELP            1 
#define COMMAND_STEP            2 
#define COMMAND_RUN             3 
#define COMMAND_PRINT_VM_STATUS    4 
#define COMMAND_PRINT_CODE      5 
#define COMMAND_PRINT_VM_OP        6 
#define COMMAND_PRINT_TRACE     7 
#define COMMAND_PRINT_REGISTERS 8 
*/

//// VM_OP Codes ////
////////////////////////////////////////////////////////////////////////////////
#define VM_OP_NUL  0
#define VM_OP_LIT  1
#define VM_OP_RTN  2
#define VM_OP_LOD  3
#define VM_OP_STO  4
#define VM_OP_CAL  5
#define VM_OP_INC  6
#define VM_OP_JMP  7
#define VM_OP_JPC  8
#define VM_OP_SIO  9
#define VM_OP_NEG 10
#define VM_OP_ADD 11
#define VM_OP_SUB 12
#define VM_OP_MUL 13
#define VM_OP_DIV 14
#define VM_OP_ODD 15
#define VM_OP_MOD 16
#define VM_OP_EQL 17
#define VM_OP_NEQ 18
#define VM_OP_LSS 19
#define VM_OP_LEQ 20
#define VM_OP_GTR 21
#define VM_OP_GEQ 22



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

/*
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

*/
/*
// fetches character from input, returns integer flag
int get_user_command();
*/
int vm_entry(FILE *fp);

// calls vm proceedures
int vm_event_loop();

/*
// calls vm procedures in an interactive context
int interactive_loop();

*/
int get_base(int l, int base);

// read instructions from file and load into code register
int read_instructions_from_file(FILE *file);

// first stage of vm
Instruction fetch_instruction();

// branches into various instructions
int execute_instruction(Instruction instruction);
