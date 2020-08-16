#include "gen_x86_64.h"

#define REG_LIST_SIZE 13
#define FREE 0
#define BUSY 1
#define STACK (-1)
#define R9 6
#define RBX 11
#define RDI 12

typedef struct reg
{
	char *reg8;
	char *reg32;
	char *reg64;
	int is_free;
}
Reg;

static Reg reg_list[REG_LIST_SIZE] =
{
	{ "%r15b", "%r15d", "%r15", FREE },
	{ "%r14b", "%r14d", "%r14", FREE },
	{ "%r13b", "%r13d", "%r13", FREE },
	{ "%r12b", "%r12d", "%r12", FREE },
	{ "%r11b", "%r11d", "%r11", FREE },
	{ "%r10b", "%r10d", "%r10", FREE },
	{ "%r9b",  "%r9d",  "%r9",  FREE },
	{ "%r8b",  "%r8d",  "%r8",  FREE },
	{ "%cl",   "%ecx",  "%rcx", FREE },
	{ "%dl",   "%edx",  "%rdx", FREE },
	{ "%sil",  "%esi",  "%rsi", FREE },
	{ "%bl",   "%ebx",  "%rbx", FREE },       // used for pushing and popping values from the stack
	{ "%dil",  "%edi",  "%rdi", FREE },       // used for pushing and popping values from the stack
};

extern FILE   *output_file;
extern Vector *string_list;

static int push_offset;

static int alloc_reg()
{
	for (int i = 0; i < REG_LIST_SIZE - 5; ++i)
	{
		if (reg_list[i].is_free == FREE)
		{
			reg_list[i].is_free = BUSY;
			return i;
		}
	}

	return STACK;
}

void free_reg(int reg)
{
	reg_list[reg].is_free = FREE;
}

int *cg_save_all_reg()
{
	int *buf_frl = malloc(sizeof(int) * (REG_LIST_SIZE - 2));

	for (int i = 0; i < REG_LIST_SIZE - 2; ++i)
	{
		buf_frl[i] = reg_list[i].is_free;
		if (reg_list[i].is_free == BUSY)
		{
			fprintf(output_file, "\tpushq %s\n", reg_list[i].reg64);
			reg_list[i].is_free = FREE;
		}
	}

	return buf_frl;
}

void cg_ret_all_reg(int *buf_frl)
{
	for (int i = REG_LIST_SIZE - 3; i >= 0; --i)
	{
		reg_list[i].is_free = buf_frl[i];
		if (reg_list[i].is_free == BUSY)
			fprintf(output_file, "\tpopq %s\n", reg_list[i].reg64);
	}

	free(buf_frl);
}

static void pop_value(int *reg1, int *reg2)
{
	if (reg2 != NULL && *reg2 == STACK)
	{
		fprintf(output_file, "\tpopq %%rdi\n");
		*reg2 = RDI;
		push_offset -= 8;
	}
	if (*reg1 == STACK)
	{
		fprintf(output_file, "\tpopq %%rbx\n");
		*reg1 = RBX;
		push_offset -= 8;
	}
}

static void push_value(int *reg1)
{
	if (*reg1 == RBX)
	{
		fprintf(output_file, "\tpushq %%rbx\n");
		*reg1 = STACK;
		push_offset += 8;
	}
}

void cg_start_prog()
{
	fprintf(output_file, ".io_int:\n");
	fprintf(output_file, "\t.string \"%%i\"\n");

	for (int i = 0; i < string_list->length; ++i)
	{
		fprintf(output_file, ".LC%i:\n", i);
		fprintf(output_file,"\t.string \"%s\"\n", string_list->data[i]);
	}

	fprintf(output_file, ".globl main\n");
}

void cg_start_func(char *name, int size)
{
	fprintf(output_file, "%s:\n", name);
	fprintf(output_file, "\tpushq %%rbp\n");
	fprintf(output_file, "\tmovq %%rsp, %%rbp\n");

	if (size != 0)
		fprintf(output_file, "\tsubq $%i, %%rsp\n", size);

	push_offset = 0;
}

void cg_end_func(int label, int size)
{
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, ".L%i:\n", label);

	if (size != 0)
		fprintf(output_file, "\taddq $%i, %%rsp\n", size);

	fprintf(output_file, "\tpopq %%rbp\n");
	fprintf(output_file, "\tret\n");
}

void cg_ret_func(int reg, int label)
{
	fprintf(output_file, "\tmovl %s, %%eax\n", reg_list[reg].reg32);
	fprintf(output_file, "\tjmp .L%i\n", label);
	free_reg(reg);
}

void cg_label(int label)
{
	fprintf(output_file, ".L%i:\n", label);
}

void cg_condit_jmp(int reg, int label)
{
	fprintf(output_file, "\tcmpl $0, %s\n", reg_list[reg].reg32);
	fprintf(output_file, "\tje .L%i\n", label);
	free_reg(reg);
}

void cg_jmp(int label)
{
	fprintf(output_file, "\tjmp .L%i\n", label);
}

