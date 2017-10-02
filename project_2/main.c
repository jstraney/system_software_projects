/*
L)       E)eeeeee X)    xx I)iiii   C)ccc    A)aa   L)       
L)       E)        X)  xx    I)    C)   cc  A)  aa  L)       
L)       E)eeeee    X)xx     I)   C)       A)    aa L)       
L)       E)         X)xx     I)   C)       A)aaaaaa L)       
L)       E)        X)  xx    I)    C)   cc A)    aa L)       
L)llllll E)eeeeee X)    xx I)iiii   C)ccc  A)    aa L)llllll 
                                                             
                                                             
  A)aa   N)n   nn   A)aa   L)       Y)    yy Z)zzzzzz E)eeeeee R)rrrrr  
 A)  aa  N)nn  nn  A)  aa  L)        Y)  yy        Z) E)       R)    rr 
A)    aa N) nn nn A)    aa L)         Y)yy       Z)   E)eeeee  R)  rrr  
A)aaaaaa N)  nnnn A)aaaaaa L)          Y)       Z)    E)       R) rr    
A)    aa N)   nnn A)    aa L)          Y)     Z)      E)       R)   rr  
A)    aa N)    nn A)    aa L)llllll    Y)    Z)zzzzzz E)eeeeee R)    rr 

*/
// written by Jeffrey Straney and Frank Volk
// 10-2-2017
// COP3402 HW 2

//// Symbol Implementations ////
Symbol Symbol_new(Symbol *next, char value) {

  Symbol this;

  // set pointer to first symbol
  this.next = next;

  // set value to character
  this.value = value;

  return this;

}

SymbolTable SymbolTable_new (int size) {

  SymbolTable this;

  // points to the 'lowest' symbol in the alphabet
  this.symbols = (Symbol *)malloc(sizeof int * size);

  // useful for various checks. again, indicates alphabet range,
  // rather than total number of characters
  this.size = size;

  return this;

}

Symbol lookup_symbol(char symbol_value, SymbolTable table) {

  return *(table.symbols + symbol_value)

}

int main (int argc, char * argv[]) {

}


