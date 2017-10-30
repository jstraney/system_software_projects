# ifndef  HEADERS 
# include "main.h"
# define  HEADERS
# endif

#define FALSE 0
#define TRUE  1 

#define STATUS_OK   0
#define STATUS_ERR  1 

#define MAX_SYMBOL_TABLE_SIZE

int main (int argc, char * argv[]) {

  int print_list     = 0;
  int print_assembly = 0;
  int print_trace    = 0;

  // no more than 4 arguments supported
  if (argc > 4) {

    // throw error
    return STATUS_ERR;

  }
  else {

    for (int i = 1; i <= argc; i++) {

      char *flag = argv[i-1];

      if (strcmp(flag, "-l") == 0) {

        print_list = TRUE;

      }
      else if (strcmp(flag, "-a") == 0){

        print_assembly = TRUE;

      }
      else if (strcmp(flag, "-v") == 0){

        print_trace = TRUE;

      }

    }

  }

  return STATUS_OK;

}


int read_lexeme_table (Token head) {

}