void cg_arg_print(int reg1, int reg2)
{
	if (reg_list[reg2].is_free == FREE)
		reg_list[reg2].is_free = BUSY;

	if (strcmp(reg_list[reg1].reg64, reg_list[reg2].reg64))
	{
		fprintf(output_file, "\tmovq %s, %s\n", reg_list[reg1].reg64, reg_list[reg2].reg64);
		free_reg(reg1);
	}
}

void cg_print(int value, int length)
{
	fprintf(output_file, "\tmovq $.LC%i, %%rdi\n", value);
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");

	int size = (length - PRINT_REG_SIZE) * 8;
	if (size > 0)
	{
		fprintf(output_file, "\taddq $%i, %%rsp\n", size);
		push_offset -= size;
	}

	for (int i = R9; i < RBX; ++i)
		reg_list[i].is_free = FREE;
}

void cg_input(char *pointer, int offset)
{
	fprintf(output_file, "\tleaq %i(%s), %%rsi\n", offset + push_offset, pointer);
	fprintf(output_file, "\tmovq $.io_int, %%rdi\n");
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall scanf\n");
}

int cg_or(int reg1, int reg2, int label1, int label2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpl $0, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tjne .L%i\n", label1);
	fprintf(output_file, "\tcmpl $0, %s\n", reg_list[reg2].reg32);
	fprintf(output_file, "\tjne .L%i\n", label1);
	fprintf(output_file, "\tmovl $0, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tjmp .L%i\n", label2);
	fprintf(output_file, ".L%i:\n", label1);
	fprintf(output_file, "\tmovl $1, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, ".L%i:\n", label2);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_and(int reg1, int reg2, int label1, int label2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpl $0, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tje .L%i\n", label1);
	fprintf(output_file, "\tcmpl $0, %s\n", reg_list[reg2].reg32);
	fprintf(output_file, "\tje .L%i\n", label1);
	fprintf(output_file, "\tmovl $1, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tjmp .L%i\n", label2);
	fprintf(output_file, ".L%i:\n", label1);
	fprintf(output_file, "\tmovl $0, %s\n", reg_list[reg1].reg32);
	fprintf(output_file, ".L%i:\n", label2);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_compare(int reg1, int reg2, char *how)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpl %s, %s\n", reg_list[reg2].reg32, reg_list[reg1].reg32);
	fprintf(output_file, "\t%s %s\n", how, reg_list[reg1].reg8);
	fprintf(output_file, "\tandl $255, %s\n", reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_add(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\taddl %s, %s\n", reg_list[reg2].reg32, reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_sub(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tsubl %s, %s\n", reg_list[reg2].reg32, reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_mult(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\timull %s, %s\n", reg_list[reg2].reg32, reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_div(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tmovl %s, %%eax\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivl %s\n", reg_list[reg2].reg32);
	fprintf(output_file, "\tmovl %%eax, %s\n", reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_mod(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tmovl %s, %%eax\n", reg_list[reg1].reg32);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivl %s\n", reg_list[reg2].reg32);
	fprintf(output_file, "\tmovl %%edx, %s\n", reg_list[reg1].reg32);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_neg(int reg1)
{
	pop_value(&reg1, NULL);

	fprintf(output_file, "\tnegl %s\n", reg_list[reg1].reg32);

	push_value(&reg1);
	return reg1;
}

int cg_load(int value)
{
	int reg = alloc_reg();

	if (reg == STACK)
	{
		fprintf(output_file, "\tpushq $%i\n", value);
		push_offset += 8;
	}
	else
		fprintf(output_file, "\tmovl $%i, %s\n", value, reg_list[reg].reg32);

	return reg;
}

int cg_load_gsym(char *pointer, int offset)
{
	int reg = alloc_reg();

	if (reg == STACK)
	{
		fprintf(output_file, "\tpushq %i(%s)\n", offset + push_offset, pointer);
		push_offset += 8;
	}
	else
		fprintf(output_file, "\tmovl %i(%s), %s\n", offset + push_offset, pointer, reg_list[reg].reg32);

	return reg;
}

int cg_store_gsym(int reg, char *pointer, int offset)
{
	fprintf(output_file, "\tmovl %s, %i(%s)\n", reg_list[reg].reg32, offset + push_offset, pointer);
	return reg;
}

void cg_uninit_var(char *pointer, int offset)
{
	fprintf(output_file, "\tmovl $0, %i(%s)\n", offset, pointer + push_offset);
}

void cg_func_call(char *name)
{
	fprintf(output_file, "\tcall %s\n", name);
}

int cg_ret_value()
{
	int reg = alloc_reg();

	if (reg == STACK)
	{
		fprintf(output_file, "\tpushq %%rax\n");
		push_offset += 8;
	}
	else
		fprintf(output_file, "\tmovl %%eax, %s\n", reg_list[reg].reg32);

	return reg;
}

void cg_push_stack(int reg)
{
	fprintf(output_file, "\tpushq %s\n", reg_list[reg].reg64);
	push_offset += 8;
	free_reg(reg);
}

void cg_pop_stack()
{
	fprintf(output_file, "\tpopq %%rdi\n");
	push_offset -= 8;
}
