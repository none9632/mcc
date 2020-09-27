#ifndef GEN_X86_64_H
#define GEN_X86_64_H

#include <stdlib.h>

#define GLOBAL_MODE 1
#define LOCAL_MODE 0

void    free_reg        (int8_t reg);
int8_t *cg_save_all_reg ();
void    cg_ret_all_reg  (int8_t *buf_frl);

void cg_start_prog ();
void cg_end_start  ();

void   cg_init_gvar  (char *name);
int8_t cg_store_gvar (int8_t reg, char *name);
int8_t cg_load_gvar  (char *name);

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

void cg_arg_printf (int8_t reg1, uint index);
void cg_printf     (size_t value, uint length);
void cg_scanf      (char *pointer, uint offset, int8_t mode);

int8_t cg_or          (int8_t reg1, int8_t reg2, uint label1, uint label2);
int8_t cg_and         (int8_t reg1, int8_t reg2, uint label1, uint label2);
int8_t cg_bit_or      (int8_t reg1, int8_t reg2);
int8_t cg_bit_xor     (int8_t reg1, int8_t reg2);
int8_t cg_bit_and     (int8_t reg1, int8_t reg2);
int8_t cg_compare     (int8_t reg1, int8_t reg2, char *how);
int8_t cg_left_shift  (int8_t reg1, int8_t reg2);
int8_t cg_right_shift (int8_t reg1, int8_t reg2);
int8_t cg_add         (int8_t reg1, int8_t reg2);
int8_t cg_sub         (int8_t reg1, int8_t reg2);
int8_t cg_mult        (int8_t reg1, int8_t reg2);
int8_t cg_div         (int8_t reg1, int8_t reg2);
int8_t cg_mod         (int8_t reg1, int8_t reg2);
int8_t cg_neg         (int8_t reg1);
int8_t cg_pre_inc     (int8_t reg1, char *pointer, uint offset);
int8_t cg_pre_dec     (int8_t reg1, char *pointer, uint offset);
void   cg_post_inc    (char *pointer, uint offset);
void   cg_post_dec    (char *pointer, uint offset);
int8_t cg_load_num    (int value);
int8_t cg_load_var    (char *pointer, uint offset);
int8_t cg_store_var   (int8_t reg, char *pointer, uint offset);

#endif
