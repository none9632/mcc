#include "../include/gen_x86_64.h"

#define REG_LIST_SIZE 10
#define STACK (-1)
#define RBX 8
#define RCX 9

static int   free_reg_list[REG_LIST_SIZE] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
static char *reg64_list[REG_LIST_SIZE] =
{
	"%r8",  "%r9",  "%r10",
	"%r11", "%r12", "%r13",
	"%r14", "%r15", "%rbx",
	"%rcx"
};
static char *reg8_list[REG_LIST_SIZE] =
{
	"%r8b",  "%r9b",  "%r10b",
	"%r11b", "%r12b", "%r13b",
	"%r14b", "%r15b", "%bl",
	"%cl"
};

extern FILE   *output_file;
extern Vector *string_list;

static int alloc_reg()
{
	for (int i = 0; i < REG_LIST_SIZE - 2; ++i)
	{
		if (free_reg_list[i] == 1)
		{
			free_reg_list[i] = 0;
			return i;
		}
	}

	return STACK;
}

void free_reg(int reg)
{
	free_reg_list[reg] = 1;
}

int *cg_save_all_reg()
{
	int *buf_frl = malloc(sizeof(int) * (REG_LIST_SIZE - 2));

	for (int i = 0; i < REG_LIST_SIZE - 2; ++i)
	{
		buf_frl[i] = free_reg_list[i];
		if (free_reg_list[i] == 0)
		{
			fprintf(output_file, "\tpushq %s\n", reg64_list[i]);
			free_reg_list[i] = 1;
		}
	}

	return buf_frl;
}

int cg_ret_all_reg(int *buf_frl)
{
	for (int i = REG_LIST_SIZE - 3; i >= 0; --i)
	{
		free_reg_list[i] = buf_frl[i];
		if (free_reg_list[i] == 0)
			fprintf(output_file, "\tpopq %s\n", reg64_list[i]);
	}

	int reg = alloc_reg();

	if (reg == STACK)
		fprintf(output_file, "\tpushq %%rax\n");
	else
		fprintf(output_file, "\tmovq %%rax, %s\n", reg64_list[reg]);

	return reg;
}

static void pop_value(int *reg1, int *reg2)
{
	if (reg2 != NULL && *reg2 == STACK)
	{
		fprintf(output_file, "\tpopq %%rcx\n");
		*reg2 = RCX;
	}
	if (*reg1 == STACK)
	{
		fprintf(output_file, "\tpopq %%rbx\n");
		*reg1 = RBX;
	}
}

