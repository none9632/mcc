#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdio.h>

#include "vector.h"

void free_reg        (int reg);
int *cg_save_all_reg ();
int  cg_ret_all_reg  (int *buf_frl);

void cg_start_prog ();

void cg_start_func (char *name, int size);
void cg_end_func   (int label, int size);
void cg_ret_func   (int reg, int label);
void cg_func_call  (char *name);

void cg_label      (int label);
void cg_condit_jmp (int reg, int label);
void cg_jmp        (int label);

void cg_push_stack (int reg);
void cg_pop_stack  ();

void cg_print (int value, int length);
void cg_input (char *pointer, int offset);

int  cg_load_gsym  (char *pointer, int offset);
int  cg_store_gsym (int reg, char *pointer, int offset);
void cg_uninit_var (char *pointer, int offset);

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
