#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VMACHINE
#include "vm.h"
#define VMACHINE
#endif

#ifndef ANALYZER
#include "analyzer.h"
#define ANALYZER
#endif

typedef struct {
  // I'm calling it type here just because in assignment 2, the
  // tokens were also of different 'types'
  int  type;
  char name [10];
  int  val;
  int  level;
  int  addr;

} Symbol;

Symbol_insert(Symbol symbol)
