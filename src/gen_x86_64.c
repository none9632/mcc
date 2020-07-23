#include "../include/gen_x86_64.h"

#define REG_LIST_SIZE 8

static int l_count = 0;

static int   free_reg_list[REG_LIST_SIZE] = { 1, 1, 1, 1, 1, 1, 1, 1 };
static char *reg_list[REG_LIST_SIZE] =
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

void free_reg(int reg)
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

	fprintf(output_file, ".globl main\n");
	fprintf(output_file, "\n");

	fprintf(output_file, "main:\n");
	fprintf(output_file, "\tpushq %%rbp\n");
	fprintf(output_file, "\tmovq %%rsp, %%rbp\n");
}

void cg_end()
{
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tpopq %%rbp\n");
	fprintf(output_file, "\tret\n");
}

void cg_condit_jmp(int reg)
{
	fprintf(output_file, "\tcmpq $0, %s\n", reg_list[reg]);
	fprintf(output_file, "\tje .L%i\n", l_count);
}

void cg_jmp(int offset)
{
	fprintf(output_file, "\tjmp .L%i\n", l_count + offset);
}

int cg_label()
{
	fprintf(output_file, ".L%i:\n", l_count);
	return l_count++;
}

int cg_or(int reg1, int reg2)
{
	fprintf(output_file, "\tcmpq $0, %s\n", reg_list[reg1]);
	fprintf(output_file, "\tjne .L%i\n", l_count);
	fprintf(output_file, "\tcmpq $0, %s\n", reg_list[reg2]);
	fprintf(output_file, "\tjne .L%i\n", l_count);
	fprintf(output_file, "\tmovq $0, %s\n", reg_list[reg1]);
	fprintf(output_file, "\tjmp .L%i\n", l_count + 1);
	fprintf(output_file, ".L%i:\n", l_count++);
	fprintf(output_file, "\tmovq $1, %s\n", reg_list[reg1]);
	fprintf(output_file, ".L%i:\n", l_count++);
	free_reg(reg2);
	return reg1;
}

int cg_and(int reg1, int reg2)
{
	fprintf(output_file, "\tcmpq $0, %s\n", reg_list[reg1]);
	fprintf(output_file, "\tje .L%i\n", l_count);
	fprintf(output_file, "\tcmpq $0, %s\n", reg_list[reg2]);
	fprintf(output_file, "\tje .L%i\n", l_count);
	fprintf(output_file, "\tmovq $1, %s\n", reg_list[reg1]);
	fprintf(output_file, "\tjmp .L%i\n", l_count + 1);
	fprintf(output_file, ".L%i:\n", l_count++);
	fprintf(output_file, "\tmovq $0, %s\n", reg_list[reg1]);
	fprintf(output_file, ".L%i:\n", l_count++);
	free_reg(reg2);
	return reg1;
}

void cg_print_int(int reg)
{
	fprintf(output_file, "\tmovq %s, %%rsi\n", reg_list[reg]);
	fprintf(output_file, "\tmovq $.print_int, %%rdi\n");
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
}

void cg_print_str(int number)
{
	fprintf(output_file, "\tmovq $.LC%i, %%rdi\n", number);
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
}

int cg_compare(int reg1, int reg2, char *how)
{
	fprintf(output_file, "\tcmpq %s, %s\n", reg_list[reg2], reg_list[reg1]);
	fprintf(output_file, "\t%s %sb\n", how, reg_list[reg1]);
	fprintf(output_file, "\tandq $255,%s\n", reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_add(int reg1, int reg2)
{
	fprintf(output_file, "\taddq %s, %s\n", reg_list[reg2], reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_sub(int reg1, int reg2)
{
	fprintf(output_file, "\tsubq %s, %s\n", reg_list[reg2], reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_mult(int reg1, int reg2)
{
	fprintf(output_file, "\timulq %s, %s\n", reg_list[reg2], reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_div(int reg1, int reg2)
{
	fprintf(output_file, "\tmovq %s, %%rax\n", reg_list[reg1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", reg_list[reg2]);
	fprintf(output_file, "\tmovq %%rax, %s\n", reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_mod(int reg1, int reg2)
{
	fprintf(output_file, "\tmovq %s, %%rax\n", reg_list[reg1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", reg_list[reg2]);
	fprintf(output_file, "\tmovq %%rdx, %s\n", reg_list[reg1]);
	free_reg(reg2);
	return reg1;
}

int cg_neg(int reg1)
{
	fprintf(output_file, "\tnegq %s\n", reg_list[reg1]);
	return reg1;
}

int cg_load(int value)
{
	int reg = alloc_reg();
	fprintf(output_file, "\tmovq $%i, %s\n", value, reg_list[reg]);
	return reg;
}