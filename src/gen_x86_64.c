#include "../include/gen_x86_64.h"

#define REG_LIST_SIZE 8

static int free_reg_list[REG_LIST_SIZE] = { 1, 1, 1, 1, 1, 1, 1, 1 };
static char *name_reg_list[REG_LIST_SIZE] =
{
	"%r8",  "%r9",  "%r10",
	"%r11", "%r12", "%r13",
	"%r14", "%r15"
};

extern FILE   *output_file;
extern Vector *string_list;

static int alloc_reg()
{
	for (int i = 0; i < REG_LIST_SIZE; ++i)
	{
		if (free_reg_list[i] == 1)
		{
			free_reg_list[i] = 0;
			return i;
		}
	}

	error(0, 0, "there are no registers");
	return -1;
}

static void free_reg(int reg)
{
	if (free_reg_list[reg] != 0)
		error(0, 0, "Error trying to free register");
	free_reg_list[reg] = 1;
}

static void cg_str_data()
{
	for (int i = 0; i < string_list->length; ++i)
	{
		char *str = string_list->data[i];

		fprintf(output_file, ".LC%i:\n", i);
		fprintf(output_file,"\t.string \"%s\"\n", str);
	}
}

void cg_start()
{
	fprintf(output_file, ".print_int:\n");
	fprintf(output_file, "\t.string \"%%i\"\n");
	cg_str_data();
	fprintf(output_file, "\n");

	fprintf(output_file, ".globl main\n");
	fprintf(output_file, "\n");

	fprintf(output_file, "main:\n");
	fprintf(output_file, "\tpushq %%rbp\n");
	fprintf(output_file, "\tmovq %%rsp, %%rbp\n");
	fprintf(output_file, "\n");
}

void cg_end()
{
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tpopq %%rbp\n");
	fprintf(output_file, "\tret\n");
}

void cg_print_int(int reg)
{
	fprintf(output_file, "\tmovq %s, %%rsi\n", name_reg_list[reg]);
	fprintf(output_file, "\tmovq $.print_int, %%rdi\n");
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
	fprintf(output_file, "\n");
}

void cg_print_str(int number)
{
	fprintf(output_file, "\tmovq $.LC%i, %%rdi\n", number);
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
	fprintf(output_file, "\n");
}

int cg_add(int r1, int r2)
{
	fprintf(output_file, "\taddq %s, %s\n", name_reg_list[r2], name_reg_list[r1]);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_sub(int r1, int r2)
{
	fprintf(output_file, "\tsubq %s, %s\n", name_reg_list[r2], name_reg_list[r1]);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_mult(int r1, int r2)
{
	fprintf(output_file, "\timulq %s, %s\n", name_reg_list[r2], name_reg_list[r1]);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_div(int r1, int r2)
{
	fprintf(output_file, "\tmovq %s, %%rax\n", name_reg_list[r1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", name_reg_list[r2]);
	fprintf(output_file, "\tmovq %%rax, %s\n", name_reg_list[r1]);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_mod(int r1, int r2)
{
	fprintf(output_file, "\tmovq %s, %%rax\n", name_reg_list[r1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", name_reg_list[r2]);
	fprintf(output_file, "\tmovq %%rdx, %s\n", name_reg_list[r1]);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_neg(int r1)
{
	fprintf(output_file, "\tnegq %s\n", name_reg_list[r1]);
	return r1;
}

int cg_load(int value)
{
	int reg_i = alloc_reg();
	fprintf(output_file, "\tmovq $%i, %s\n", value, name_reg_list[reg_i]);
	fprintf(output_file, "\n");
	return reg_i;
}