#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdio.h>

#include "vector.h"
#include "error.h"

void cg_start ();
void cg_end   ();

void cg_print_int (int reg);
void cg_print_str (int number);

int cg_add  (int r1, int r2);
int cg_sub  (int r1, int r2);
int cg_mult (int r1, int r2);
int cg_div  (int r1, int r2);
int cg_mod  (int r1, int r2);
int cg_neg  (int r1);
int cg_load (int r1);

#endif
