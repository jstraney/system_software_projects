/*
______      ___  ___  ___  _____  _   _ _____ _   _  _____ 
| ___ \     |  \/  | / _ \/  __ \| | | |_   _| \ | ||  ___|
| |_/ /_____| .  . |/ /_\ \ /  \/| |_| | | | |  \| || |__  
|  __/______| |\/| ||  _  | |    |  _  | | | | . ` ||  __| 
| |         | |  | || | | | \__/\| | | |_| |_| |\  || |___ 
\_|         \_|  |_/\_| |_/\____/\_| |_/\___/\_| \_/\____/ 

*/
// Written by Jeffrey Straney and Frank Volk 
// 09-22-2017
// COP3402 assignment 1

//// Header Files ////
////////////////////////////////////////////////////////////////////////////////
#ifndef MAIN_FUNCTIONS
#include "main.h"
#define MAIN_FUNCTIONS
#endif

//// Initial Pointer Values ////
// these are just the starting values, actual stack pointer, base pointer etc.
// are defined lower in code.
#define SP 0 
#define BP 1 
#define PC 0 
#define IR 0 

//// Constants ////
////////////////////////////////////////////////////////////////////////////////
#define MAX_STACK_HEIGHT        2000
#define MAX_CODE_LENGTH          500
#define REG_FILE_SIZE             16 
#define MAX_LEXI_LEVEL             3
#define INSTRUCTION_REGISTER_SIZE 16

// lexigraphical levels. used in printing trace, mainly.
// each level is initialized to -1, until a call is made to that lexigraphical
// level. at that point, the index (lexical level) is set to the current
// base pointer.
int lex_level;
int lex_levels[MAX_LEXI_LEVEL] {-1};
lex_levels[0] = BP;

//// VM Modes ////
////////////////////////////////////////////////////////////////////////////////
#define MODE_INTERACTIVE 0
#define MODE_NORMAL      1 

//// VM Statuses ////
////////////////////////////////////////////////////////////////////////////////
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

//// Interactive Mode User Commands ////
////////////////////////////////////////////////////////////////////////////////
#define COMMAND_INVALID        -1
#define COMMAND_QUIT            0 
#define COMMAND_HELP            1 
#define COMMAND_STEP            2 
#define COMMAND_RUN             3 
#define COMMAND_PRINT_STATUS    4 
#define COMMAND_PRINT_CODE      5 
#define COMMAND_PRINT_OP        6 
#define COMMAND_PRINT_TRACE     7 
#define COMMAND_PRINT_REGISTERS 8 

//// OP Codes ////
////////////////////////////////////////////////////////////////////////////////
#define OP_NUL  0
#define OP_LIT  1
#define OP_RTN  2
#define OP_LOD  3
#define OP_STO  4
#define OP_CAL  5
#define OP_INC  6
#define OP_JMP  7
#define OP_JPC  8
#define OP_SIO  9
#define OP_NEG 10
#define OP_ADD 11
#define OP_SUB 12
#define OP_MUL 13
#define OP_DIV 14
#define OP_ODD 15
#define OP_MOD 16
#define OP_EQL 17
#define OP_NEQ 18
#define OP_LSS 19
#define OP_LEQ 20
#define OP_GTR 21
#define OP_GEQ 22

char *opcodes[] = {
  // arrays start from 0, ISA starts from 1. "???" 
  // added to position 0 in name look-up array for
  // unreadable commands. Note that this is not the
  // actual instruction set, but a lookup for human
  // readable code names.
  "???",
  "lit",
  "rtn",
  "lod",
  "sto",
  "cal",
  "inc",
  "jmp",
  "jpc",
  "sio",
  "neg",
  "add",
  "sub",
  "mul",
  "div",
  "odd",
  "mod",
  "eql",
  "neq",
  "lss",
  "leq",
  "gtr",
  "geq"
};

