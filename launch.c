#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "vector.h"
#include "pars.h"
#include "launch.h"

Vector *commands;
int count_com;

struct double_value
{
	int value1;
	int value2;
};

typedef struct double_value Double_value;

static int expr_processing();
static void VM();

static Double_value get_val()
{
	Double_value dv;

	dv.value2 = expr_processing(commands->data[--count_com]);
	dv.value1 = expr_processing(commands->data[--count_com]);

	return dv;
}

static int expr_processing(Command *c)
{
	int value;
	Double_value dv;

	if (c->command == CM_GET)
		value = c->value;
	else if (c->command == CM_GET_TABLE)
		value = c->table_TN->value;
	else if (c->command == CM_NEG)
		value = -expr_processing(commands->data[--count_com]);
	else
	{
		dv = get_val();

		switch (c->command) {
		case CM_PLUS:   value = dv.value1 + dv.value2;  break;
		case CM_MINUS:  value = dv.value1 - dv.value2;  break;
		case CM_MULT:   value = dv.value1 * dv.value2;  break;
		case CM_DIV:    value = dv.value1 / dv.value2;  break;
		case CM_MOD:    value = dv.value1 % dv.value2;  break;
		case CM_EQUAL:  value = dv.value1 == dv.value2; break;
		case CM_MORE:   value = dv.value1 > dv.value2;  break;
		case CM_LESS:   value = dv.value1 < dv.value2;  break;
		case CM_MOREEQ: value = dv.value1 >= dv.value2; break;
		case CM_LESSEQ: value = dv.value1 <= dv.value2; break;
		case CM_AND:    value = dv.value1 * dv.value2;  break;
		case CM_OR:
			value = dv.value1 + dv.value2;
			if (value != 0)
				value = 1;
			break;
		}
	}

	return value;
}

static int CDriver(Command *c)
{
	int buffer_count = count_com;
	int value;

	if (c->command == CM_IF) {
		do
		{
			buffer_count = count_com;
			if (c->command == CM_ELSE)
				value = 1;
			else
			{
				value = expr_processing(commands->data[--count_com]);
				count_com = buffer_count;
			}

			if (value >= 1)
			{
				++count_com;

				VM(CM_STOP_IF);

				while (c->command != CM_END_IF)
					c = commands->data[++count_com];
			}
			else
			{
				c = commands->data[++count_com];

				while (c->command != CM_ELSE_IF && c->command != CM_ELSE && c->command != CM_END_IF)
					c = commands->data[++count_com];
			}
		} while (c->command == CM_ELSE_IF || c->command == CM_ELSE);
	}
	else if (c->command == CM_WHILE) {
		value = expr_processing(commands->data[--count_com]);
		count_com = buffer_count;

		while (value >= 1)
		{
			++count_com;

			VM(CM_STOP_WHILE);

			count_com = buffer_count;
			value = expr_processing(commands->data[--count_com]);
			count_com = buffer_count;
		}

		while (c->command != CM_STOP_WHILE)
			c = commands->data[++count_com];
	}
	else if (c->command == CM_DO){
		do {
			count_com = buffer_count;
			++count_com;

			VM(CM_STOP_WHILE);

			c = commands->data[count_com];
			while (c->command != CM_WHILE)
				c = commands->data[++count_com];

			value = expr_processing(commands->data[--count_com]);
		} while (value >= 1);

		c = commands->data[count_com];
		while (c->command != CM_WHILE)
			c = commands->data[++count_com];
	}
	else if (c->command == CM_PRINT) {
		printf("%i", expr_processing(commands->data[--count_com]));
		count_com = buffer_count;
	}
	else if (c->command == CM_PRINTS) {
		printf("%s", c->data);
	}
	else if (c->command == CM_INPUT) {
		scanf("%i", &c->table_TN->value);
	}
	else if (c->command == CM_STORE) {
		value = c->table_TN->value;
		Command *buf_c = commands->data[--count_com];
		c = commands->data[--count_com];

		switch (buf_c->command)
		{
		case CM_PLUSA:  value += expr_processing(c); break;
		case CM_MINUSA: value -= expr_processing(c); break;
		case CM_MULTA:  value *= expr_processing(c); break;
		case CM_DIVA:   value /= expr_processing(c); break;
		case CM_MODA:   value %= expr_processing(c); break;
		case CM_ASSIGN: value = expr_processing(c);  break;
		}

		c = commands->data[buffer_count];
		c->table_TN->value = value;
		count_com = buffer_count;
	}
}

static void VM(int stop)
{
	Command *c = commands->data[count_com];

	while (c->command != stop)
	{
		CDriver(c);
		c = commands->data[++count_com];
	}
}

void launching_VM(Vector *_commands)
{
	commands = _commands;
	count_com = 0;

	VM(CM_STOP);
}