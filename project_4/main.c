/*
qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb

       _ _     _   | _     _ ,_ _     |-    
|)(||`_\(/_|` (_()(|(/_ (|(/_||(/_|`(||_()|`
|                       _|                  

pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp
dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd
*/
// Source code created by Frank Volk and Jeffrey Straney
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

  char* file_name = NULL;

  // no more than 4 arguments supported
  if (argc > 5) {

    // throw error
    return DRIVER_STATUS_ERR;

  }

  for (int i = 1; i < argc; i++) {

    char *arg = argv[i];

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

  if (file_name == NULL) {

    printf("please provide the name of file to parse\n");

    return 1;

  }

  // submit the filename argument to analyzer(and parser)
  status = analyzer_entry(file_name);

  if (status < 0) {

    return status;

  }

  if (print_list) {

    print_token_table();

  }

  // gen has code created at this point. print_assembly passed in
  // as flag to trigger a 'verbose' mode while writing assembly
  // file if -a flag is provided
  write_assembly_to_file(print_assembly);

  // run machine code on VM
  vm_entry("assembly.code", print_trace);

  return status;

}