static void push_value(int *reg1)
{
	if (*reg1 == RBX)
	{
		fprintf(output_file, "\tpushq %%rbx\n");
		*reg1 = STACK;
	}
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

void cg_start_prog()
{
	fprintf(output_file, ".io_int:\n");
	fprintf(output_file, "\t.string \"%%li\"\n");
	cg_str_data();

	fprintf(output_file, ".globl main\n");
	fprintf(output_file, "\n");
}

void cg_start_func(char *name, int size)
{
	fprintf(output_file, "%s:\n", name);
	fprintf(output_file, "\tpushq %%rbp\n");
	fprintf(output_file, "\tmovq %%rsp, %%rbp\n");

	if (size != 0)
		fprintf(output_file, "\tsubq $%i, %%rsp\n", size);
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
	fprintf(output_file, "\tmovq %s, %%rax\n", reg64_list[reg]);
	fprintf(output_file, "\tjmp .L%i\n", label);
	free_reg(reg);
}

void cg_label(int label)
{
	fprintf(output_file, ".L%i:\n", label);
}

void cg_condit_jmp(int reg, int label)
{
	fprintf(output_file, "\tcmpq $0, %s\n", reg64_list[reg]);
	fprintf(output_file, "\tje .L%i\n", label);
	free_reg(reg);
}

void cg_jmp(int label)
{
	fprintf(output_file, "\tjmp .L%i\n", label);
}

void cg_print_int(int reg)
{
	fprintf(output_file, "\tmovq %s, %%rsi\n", reg64_list[reg]);
	fprintf(output_file, "\tleaq .io_int, %%rdi\n");
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
	free_reg(reg);
}

void cg_print_str(int number)
{
	fprintf(output_file, "\tmovq $.LC%i, %%rdi\n", number);
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall printf\n");
}

void cg_input(char *pointer)
{
	fprintf(output_file, "\tleaq %s, %%rsi\n", pointer);
	fprintf(output_file, "\tmovq $.io_int, %%rdi\n");
	fprintf(output_file, "\txor %%rax, %%rax\n");
	fprintf(output_file, "\tcall scanf\n");
}

int cg_or(int reg1, int reg2, int label1, int label2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpq $0, %s\n", reg64_list[reg1]);
	fprintf(output_file, "\tjne .L%i\n", label1);
	fprintf(output_file, "\tcmpq $0, %s\n", reg64_list[reg2]);
	fprintf(output_file, "\tjne .L%i\n", label1);
	fprintf(output_file, "\tmovq $0, %s\n", reg64_list[reg1]);
	fprintf(output_file, "\tjmp .L%i\n", label2);
	fprintf(output_file, ".L%i:\n", label1);
	fprintf(output_file, "\tmovq $1, %s\n", reg64_list[reg1]);
	fprintf(output_file, ".L%i:\n", label2);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_and(int reg1, int reg2, int label1, int label2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpq $0, %s\n", reg64_list[reg1]);
	fprintf(output_file, "\tje .L%i\n", label1);
	fprintf(output_file, "\tcmpq $0, %s\n", reg64_list[reg2]);
	fprintf(output_file, "\tje .L%i\n", label1);
	fprintf(output_file, "\tmovq $1, %s\n", reg64_list[reg1]);
	fprintf(output_file, "\tjmp .L%i\n", label2);
	fprintf(output_file, ".L%i:\n", label1);
	fprintf(output_file, "\tmovq $0, %s\n", reg64_list[reg1]);
	fprintf(output_file, ".L%i:\n", label2);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_compare(int reg1, int reg2, char *how)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tcmpq %s, %s\n", reg64_list[reg2], reg64_list[reg1]);
	fprintf(output_file, "\t%s %s\n", how, reg8_list[reg1]);
	fprintf(output_file, "\tandq $255,%s\n", reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_add(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\taddq %s, %s\n", reg64_list[reg2], reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_sub(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tsubq %s, %s\n", reg64_list[reg2], reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_mult(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\timulq %s, %s\n", reg64_list[reg2], reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_div(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tmovq %s, %%rax\n", reg64_list[reg1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", reg64_list[reg2]);
	fprintf(output_file, "\tmovq %%rax, %s\n", reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_mod(int reg1, int reg2)
{
	pop_value(&reg1, &reg2);

	fprintf(output_file, "\tmovq %s, %%rax\n", reg64_list[reg1]);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidivq %s\n", reg64_list[reg2]);
	fprintf(output_file, "\tmovq %%rdx, %s\n", reg64_list[reg1]);

	free_reg(reg2);
	push_value(&reg1);
	return reg1;
}

int cg_neg(int reg1)
{
	pop_value(&reg1, NULL);

	fprintf(output_file, "\tnegq %s\n", reg64_list[reg1]);

	push_value(&reg1);
	return reg1;
}

int cg_load(int value)
{
	int reg = alloc_reg();

	if (reg == STACK)
		fprintf(output_file, "\tpushq $%i\n", value);
	else
		fprintf(output_file, "\tmovq $%i, %s\n", value, reg64_list[reg]);

	return reg;
}

int cg_load_gsym(char *pointer)
{
	int reg = alloc_reg();

	if (reg == STACK)
		fprintf(output_file, "\tpushq %s\n", pointer);
	else
		fprintf(output_file, "\tmovq %s, %s\n", pointer, reg64_list[reg]);

	return reg;
}

int cg_store_gsym(int reg, char *pointer)
{
	fprintf(output_file, "\tmovq %s, %s\n", reg64_list[reg], pointer);
	return reg;
}

void cg_uninit_var(char *pointer)
{
	fprintf(output_file, "\tmovq $0, %s\n", pointer);
}

void cg_func_call(char *name)
{
	fprintf(output_file, "\tcall %s\n", name);
}

void cg_push_stack(int reg)
{
	fprintf(output_file, "\tpushq %s\n", reg64_list[reg]);
	free_reg(reg);
}

void cg_pop_stack()
{
	int reg = alloc_reg();

	pop_value(&reg, NULL);
	fprintf(output_file, "\tpopq %s\n", reg64_list[reg]);
	free_reg(reg);
}
