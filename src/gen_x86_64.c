#include "../include/gen_x86_64.h"

#define REG_LIST_SIZE 8

typedef struct reg
{
	char *name;
	int is_free;
}
Reg;

static Reg reg_list[REG_LIST_SIZE] =
{
	{"r8", 1},  {"r9", 1},
	{"r10", 1}, {"r11", 1},
	{"r12", 1}, {"r13", 1},
	{"r14", 1}, {"r15", 1}
};

extern FILE   *output_file;
extern Vector *string_list;

static int alloc_reg()
{
	for (int i = 0; i < REG_LIST_SIZE; ++i)
	{
		if (reg_list[i].is_free == 1)
		{
			reg_list[i].is_free = 0;
			return i;
		}
	}

	error(0, 0, "there are no registers");
	return -1;
}

static void free_reg(int reg)
{
	if (reg_list[reg].is_free != 0)
		error(0, 0, "Error trying to free register");
	reg_list[reg].is_free = 1;
}

static void cg_str_data()
{
	for (int i = 0; i < string_list->length; ++i)
	{
		char *str     = string_list->data[i];
		int   is_open = 0;

		fprintf(output_file,"\tstr%i db ", i);

		while (*str != '\0')
		{
			if (*str == '\\')
			{
				switch (*++str)
				{
					case 'n':
						if (is_open == 1)
						{
							fprintf(output_file, "\", ");
							is_open = 0;
						}
						fprintf(output_file, "10, ");
						break;
					default:
						if (is_open == 0)
						{
							fprintf(output_file, "\"");
							is_open = 1;
						}
						fprintf(output_file, "%c", *(str - 1));
						fprintf(output_file, "%c", *str);
						break;
				}
			}
			else
			{
				if (is_open == 0)
				{
					fprintf(output_file, "\"");
					is_open = 1;
				}
				fprintf(output_file, "%c", *str);
			}
			++str;
		}
		if (is_open == 1)
			fprintf(output_file, "\", ");
		fprintf(output_file, "0\n");
	}
}

void cg_start()
{
	// init data section
	fprintf(output_file, "section .data\n");
	fprintf(output_file, "\tprint_int db \"%%i\", 0\n");
	cg_str_data();
	fprintf(output_file, "\n");

	// init text section
	fprintf(output_file, "section .text\n");
	fprintf(output_file, "global main\n");
	fprintf(output_file, "extern printf\n");
	fprintf(output_file, "\n");

	fprintf(output_file, "main:\n");
	fprintf(output_file, "\tsub rsp, 8\n");
	fprintf(output_file, "\n");
}

void cg_end()
{
	fprintf(output_file, "\txor rax, rax\n");
	fprintf(output_file, "\tadd rsp, 8\n");
	fprintf(output_file, "\tret\n");
}

void cg_print_int(int reg)
{
	fprintf(output_file, "\tmov rsi, %s\n", reg_list[reg].name);
	fprintf(output_file, "\tlea rdi, [rel print_int]\n");
	fprintf(output_file, "\txor rax, rax\n");
	fprintf(output_file, "\tcall printf\n");
	fprintf(output_file, "\n");
}

void cg_print_str(int number)
{
	fprintf(output_file, "\tlea rdi, [rel str%i]\n", number);
	fprintf(output_file, "\txor rax, rax\n");
	fprintf(output_file, "\tcall printf\n");
	fprintf(output_file, "\n");
}

int cg_add(int r1, int r2)
{
	fprintf(output_file, "\tadd %s, %s\n", reg_list[r1].name, reg_list[r2].name);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_sub(int r1, int r2)
{
	fprintf(output_file, "\tsub %s, %s\n", reg_list[r1].name, reg_list[r2].name);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_mult(int r1, int r2)
{
	fprintf(output_file, "\timul %s, %s\n", reg_list[r1].name, reg_list[r2].name);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_div(int r1, int r2)
{
	fprintf(output_file, "\tmov rax, %s\n", reg_list[r1].name);
	fprintf(output_file, "\tcqo\n");
	fprintf(output_file, "\tidiv %s\n", reg_list[r2].name);
	fprintf(output_file, "\tmov %s, rax\n", reg_list[r1].name);
	fprintf(output_file, "\n");
	free_reg(r2);
	return r1;
}

int cg_load(int value)
{
	int reg_index = alloc_reg();
	fprintf(output_file, "\tmov %s, %i\n", reg_list[reg_index].name, value);
	fprintf(output_file, "\n");
	return reg_index;
}