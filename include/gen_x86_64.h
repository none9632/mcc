#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdio.h>

#include "vector.h"
#include "error.h"

void free_reg (int reg);

void cg_start ();
void cg_end   ();

int  cg_label      ();
void cg_condit_jmp (int reg);
void cg_jmp        (int offset);

void cg_print_int (int reg);
void cg_print_str (int number);

int cg_or      (int reg1, int r2);
int cg_and     (int r1, int r2);
int cg_compare (int r1, int r2, char *how);
int cg_add     (int r1, int r2);
int cg_sub     (int r1, int r2);
int cg_mult    (int r1, int r2);
int cg_div     (int r1, int r2);
int cg_mod     (int r1, int r2);
int cg_neg     (int r1);
int cg_load    (int r1);

#endif