//// Program Structures ////
////////////////////////////////////////////////////////////////////////////////

// constructor for instruction
Instruction Instruction_new (int op, int r, int l, int m) {

  Instruction this;

  this.op    = op;
  this.r     = r;
  this.l     = l;
  this.m     = m;

  // ^this is that
  return this;

}

// determines if an instance of Instruction is empty
int Instruction_empty (Instruction this) {

  return (this.op | this.r | this.l | this.m) == 0;

}

// simply check if instruction is OP_SIO and has appropriate values
int Instruction_halt (Instruction this) {

  // check if instruction is halt
  return this.op == OP_SIO && this.r == 0 && this.l == 0 && this.m == 3;

}

// create stack. initialize to 0
int stack_pointer = SP;
int stack[MAX_STACK_HEIGHT] = {0};

// I am under the assumption that 'code length' refers to the number of
// instructions
Instruction code[MAX_CODE_LENGTH];

// initialize to 0
int program_counter = PC;

// create register file. initialize to 0
int reg[REG_FILE_SIZE] = {0}; 

// keeps track of lexigraphical levels
int base_pointer = BP;

int main (int argsc, char *argv[]) {

  // status starts off ok
  int status = STATUS_OK;

  // interactive or normal mode
  int mode;

  // name of file from argv
  char *filename;

  // running interactive mode. allow user commands
  if (argsc == 3 && strcmp(argv[1], "-i") == 0) {

    mode = MODE_INTERACTIVE;
    // file containing instructions
    filename = argv[2];

  }
  else if (argsc == 2) {

    mode = MODE_NORMAL;

    // file containing instructions
    filename = argv[1];

  }
  else {

    printf("the p-machine requires a filename, or the -i flag followed by a file name.\n");

    status = STATUS_ERR;

    return status;

  }

  FILE *file = fopen(filename, "r");

  read_instructions_from_file(file);

  fclose(file);

  if (mode == MODE_INTERACTIVE) {

    status = interactive_loop();

  }
  else if (mode == MODE_NORMAL) {

    status = event_loop();

  }

  return status;

}

