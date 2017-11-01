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
// CVM_OP3402 assignment 1

//// Header Files ////
////////////////////////////////////////////////////////////////////////////////
#ifndef VM 
#include "vm.h"
#define VM 
#endif

char *vm_statuses[] = {
  "VM's cool",
  "there was a VM error",
  "VM has quit",
  "VM has finished",
};

// I use this approach so that if we want to support more lexical levels, the
// MAX_LEXI_LEVEL setting needs only to be changed above.
int lex_levels[VM_MAX_LEXI_LEVEL];

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

// simply check if instruction is VM_OP_SIO and has appropriate values
int Instruction_halt (Instruction this) {

  // check if instruction is halt
  return this.op == VM_OP_SIO && this.r == 0 && this.l == 0 && this.m == 3;

}

// create stack. initialize to 0
int stack_pointer = SP;
int stack[VM_MAX_STACK_HEIGHT] = {0};

// I am under the assumption that 'code length' refers to the number of
// instructions.
Instruction code[VM_MAX_CODE_LENGTH];

// initialize to 0
int program_counter = PC;

// create register file. initialize to 0
int reg[VM_REG_FILE_SIZE] = {0}; 

// keeps track of lexigraphical levels
int base_pointer = BP;


int vm_entry(FILE *fp) {

  // status starts off ok
  int status = VM_STATUS_OK;

  // for printing, first lexical level starts at 0 in the stack
  // all remaining lexical levels should be -1 to indicate 'not active'
  // when a sub-procedure is called, the base pointer will be indexed
  // under that level's key (again, for printing the trace)
  lex_levels[0] = BP - 1;

  for (int i = 1; i < VM_MAX_LEXI_LEVEL; i++) {

    lex_levels[i] = -1;

  }

  /*
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

    status = VM_STATUS_ERR;

    return status;

  }

  FILE *file = fopen(filename, "r");

  if (file == NULL) {

    printf("cannot find the file %s\n", filename);

    return 1;

  }
  */

  read_instructions_from_file(fp);

  status = vm_event_loop();

  fclose(fp);

  /*
  if (mode == MODE_INTERACTIVE) {

    status = interactive_loop();

  }
  else if (mode == MODE_NORMAL) {

    status = event_loop();

  }
  */

  return status;

}

/*
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
  commands['i'] = COMMAND_PRINT_VM_STATUS;

  // prints decimal instruction codes 
  commands['c'] = COMMAND_PRINT_CODE;

  // prints codes with string opcode names 
  commands['o'] = COMMAND_PRINT_VM_OP;

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
*/

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

    status = VM_STATUS_FINISHED;

    return status;

  }

  // or check if instruction is empty
  if (Instruction_empty(instruction)) {

    status = VM_STATUS_FINISHED;

    return status;

  }

  status = execute_instruction(instruction); 

  return status;

}

// called from interactive mode when 'r' or 't' command is given.
// also called when this executable is run without the 'i' flag
int vm_event_loop () {

  int status = VM_STATUS_OK;

  // stop program if quit, error, or finished.
  while (status == VM_STATUS_OK) {

    // run a single cycle of the program.  and return the new status
    status = fetch_execute(status);

  }

  // return end status.
  return status;

}

