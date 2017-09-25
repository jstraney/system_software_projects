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

//// Includes ////
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>

//// Header Files ////
////////////////////////////////////////////////////////////////////////////////
#ifndef MAIN_FUNCTIONS
#include "main.h"
#define MAIN_FUNCTIONS
#endif

//// Constants ////
////////////////////////////////////////////////////////////////////////////////
#define MAX_STACK_HEIGHT  2000
#define MAX_CODE_LENGTH  500
#define REG_FILE_SIZE 16 
#define MAX_LEXI_LEVEL  3
#define INSTRUCTION_REGISTER_SIZE 16

// initial pointer values
#define SP 0 
#define BP 1 
#define PC 0 
#define IR 0 

//// VM Statuses ////
////////////////////////////////////////////////////////////////////////////////
#define STATUS_OK  0
#define STATUS_ERR  1
#define STATUS_QUIT  2 
#define STATUS_FINISHED  2 

//// User Commands ////
////////////////////////////////////////////////////////////////////////////////
#define COMMAND_INVALID  -1
#define COMMAND_QUIT  0 
#define COMMAND_HELP  1 
#define COMMAND_STEP  2 
#define COMMAND_RUN  3 

//// OP Codes ////
////////////////////////////////////////////////////////////////////////////////
#define OP_LIT 1
#define OP_RTN 2
#define OP_LOD 3
#define OP_STO 4
#define OP_CAL 5
#define OP_INC 6
#define OP_JMP 7
#define OP_JPC 8
#define OP_SIO 9
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

//// Program Structures ////
////////////////////////////////////////////////////////////////////////////////
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
int base_pointer[MAX_LEXI_LEVEL];

int main (int argsc, char *argv[]) {

  // running interactive mode. allow user commands
  if (argsc == 2 && strcmp(argv[1], "-i") == 0) {

    interactive_loop();

  }
  else if (argsc == 2) {

    // file containing instructions
    char *filename = argv[1];

    event_loop(filename);

  }

  return 0;

}

// print table on program start in interactive mode
int print_prompt () {

  // print out the prompt
  printf("==================================================\n");
  printf("= P-Machine Interactive Mode                     =\n");
  printf("==================================================\n");
  printf("= q - quit the virtual machine          ==========\n");
  printf("= h - call for help                     ==========\n");
  printf("= s - step through each cycle           ==========\n");
  printf("= r - run program from start to finsish ==========\n");
  printf("==================================================\n\n");

  return 0;

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

  // character entered in by user
  char user_entry;

  scanf("%c", &user_entry);
  printf("\n");

  // check if valid using a whitelist approach
  int valid =
    user_entry == 'q' || user_entry == 'h' || user_entry == 's' ||
    user_entry == 'r';

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

// called when no arguments are provided to the binary
int event_loop (char *filename) {

  FILE *file = fopen(filename, "r");

  read_instructions_from_file(file);

  fclose(file);

  // status okay
  int status = STATUS_OK;

  while (status == STATUS_OK) {

    // load instruction from 'code'
    Instruction instruction = fetch_instruction();

    execute_instruction(instruction); 

  }

  return STATUS_OK;

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

    }
    
    else if (user_command == COMMAND_RUN) {

    }

  }
  
  return 0;

}

// fetches the instructions into 'code' register
int read_instructions_from_file(FILE *file) {

  // locally used for inserting codes into the code register. 
  int code_index = 0;

  while (!feof(file)) {

    // allocate memory for instruction
    Instruction *instruction = malloc(sizeof(Instruction));

    // go through each part of the instruction on this line
    for (int i = 0; i < 4; i++) {

      int instruction_part;

      // get the next integer from the file
      fscanf(file, "%d", &instruction_part);

      // opcode, should be first of every 4 values
      if (i % 4 == 0) {

        instruction->op = instruction_part;

      }
      // register should be second
      else if (i % 4 == 1) {

        instruction->r = instruction_part;

      }
      // l should be third
      else if (i % 4 == 2) {

        instruction->l = instruction_part;

      }
      // m should be fourth
      else if (i % 4 == 3) {

        instruction->m = instruction_part;

      }
      
    }

    // place the instruction in our code register
    code[code_index++] = *instruction;

    // free allocated memory for instruction
    free(instruction);

  }

  // everything okay so far
  return 1;

}

Instruction fetch_instruction () {

  Instruction instruction;

  instruction = code[PC];

  return instruction;

}

int execute_instruction (Instruction instruction) {

  int op = instruction.op;  
  int r  = instruction.r;
  int l  = instruction.l;
  int m  = instruction.m;

  if (op == OP_LIT) {

    // load constant value into register 
    reg[r] = m;

  }

  else if (op == OP_RTN) {

    // return from subroutine

  }

  else if (op == OP_LOD) {

    // nope 

  }

  else if (op == OP_STO) {

    // no idea

  }

  else if (op == OP_CAL) {

    // fetch procedure at code index m 
    Instruction subprocedure = code[m]; 
    
    // create new activation record 
    program_counter = m;

  }

  else if (op == OP_INC) {

    // allocate m locals
    // functional value
    stack[m - m] = 0;

    // functional value
    stack[m - m + 1] = 0;

    // functional value
    stack[m - m + 2] = 0;

    // functional value
    stack[m - m + 4] = 0;

    // leaves m - 4 spaces for local procedure data
    stack_pointer += m;

  }

  else if (op == OP_JMP) {

    program_counter = m;

  }

  else if (op == OP_JPC) {

    if (reg[r] == 0) {

      program_counter = m;

    }

  }

  else if (op == OP_SIO) {

    // do multiple levels of decoding. check r, l and m

  }

  else if (op == OP_NEG) {

    reg[r] = -(reg[l]);

  }

  else if (op == OP_ADD) {

    reg[r] = reg[l] + reg[m];

  }

  else if (op == OP_SUB) {

    reg[r] = reg[l] - reg[m];

  }

  else if (op == OP_MUL) {

    reg[r] = reg[l] * reg[m];

  }

  else if (op == OP_DIV) {

    reg[r] = reg[l] / reg[m];

  }

  else if (op == OP_ODD) {

    reg[r] = reg[l] % 2;

  }

  else if (op == OP_MOD) {

    reg[r] = reg[l] % reg[m];

  }

  else if (op == OP_EQL) {

    reg[r] = reg[l] == reg[m];

  }

  else if (op == OP_NEQ) {

    reg[r] = reg[l] != reg[m];

  }

  else if (op == OP_LSS) {

    reg[r] = reg[l] < reg[m];

  }

  else if (op == OP_LEQ) {

    reg[r] = reg[l] <= reg[m];

  }

  else if (op == OP_GTR) {

    reg[r] = reg[l] > reg[m];

  }

  else if (op == OP_GEQ) {

    reg[r] = reg[l] >= reg[m];

  }

  else {

    return STATUS_ERR;

  }

  return STATUS_OK;

}