// print table on program start in interactive mode
void print_prompt () {

  // print out the prompt
  printf("==================================================\n");
  printf("=           P-Machine Interactive Mode           =\n");
  printf("==================================================\n");
  printf("= q - quit the virtual machine          ==========\n");
  printf("= h - call for help                     ==========\n");
  printf("= s - step through each cycle           ==========\n");
  printf("= r - run program from start to finsish ==========\n");
  printf("= i - print status code                 ==========\n");
  printf("= c - print raw machine code from file  ==========\n");
  printf("= o - print code with opcode names      ==========\n");
  printf("= t - print full stack trace            ==========\n");
  printf("= p - print all registers               ==========\n");
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

  // step to next instruction
  commands['s'] = COMMAND_STEP;

  // run all instruction from start to finish
  commands['r'] = COMMAND_RUN;

  // prints status code when stepping through 
  commands['i'] = COMMAND_PRINT_STATUS;

  // prints decimal instruction codes 
  commands['c'] = COMMAND_PRINT_CODE;

  // prints codes with string opcode names 
  commands['o'] = COMMAND_PRINT_OP;

  // prints full trace of the program output 
  commands['t'] = COMMAND_PRINT_TRACE;

  // print registers 
  commands['p'] = COMMAND_PRINT_REGISTERS;

  // character entered in by user
  char user_entry;

  scanf(" %c", &user_entry);
  printf("\n");

  // check if valid using a whitelist approach
  int valid =
    user_entry == 'q' || user_entry == 'h' || user_entry == 's' ||
    user_entry == 'r' || user_entry == 'i' || user_entry == 'c' || 
    user_entry == 'o' || user_entry == 't' || user_entry == 'p';

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

// inner function of the event loop. separated to allow its
// use as a sub-procedure (for print_trace, interactive_loop, event_loop).
// in summary, fetches a single instruction and executes it
int fetch_execute (int status) {

  // load instruction from 'code'
  Instruction instruction = fetch_instruction();

  int op, r, l, m;

  op = instruction.op;
  r  =  instruction.r;
  l  =  instruction.l;
  m  =  instruction.m;

  // check if instruction triggers halt
  if (Instruction_halt(instruction)) {

    status = STATUS_FINISHED;

    return status;

  }

  // or check if instruction is empty
  if (Instruction_empty(instruction)) {

    status = STATUS_FINISHED;

    return status;

  }

  status = execute_instruction(instruction); 

  if (status == STATUS_ERR) {

    return status;

  }

  return status;

}

// called when no arguments are provided to the binary. is
// also used from interactive mode when the 'run' or 'trace' command is used
int event_loop () {

  int status = STATUS_OK;

  // stop program if quit, error, or finished.
  while (status == STATUS_OK) {

    // run a single cycle of the program.  and return the new status
    status = fetch_execute(status);

  }

  // return end status.
  return status;

}

// run the vm through interactive mode
int interactive_loop () {

  int status = STATUS_OK;

  // print the prompt once on start up. print again
  // if the COMMAND_HELP is issued 
  print_prompt();

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

      printf("Bye!\n");

      break;

    }

    else if (user_command == COMMAND_HELP) {

      // simply print the prompt again
      print_prompt();

      continue;

    }

    else if (user_command == COMMAND_STEP) {

      // run a single cycle of the program
      status = fetch_execute(status);

      if (status != STATUS_OK) {

        print_status(status); 

      }
      else {

        // let the user know we stepped.
        printf("step from pc %d -> %d\n", program_counter - 1, program_counter);

      }

    }
    
    else if (user_command == COMMAND_RUN) {

      // call event loop as a subprocedure.
      status = event_loop();

      print_status(status);

    }

    // give a simple status (ok, quit, error, finished)
    else if (user_command == COMMAND_PRINT_STATUS) {

      print_status(status);

    }

    // print raw decimal code values found in text file
    else if (user_command == COMMAND_PRINT_CODE) {

      print_code();

    }

    // print numbered code lines with stringified opcode names
    else if (user_command == COMMAND_PRINT_OP) {

      print_op();

    }

    // print a full trace (runs program as side effect)
    else if (user_command == COMMAND_PRINT_TRACE) {

      print_trace(status);

    }

    // print full registers (not mentioned in rubric. used for
    // debugging/extra feature).
    else if (user_command == COMMAND_PRINT_REGISTERS) {

      print_registers();

    }

  }
  
  return status;

}

// fetches the instructions into 'code'
int read_instructions_from_file(FILE *file) {

  // locally used for inserting codes into the code register. 
  int code_index = 0;

  while (!feof(file)) {

    // instruction instance
    Instruction instruction;

    // members of instruction
    int op, r, l, m;

    // go through each part of the instruction on this line
    for (int i = 0; i < 4; i++) {

      // the member of the instruction struct
      int instruction_part;

      // get the next integer from the file
      int count = fscanf(file, "%d", &instruction_part);

      // may have not hit EOF to trigger feof. if so, check value
      // returned from fscanf
      if (count == EOF) {

        // end of input
        return 1;

      }

      // opcode, should be first of every 4 values
      if (i == 0) {

        op = instruction_part;

      }
      // register should be second
      else if (i == 1) {

        r = instruction_part;

      }
      // l should be third
      else if (i == 2) {

        l = instruction_part;

      }
      // m should be fourth
      else if (i == 3) {

        m = instruction_part;

      }
      
    }

    // call the instruction factory function
    instruction = Instruction_new(op, r, l, m);

    // place the instruction in our code register
    code[code_index++] = instruction;

  }

  // everything okay so far
  return 1;

}