/*
// run the vm through interactive mode
int interactive_loop () {

  int status = VM_STATUS_OK;

  // print the prompt once on start up. print again
  // if the COMMAND_HELP is issued 
  print_prompt();

  // integer flag COMMAND_<command>
  int user_command;

  while (status == VM_STATUS_OK) {

    printf(">> ");

    user_command = get_user_command();

    if (user_command == COMMAND_INVALID) {

      printf("Please enter a valid command. use 'h' for help");
      // return to top of the loop
      continue;
      
    }

    else if (user_command == COMMAND_QUIT) {

      status = VM_STATUS_QUIT;

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

      // if there is a problem, print out the status
      if (status != VM_STATUS_OK) {

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
    else if (user_command == COMMAND_PRINT_VM_STATUS) {

      print_status(status);

    }

    // print raw decimal code values found in text file
    else if (user_command == COMMAND_PRINT_CODE) {

      print_code();

    }

    // print numbered code lines with stringified opcode names
    else if (user_command == COMMAND_PRINT_VM_OP) {

      print_op();

    }

    // print a full trace (runs program as side effect)
    else if (user_command == COMMAND_PRINT_TRACE) {

      status = print_trace(status);

      print_status(status);

    }

    // print full registers (not mentioned in rubric. used for
    // debugging/extra feature).
    else if (user_command == COMMAND_PRINT_REGISTERS) {

      print_registers();

    }

  }
  
  return status;

}

*/


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

  // increase the program counter. some operations will overwrite
  // this with the m value of the instruction
  program_counter++;
  
  // look at various op code possibilities
  
  // load constant value
  if (op == VM_OP_LIT) {

    // load constant value into register 
    reg[r] = m;

  }

  // return from sub-procedure
  else if (op == VM_OP_RTN) {

    // remove the current lexical level by setting
    // it to -1
    for (int i = VM_MAX_LEXI_LEVEL; i > 0; i--) {

      // lexical level is occupied
      if (lex_levels[i] != -1) {

        // mark it unoccupied
        lex_levels[i] = -1;
        break;

      }

      // only vacant levels so far, keep looking
      continue;

    }

    // set SP behind current activation record
    stack_pointer = base_pointer - 1;

    // set base pointer to previous activation record
    base_pointer = stack[stack_pointer + 3];

    // set PC to instruction after last VM_OP_CAL 
    program_counter = stack[stack_pointer + 4];

  }

  // load from offset m, l legigraphical levels down
  else if (op == VM_OP_LOD) {
    
    reg[r] = stack[get_base(l, base_pointer) + m];

  }

  // store value at offset m, l lexigraphical levels down.
  else if (op == VM_OP_STO) {

    stack[get_base(l, base_pointer) + m] = reg[r];

  }

  // call sub-procedure
  else if (op == VM_OP_CAL) {

    // check if we have room in the stack
    if (stack_pointer + 4 >= VM_MAX_STACK_HEIGHT) {

      // stack overflow
      return VM_STATUS_ERR;

    }

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

    // to keep track of printing later, we will check the lexical
    // level. find the first empty lexical level, and assign our base
    // pointer to that level
    for (int i = 0; i < VM_MAX_LEXI_LEVEL; i++) {

      // lexical level not accessed
      if (lex_levels[i] == -1) {

        lex_levels[i] = base_pointer;
        break;

      }
      else if ( i == VM_MAX_LEXI_LEVEL - 1) {

        // we are attempting to create a new lexical scope outside
        // of the virtual machine settings. return an error.
        return VM_STATUS_ERR;

      }

      // keep looking for available lexical level
      continue;

    }

  }

  // allocate m locals by incrementing stack pointer 
  else if (op == VM_OP_INC) {

    stack_pointer += m;

  }

  // change position of program counter to m 
  else if (op == VM_OP_JMP) {

    program_counter = m;

  }

  // jump conditional. jump if reg[r] = 0
  else if (op == VM_OP_JPC) {

    if (reg[r] == 0) {

      program_counter = m;

      // return to prevent PC from incrementing to m + 1
      return VM_STATUS_OK;

    }

  }

  // io operations
  else if (op == VM_OP_SIO) {

    // do multiple levels of decoding. check m
     
    // print value of a register file
    if (m == 1) {

      // please note that this will print before a stack trace.
      // this makes sense because the instruction must execute
      // before it can be traced
      printf(" VM_OP_SIO > REG[%d] = %d\n", r, reg[r]);

    }
    // allow user to put in a value for register
    else if (m == 2) {

      // get user input for register r
      printf("enter a value for register %d", r);
      int value;
      scanf(" %d", &value);
      reg[r] = value;

    }
    // exit program
    else if (m == 3) {

      // reset pointers and PC 
      program_counter = 0;
      base_pointer    = 0;
      stack_pointer   = 0;

      return VM_STATUS_FINISHED;

    } 

  }

  // negation
  else if (op == VM_OP_NEG) {

    reg[r] = -(reg[l]);

  }

  // addition
  else if (op == VM_OP_ADD) {

    reg[r] = reg[l] + reg[m];

  }

  // subtraction
  else if (op == VM_OP_SUB) {

    reg[r] = reg[l] - reg[m];

  }

  // multiplication
  else if (op == VM_OP_MUL) {

    reg[r] = reg[l] * reg[m];

  }

  // division
  else if (op == VM_OP_DIV) {

    reg[r] = reg[l] / reg[m];

  }

  // check if odd
  else if (op == VM_OP_ODD) {

    reg[r] = reg[l] % 2;

  }

  // modulo
  else if (op == VM_OP_MOD) {

    reg[r] = reg[l] % reg[m];

  }

  // check equality
  else if (op == VM_OP_EQL) {

    reg[r] = reg[l] == reg[m];

  }

  // check non-equality
  else if (op == VM_OP_NEQ) {

    reg[r] = reg[l] != reg[m];

  }

  // less than
  else if (op == VM_OP_LSS) {

    reg[r] = reg[l] < reg[m];

  }

  // less than or equal to
  else if (op == VM_OP_LEQ) {

    reg[r] = reg[l] <= reg[m];

  }

  // greater than 
  else if (op == VM_OP_GTR) {

    reg[r] = reg[l] > reg[m];

  }

  // greater than or equal to
  else if (op == VM_OP_GEQ) {

    reg[r] = reg[l] >= reg[m];

  }

  // problem with instruction. not in ISA.
  else {

    return VM_STATUS_ERR;

  }

  return VM_STATUS_OK;

}

