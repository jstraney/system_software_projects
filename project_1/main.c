#include <stdio.h>
#include <string.h>

#ifndef MAIN_FUNCTIONS
#include "main.h"
#define MAIN_FUNCTIONS 1
#endif

#define MAX_STACK_HEIGHT  2000
#define MAX_CODE_LENGTH  500
#define MAX_LEXI_LEVEL  3

// VM statuses
#define STATUS_OK  0
#define STATUS_ERR  1
#define STATUS_QUIT  2 
#define STATUS_FINISHED  2 

// user commands supplied running with -i flag
#define COMMAND_INVALID  -1
#define COMMAND_QUIT  0 
#define COMMAND_HELP  1 
#define COMMAND_STEP  2 
#define COMMAND_RUN  3 

int main (int argsc, char *argv[]) {

  // running interactive mode. allow user commands
  if (argsc == 2 && strcmp(argv[1], "-i") == 0) {

    interactive_loop();

  }
  else {

    event_loop();

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
int event_loop () {

  // status okay
  int status = STATUS_OK;

  while (status == STATUS_OK) {

    vm_procedures();

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

// the virtual machine execution cycle 
int vm_procedures () {

  return STATUS_FINISHED;

}
