#include "../include/node.h"

// number of spaces or '|' to be printed
static int column = 0;

/*
 * Outputs spaces and '|'
 * @param count_bar count vertical bar
 */
static void print_spaces(int count_bar)
{
	for (int i = 0; i < column; i++)
	{
		if (i % 4 == 0)
		{
			if (count_bar > 0)
			{
				printf("│");
				count_bar--;
			}
			else
				printf(" ");
		}
		else
			printf(" ");
	}
}

/*
 * Outputs the node diagram to the console
 * @param n         the node that will be displayed
 * @param count_bar count vertical bar
 */
void output_node(Node *n, int count_bar)
{
	if (n->kind == K_ADD  || n->kind == K_SUB ||
		n->kind == K_MULT || n->kind == K_DIV)
	{
		switch (n->kind)
		{
			case K_ADD:  printf("+\n"); break;
			case K_SUB:  printf("-\n"); break;
			case K_MULT: printf("*\n"); break;
			case K_DIV:  printf("/\n"); break;

		}

		print_spaces(count_bar);
		printf("├── ");
		column += 4;
		output_node(n->n1, count_bar + 1);

		print_spaces(count_bar);
		printf("└── ");
		column += 4;
		output_node(n->n2, count_bar);
	}
	else if (n->kind == K_NUM)
		printf("%i\n", n->value);
	else if (n->kind == K_NONE)
		printf("none\n");

	column -= 4;
}
