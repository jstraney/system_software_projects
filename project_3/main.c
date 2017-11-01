# ifndef  HEADERS 
# include "main.h"
# define  HEADERS
# endif

#define DRIVER_STATUS_OK   0
#define DRIVER_STATUS_ERR  1 

int main (int argc, char * argv[]) {

  int status = DRIVER_STATUS_OK;

  int print_list     = 0;
  int print_assembly = 0;
  int print_trace    = 0;

  char* file_name;

  // no more than 4 arguments supported
  if (argc > 4) {

    // throw error
    return DRIVER_STATUS_ERR;

  }

  for (int i = 1; i <= argc; i++) {

    char *arg = argv[i-1];

    if (strcmp(arg, "-l") == 0) {

      print_list = TRUE;

    }
    else if (strcmp(arg, "-a") == 0){

      print_assembly = TRUE;

    }
    else if (strcmp(arg, "-v") == 0){

      print_trace = TRUE;

    }
    else {

      // this must be the code file name!
      file_name = arg;

    }

  }

  // submit the filename argument to analyzer(and parser)
  analyzer_entry(file_name);

  // get head from token table
  Token *head = get_token_table();

  if (print_list) {

    print_token_table();

  }

  if (print_assembly) {

  }

  // run machine code on VM


  return status;

}
