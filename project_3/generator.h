#ifndef STDH 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STDH 
#endif

#ifndef ANALYZER
#include "analyzer.h"
#define ANALYZER
#endif

// generator statuses
#define GENERATOR_STATUS_OK      0
#define GENERATOR_STATUS_ERR     1 


// hash function used for symbol table
int generator_entry(Token *head);

int gen(int op, int reg, int l, int m);

void gen_inc_lex_lev();

void gen_dec_lex_lev();

// returns lexical level maintained by generator
int gen_get_lex_level ();

int gen_get_bp_offset ();

void gen_inc_bp_offset ();

void gen_dec_bp_offset ();

int gen_get_last_reg();

void gen_inc_reg_offset();

int gen_get_reg_offset();

// should only really have to increase bp for allocation
// (once variables are allocated, their address can be retrieved
// by symbol)
void gen_inc_bp_offset ();

// get's the generators abstraction of PC
int gen_get_pc ();

// allows an instructions m value to be changed. (only case
// for modifying instructions seems to be JPC at this time)
void gen_alt_instr_m (int instr_addr, int instr_m);

int write_assembly_to_file(int is_verbose);

