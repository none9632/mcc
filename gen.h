#ifndef GEN_H
#define GEN_H

#define COMMANDSIZE 8192

#define CPLUS -1
#define CMINUS -2
#define CMULT -3
#define CDIV -4
#define CMOD -5
#define CMORE -6
#define CMOREEQ -7
#define CLESS -8
#define CLESSEQ -9
#define CEQUAL -10
#define CNOTEQ -11
#define CMINONE -12
#define CPLUSONE -13
#define CPRINT -14
#define CSCAN -15
#define CLOAD -16
#define CSTOP -17
#define CNONE -18
#define CPRINTS -19
#define CNONEP -20
#define CNEGP -21
#define CPLUSA -22
#define CMINUSA -23
#define CMULTA -24
#define CDIVA -25
#define CMODA -26
#define CASSIGN -27

typedef struct Command {
	int command;
	int var;
} Commands;

extern Commands *commands;
extern count;

void gen(int command, int var);

#endif