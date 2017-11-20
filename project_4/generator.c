#ifndef STDH 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define STDH 
#endif

#ifndef VM
#include "vm.h"
#define VM 
#endif

#ifndef GENERATOR
#include "generator.h"
#define GENERATOR
#endif

// lexical level tracked by the generator. used when generating assembly with
// parse tree. setting it to negative 1 so it becomes 0 when main is entered.
int gen_lex_level = -1;

// offsets from start of activation records. -1 indicates unused.
int lex_offsets[VM_MAX_LEXI_LEVEL] = {-1};

// generators abstraction of program counter for incrementing
int gen_pc = PC;


// bp offset from lexigraphical level (referred to as M in some commands)
// for example, when doing a STO instruction, we need to know the offset
// from the start of the activation record.
int gen_bp_offset = 0;

// abstraction of stack pointer, mainly used to

// okay. was not really sure how to handle register management, but am using
// a "round robin" approach which basically cycles through. I achieve this
// with modular arithmetic
int gen_reg_offset = IR;

void gen_inc_reg_offset () {

  gen_reg_offset++;

}

int gen_get_last_reg () {

  // return last reg address used
  return (gen_reg_offset - 1) % VM_REG_FILE_SIZE;

}

int gen_get_reg_offset () {

  // remember, the offset just keeps increasing and is modded
  // to rotate through the register as needed
  return (gen_reg_offset % VM_REG_FILE_SIZE);

}

int gen_get_bp (int lex_level) {

  return lex_offsets[lex_level];

}

int gen_get_bp_offset () {

  return gen_bp_offset - lex_offsets[gen_lex_level];

}

// offset increases when we create space for a variable
void gen_inc_bp_offset () {

  gen_bp_offset++;

}

// array of string codes to be written to a file
char *gen_code[VM_MAX_CODE_LENGTH];

Instruction code[VM_MAX_CODE_LENGTH];

int write_assembly_to_file (int is_verbose) {

  FILE *fp;

  fp = fopen("assembly.code", "w");

  if (fp == NULL) {

    printf("GEN STATUS: could not open assembly.code for writing\n");

    return GENERATOR_STATUS_OK;

  }

  // print header
  if (is_verbose) {

    printf("Assembly Output\n");

  }

  for (int i = 0; i < gen_pc; i++) {

    Instruction instruction;

    instruction = code[i];

    char * instr_str = Instruction_to_string(instruction);

    if (is_verbose) {

      // not the actual output of file. just a human friendly print
      printf("%d %s %d %d %d\n", i, get_str_opcode(instruction.op), instruction.r, instruction.l, instruction.m);

    }
    
    fwrite(instr_str, sizeof(char), strlen(instr_str), fp);

  }

  printf("\n");

  fclose(fp);

  printf("GEN STATUS: finished writing to assembly.code\n");

  return GENERATOR_STATUS_OK;

}

void gen_inc_lex_lev() {

  gen_lex_level = gen_lex_level + 1;

  // set the offset of lexical level to the current offset from BP
  lex_offsets[gen_lex_level] = gen_bp_offset;

  // allocate space for return value, dynaic link, static link, return address.
  // increase offset from BP for AR variables
  if (gen_lex_level > 0) 
    gen_bp_offset += 4;

}

void gen_dec_lex_lev() {

  gen_lex_level = gen_lex_level - 1;

  // jump before any AR specific memory
  gen_bp_offset -= 4;

}

int gen_get_lex_level () {

  return gen_lex_level;

}

void gen_print_inst (char * inst) {

  // printf("new inst! %s\n", inst);

}

int gen(int op, int reg, int l, int m) {

  // put new instruction in code
  code[gen_pc++] = Instruction_new(op, reg, l, m);

  // printf("%s: %d %d %d\n", get_str_opcode(op), reg, l, m);

  // gen_print_inst(instruction_str);

  // insert strin instruction in code array
  // gen_code[gen_pc++] = instruction_str;

  return GENERATOR_STATUS_OK;

}

// allows the modification of an instruction's M value
void gen_alt_instr_m (int instr_addr, int instr_m) {

  code[instr_addr].m = instr_m;

  // testing only
  // Instruction instr = code[instr_addr];
  // printf("changedjpc %d at %d\n", instr.op, instr_addr);
  // printf("%s: %d %d %d\n", get_str_opcode(instr.op), instr.r, instr.l, instr.m);

}

// used in jump commands and jump conditionals
int gen_get_pc () {

  return gen_pc;

}