/*
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

    printf("%d %d %d %d", op, r, l, m );

    if (i == program_counter) {

      printf("  <-- you are here\n");

    }
    else {

      printf("\n");

    }

  }

}
*/

/*
void print_op () {

  printf("======================VM_OP-CODES====================\n");
   
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

    str_op = op < 23 && op > 0 ? opcodes[op]: opcodes[VM_OP_NUL];

    // print out instruction with string name
    printf("%d %s %d %d %d", i, str_op, r, l, m );

    if (i == program_counter) {

      printf("  <-- you are here\n");

    }
    else {

      printf("\n");

    }



  }
  
}
*/

/*
// runs the program and prints the stack trace
int print_trace (int status) {

  // extra labels were added in addtion to the required ones per the rubric. 
  printf("ins  op   r   l   m      pc  bp  sp   stack values\n");

  while (status == VM_STATUS_OK) {

    // locations of pipes based on lexical levels. initialize to 0
    int *pipes = (int *)calloc(stack_pointer + 1, sizeof(int));

    // set the locations of pipes
    for (int i = 0; i < MAX_LEXI_LEVEL; i++) {

      if (lex_levels[i] == -1) {

        // unaccessed lexical levels. return early 
        break;

      }

      // start of lexical level in stack.
      int lex_start = lex_levels[i];

      // assert index i to say 'print a pipe here' 
      *(pipes + lex_start) = 1; 

    }

    // fetching the instruction here for printing
    Instruction instruction = fetch_instruction();

    // save the pc before it is modified, for printing.
    int initial_pc = program_counter; 

    status = fetch_execute(status);

    int op, r, l, m;

    op = instruction.op;
    r  = instruction.r;
    l  = instruction.l;
    m  = instruction.m;

    // string name of opcode
    char *str_op = opcodes[op];

    // print operation, as well as PC, BP, and SP 
    printf("%3d %s  %2d  %2d  %2d      %2d  %2d  %2d  ",
        initial_pc, str_op, r, l, m, program_counter, base_pointer, stack_pointer);

    // print out the stack values
    for (int i = 0; i <= stack_pointer; i++) {

      // check if this index of stack is the start of a new lexical scope
      // but don't bother to print a pipe for the first scope 
      if (*(pipes + i) == 1 && i != 0) {

        printf("| ");

      }
    
      // print value at index of stack
      printf("%2d ", stack[i]);
      
    }

    printf("\n");

    // no longer need the pipes at each lexical level for
    // this instruction, may change by next instruction
    free(pipes);

  }
  
  return status;

}
*/

/*
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
*/
/*
void print_status (int status) {

  char *status_str = statuses[status];

  printf("VM_STATUS: %s\n", status_str);

}
*/

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
