#ifndef GEN_H
#define GEN_H

#define COMMANDSIZE 8192

#define CPLUS 1
#define CMINUS 2
#define CMULT 3
#define CDIV 4
#define CMOD 5
#define CMORE 6
#define CMOREEQ 7
#define CLESS 8
#define CLESSEQ 9
#define CEQUAL 10
#define CNOTEQ 11
#define CPRINTS 12
#define CPRINTN 13
#define CPRINT 14
#define CSCAN 15
#define CLOAD 16
#define CSTOP 17
#define CNONE 18
#define CNONEP 20
#define CNEGP 21
#define CPLUSA 22
#define CMINUSA 23
#define CMULTA 24
#define CDIVA 25
#define CMODA 26
#define CASSIGN 27
#define CNEG 28
#define CIF 29
#define CELSE 30
#define CWHILE 31
#define CJUMP 32
#define CEND 33

typedef struct Command {
	int command;
	int var;
} Commands;

extern Commands *commands;
extern cGen;

void gen(int command, int var);

#endif