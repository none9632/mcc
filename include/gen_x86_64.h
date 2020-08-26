#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdlib.h>

#define PRINT_REG_SIZE 5

void    free_reg        (int8_t reg);
int8_t *cg_save_all_reg ();
void    cg_ret_all_reg  (int8_t *buf_frl);

void cg_start_prog ();

void   cg_start_func (char *name, int size);
void   cg_end_func   (uint label, int size);
void   cg_ret_func   (int8_t reg, uint label);
void   cg_func_call  (char *name);
int8_t cg_ret_value  ();

void cg_label      (uint label);
void cg_condit_jmp (int8_t reg, uint label);
void cg_jmp        (uint label);

void cg_push_stack (int8_t reg);
void cg_pop_stack  ();

void cg_arg_print (int8_t reg1, int8_t reg2);
void cg_print     (size_t value, uint length);
void cg_input     (char *pointer, uint offset);

int8_t cg_or         (int8_t reg1, int8_t reg2, uint label1, uint label2);
int8_t cg_and        (int8_t reg1, int8_t reg2, uint label1, uint label2);
int8_t cg_compare    (int8_t reg1, int8_t reg2, char *how);
int8_t cg_add        (int8_t reg1, int8_t reg2);
int8_t cg_sub        (int8_t reg1, int8_t reg2);
int8_t cg_mult       (int8_t reg1, int8_t reg2);
int8_t cg_div        (int8_t reg1, int8_t reg2);
int8_t cg_mod        (int8_t reg1, int8_t reg2);
int8_t cg_neg        (int8_t reg1);
int8_t cg_pre_inc    (int8_t reg1, char *pointer, uint offset);
int8_t cg_pre_dec    (int8_t reg1, char *pointer, uint offset);
void   cg_post_inc   (char *pointer, uint offset);
void   cg_post_dec   (char *pointer, uint offset);
int8_t cg_load       (int value);
int8_t cg_load_gsym  (char *pointer, uint offset);
int8_t cg_store_gsym (int8_t reg, char *pointer, uint offset);
void   cg_uninit_var (char *pointer, uint offset);

#endif
