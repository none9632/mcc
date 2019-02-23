#ifndef GEN_H
#define GEN_H

typedef struct Command {
	int command;
	int *var;
} Commands;

extern Commands *commands;
extern int cGen;

void gen(int command, int *var);

#endif
