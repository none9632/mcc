#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdio.h>

#include "vector.h"
#include "error.h"

void cg_start ();
void cg_end   ();

void cg_label      (int label);
void cg_condit_jmp (int reg, int label);
void cg_jmp        (int label);

void cg_print_int (int reg);
void cg_print_str (int number);
void cg_input     (char *name);

void cg_gsym       (char *name);
int  cg_load_gsym  (char *name);
void cg_store_gsym (int reg, char *name);

int cg_or      (int reg1, int reg2, int label1, int label2);
int cg_and     (int reg1, int reg2, int label1, int label2);
int cg_compare (int reg1, int reg2, char *how);
int cg_add     (int reg1, int reg2);
int cg_sub     (int reg1, int reg2);
int cg_mult    (int reg1, int reg2);
int cg_div     (int reg1, int reg2);
int cg_mod     (int reg1, int reg2);
int cg_neg     (int reg1);
int cg_load    (int value);

#endif