// returns the next instruction from memory using the program counter
Instruction fetch_instruction () {

  Instruction instruction;

  // fetch instruction using the program counter
  instruction = code[program_counter];

  return instruction;

}

// decodes instruction
int execute_instruction (Instruction instruction) {

  int op = instruction.op;  
  int r  = instruction.r;
  int l  = instruction.l;
  int m  = instruction.m;

  // look at various op code possibilities
  
  // load constant value
  if (op == OP_LIT) {

    // load constant value into register 
    reg[r] = m;

  }

  // return from sub-procedure
  else if (op == OP_RTN) {

    // set SP behind current activation record
    stack_pointer = base_pointer - 1;

    // set base pointer to previous activation record
    base_pointer = stack[stack_pointer + 3];

    // set PC to instruction after last OP_CAL 
    program_counter = stack[stack_pointer + 4];

  }

  // load from offset m, l legigraphical levels down
  else if (op == OP_LOD) {
    
    reg[r] = stack[get_base(l, base_pointer) + m];

  }

  // store value at offset m, l lexigraphical levels down.
  else if (op == OP_STO) {

    stack[get_base(l, base_pointer) + m] = reg[r];

  }

  // call sub-procedure
  else if (op == OP_CAL) {

    // space to return value
    stack[stack_pointer + 1] = 0;

    // static link
    stack[stack_pointer + 2] = get_base(l, base_pointer);

    // dynamic link
    stack[stack_pointer + 3] = base_pointer; 

    // return address
    stack[stack_pointer + 4] = program_counter;

    // set the base pointer below stack pointer
    base_pointer = stack_pointer + 1;
    
    // create new activation record 
    program_counter = m;

    // return to prevent PC from incrementing to m + 1
    return STATUS_OK;

  }

  // allocate m locals by incrementing stack pointer 
  else if (op == OP_INC) {

    stack_pointer += m;

  }

  // change position of program counter to m 
  else if (op == OP_JMP) {

    program_counter = m;

    // return to prevent PC from incrementing to m + 1
    return STATUS_OK;

  }

  // jump conditional. jump if reg[r] = 0
  else if (op == OP_JPC) {

    if (reg[r] == 0) {

      program_counter = m;

      // return to prevent PC from incrementing to m + 1
      return STATUS_OK;

    }

  }

  // io operations
  else if (op == OP_SIO) {

    // do multiple levels of decoding. check m
     
    // print value of a register file
    if (m == 1) {

      printf("R[%d] = %d\n", r, reg[r]);

    }
    // allow user to put in a value for register
    else if (m == 2) {

      int value;
      // get user input
      printf("enter a value for register %d", r);
      scanf(" %d", &value);
      reg[r] = value;

    }
    // exit program
    else if (m == 3) {

      // reset pointers and PC 
      program_counter = 0;
      base_pointer    = 0;
      stack_pointer   = 0;

      return STATUS_FINISHED;

    } 

  }

  // negation
  else if (op == OP_NEG) {

    reg[r] = -(reg[l]);

  }

  // addition
  else if (op == OP_ADD) {

    reg[r] = reg[l] + reg[m];

  }

  // subtraction
  else if (op == OP_SUB) {

    reg[r] = reg[l] - reg[m];

  }

  // multiplication
  else if (op == OP_MUL) {

    reg[r] = reg[l] * reg[m];

  }

  // division
  else if (op == OP_DIV) {

    reg[r] = reg[l] / reg[m];

  }

  // check if odd
  else if (op == OP_ODD) {

    reg[r] = reg[l] % 2;

  }

  // modulo
  else if (op == OP_MOD) {

    reg[r] = reg[l] % reg[m];

  }

  // check equality
  else if (op == OP_EQL) {

    reg[r] = reg[l] == reg[m];

  }

  // check non-equality
  else if (op == OP_NEQ) {

    reg[r] = reg[l] != reg[m];

  }

  // less than
  else if (op == OP_LSS) {

    reg[r] = reg[l] < reg[m];

  }

  // less than or equal to
  else if (op == OP_LEQ) {

    reg[r] = reg[l] <= reg[m];

  }

  // greater than 
  else if (op == OP_GTR) {

    reg[r] = reg[l] > reg[m];

  }

  // greater than or equal to
  else if (op == OP_GEQ) {

    reg[r] = reg[l] >= reg[m];

  }

  // problem with instruction. not in ISA.
  else {

    return STATUS_ERR;

  }

  program_counter++;

  return STATUS_OK;

}

// can be run through interactive mode
void print_code () {

  printf("===================FILE-CODE======================\n");
   
  // length of code register 
  int code_len = sizeof(code) / sizeof(code[0]);

  for (int i = 0; i < code_len; i++) {

    Instruction instruction;

    instruction = code[i];

    // check if all members 
    if (Instruction_empty(instruction)) {

      printf("========================FIN=======================\n");
      break;

    }

    int op, r, l, m;

    op = instruction.op;
    r  = instruction.r;
    l  = instruction.l;
    m  = instruction.m;

    printf("%d %d %d %d\n", op, r, l, m );

  }

}

void print_op () {

  printf("======================OP-CODES====================\n");
   
  // length of code register 
  int code_len = sizeof(code) / sizeof(code[0]);

  for (int i = 0; i < code_len; i++) {

    Instruction instruction;

    instruction = code[i];

    if (Instruction_empty(instruction)) {

      printf("========================FIN=======================\n");
      break;

    }

    int op, r, l, m;

    op = instruction.op;
    r  = instruction.r;
    l  = instruction.l;
    m  = instruction.m;

    // get stringified opcode name
    char *str_op;

    str_op = op < 23 && op > 0 ? opcodes[op]: opcodes[OP_NUL];

    // print out instruction with string name
    printf("%d %s %d %d %d\n", i, str_op, r, l, m );

  }
  
}

// runs the program and prints the stack trace
int print_trace (int status) {

  printf("initial values           pc  bp  sp  \n");

  while (status == STATUS_OK) {

    // fetching the instruction here for printing
    Instruction instruction = fetch_instruction();

    // has a side effect of increasing the programm counter,
    // which is why the instruction is fetched here, locally as well
    status = fetch_execute(status);

    int op, r, l, m;

    op = instruction.op;
    r  = instruction.r;
    l  = instruction.l;
    m  = instruction.m;

    char *str_op = opcodes[op];

    // print operation, as well as PC, BP, and SP 
    printf("    %s  %2d  %2d  %2d      %2d  %2d  %2d  ",
        str_op, r, l, m, program_counter, base_pointer, stack_pointer);

    // print out the stack values up to the current lexigraphical level

  }
  
  return status;

}

// prints the full registers at any given time
void print_registers() {

  int stack_length, reg_length;

  stack_length = sizeof(stack) / sizeof(stack[0]);

  reg_length   = sizeof(reg) / sizeof(reg[0]);

  printf("==================REGISTERS=======================\n");

  printf("SP = %d | PC = %d | BP = %d \t\t\t\n", stack_pointer, program_counter, base_pointer);

  for (int i = 0; i < stack_length; i++) {

    printf("SP[%4d] = %6d | ", i, stack[i]);

    if (i % 4 == 3)
      printf("\n");

  }

  for (int j = 0; j < reg_length; j++) {

    printf("R[%2d] = %6d | ", j, reg[j]);

    if (j % 4 == 3)
      printf("\n");

  }

  printf("=====================FIN==========================\n");

}

void print_status (int status) {

  char *status_str = statuses[status];

  printf("STATUS: %s\n", status_str);

}

// per assignments recommendation, here is a helper function
// to descend L lexigraphical levels.
int get_base(int l, int base) {

  int b = base;

  while (l > 0) {

    b = stack[base + l];

    l--;

  }

  return b;

}
