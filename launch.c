#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "launch.h"
#include "gen.h"
#include "error.h"

int cLaunch = 0;  // counter in file launch
int bufferCount;

// executes commands from the list of commands
int *CDriver(Commands command, int bufferBit) {
	int *var;
	switch (command.command) {
	case CNONE:
		if (bufferBit == 1)
			var = command.var;
		break;
	case CNONEP:
		if (bufferBit == 1)
			var = command.var;
		break;
	case CNEG:
		if (bufferBit == 1) {
			int *point = CDriver(commands[--bufferCount], 1);
			*var = -*point;
		}
		break;
	case CNEGP:
		if (bufferBit == 1)
			*var = -*command.var;
		break;
	case CPLUS:
		if (bufferBit == 1) {
			int bufferVar;
			var = CDriver(commands[--bufferCount], 1);
			bufferVar = *var;
			var = CDriver(commands[--bufferCount], 1);
			*var += bufferVar;
		}
		break;
	case CMINUS:
		if (bufferBit == 1) {
			int bufferVar;
			var = CDriver(commands[--bufferCount], 1);
			bufferVar = *var;
			var = CDriver(commands[--bufferCount], 1);
			*var -= bufferVar;
		}
		break;
	case CMULT:
		if (bufferBit == 1) {
			int bufferVar;
			var = CDriver(commands[--bufferCount], 1);
			bufferVar = *var;
			var = CDriver(commands[--bufferCount], 1);
			*var *= bufferVar;
		}
		break;
	case CDIV:
		if (bufferBit == 1) {
			int bufferVar;
			var = CDriver(commands[--bufferCount], 1);
			bufferVar = *var;
			var = CDriver(commands[--bufferCount], 1);
			*var /= bufferVar;
		}
		break;
	case CMOD:
		if (bufferBit == 1) {
			int bufferVar;
			var = CDriver(commands[--bufferCount], 1);
			bufferVar = *var;
			var = CDriver(commands[--bufferCount], 1);
			*var %= bufferVar;
		}
		break;
	case CJUMP: {
		cLaunch = *command.var;
		break;
	}
	case CIF: {
		int saveCommand = commands[--bufferCount].command,
			*point = CDriver(commands[--bufferCount], 1),
			*bufferPoint = CDriver(commands[--bufferCount], 1),
			buffer = 0;
		cLaunch++;
		switch (saveCommand) {
		case CEQUAL:
			if (*bufferPoint == *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CNOTEQ:
			if (*bufferPoint != *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CMORE:
			if (*bufferPoint > *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CLESS:
			if (*bufferPoint < *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CMOREEQ:
			if (*bufferPoint >= *point)
				cLaunch++;
			else
				buffer++;
			break;
		case CLESSEQ:
			if (*bufferPoint <= *point)
				cLaunch++;
			else
				buffer++;
			break;
		}
		launching();
		if (commands[++cLaunch].command == CELSE) {
			if (buffer == 1)
				cLaunch += 2;
			else
				cLaunch++;
			launching();
		}
		else
			cLaunch--;
		break;
	}
	case CWHILE: {
		int saveCommand = commands[--bufferCount].command,
			*point = CDriver(commands[--bufferCount], 1),
			*bufferPoint = CDriver(commands[--bufferCount], 1);
		switch (saveCommand) {
		case CEQUAL:
			if (*bufferPoint == *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		case CNOTEQ:
			if (*bufferPoint != *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		case CMORE:
			if (*bufferPoint > *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		case CLESS:
			if (*bufferPoint < *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		case CMOREEQ:
			if (*bufferPoint >= *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		case CLESSEQ:
			if (*bufferPoint <= *point) {
				cLaunch += 2;
				launching();
				cLaunch = *command.var;
			}
			break;
		}
		break;
	}
	case CPRINT:
		if (command.var == NULL)
			error("syntax error", 0);
		printf("%d", *command.var);
		break;
	case CPRINTN: {
		int *point = CDriver(commands[--bufferCount], 1);
		printf("%d", *point);
		break;
	}
	case CPRINTS:
		printf("%ls", command.var);
		break;
	case CPRINTLN:
		printf("\n");
		break;
	case CINPUT:
		scanf("%d", command.var);
		break;
	case CLOAD: {
		int *point = CDriver(commands[--bufferCount], 1);
		bufferCount = cLaunch;
		switch (commands[++bufferCount].command) {
		case CPLUSA:
			*command.var += *point;
			break;
		case CMINUSA:
			*command.var -= *point;
			break;
		case CMULTA:
			*command.var *= *point;
			break;
		case CDIVA:
			*command.var /= *point;
			break;
		case CMODA:
			*command.var %= *point;
			break;
		case CASSIGN:
			*command.var = *point;
			break;
		}
		break;
	}
	}
	return var;
}

void printCommands() {
	int b = 0;
	while (commands[b].command != CSTOP) {
		switch (commands[b].command) {
			case CPLUS:
				printf("cplus - %p\n", commands[b].var);
				break;
			case CMINUS:
				printf("cminus - %p\n", commands[b].var);
				break;
			case CMULT:
				printf("cmult - %p\n", commands[b].var);
				break;
			case CDIV:
				printf("cdiv - %p\n", commands[b].var);
				break;
			case CMOD:
				printf("cmod - %p\n", commands[b].var);
				break;
			case CIF:
				printf("cif - %p\n", commands[b].var);
				break;
			case CWHILE:
				printf("cwhile - %p\n", *commands[b].var);
				break;
			case CPRINT:
				printf("cprint - %p\n", commands[b].var);
				break;
			case CPRINTN:
				printf("cprintn - %p\n", commands[b].var);
				break;
			case CPRINTS:
				printf("cprints - %p\n", commands[b].var);
				break;
			case CPRINTLN:
				printf("cprintln - %p\n", commands[b].var);
				break;
			case CSTOP:
				printf("cstop - %p\n", commands[b].var);
				break;
			case CDOUBLE:
				printf("cdouble - %p\n", commands[b].var);
				break;
			case CNONE:
				printf("cnone - %p\n", commands[b].var);
				break;
			case CNEG:
				printf("cneg - %p\n", commands[b].var);
				break;
			case CNONEP:
				printf("cnonep - %p\n", commands[b].var);
				break;
			case CINPUT:
				printf("cinput - %p\n", commands[b].var);
				break;
			case CPLUSA:
				printf("cplusa - %p\n", commands[b].var);
				break;
			case CMINUSA:
				printf("cminusa - %p\n", commands[b].var);
				break;
			case CMULTA:
				printf("cmulta - %p\n", commands[b].var);
				break;
			case CDIVA:
				printf("cdiva - %p\n", commands[b].var);
				break;
			case CMODA:
				printf("cmoda - %p\n", commands[b].var);
				break;
			case CASSIGN:
				printf("cassign - %p\n", commands[b].var);
				break;
			case CLOAD:
				printf("cload - %p\n", commands[b].var);
				break;
			case CEQUAL:
				printf("cequal - %p\n", commands[b].var);
				break;
			case CNOTEQ:
				printf("cnoteq - %p\n", commands[b].var);
				break;
			case CMORE:
				printf("cmore - %p\n", commands[b].var);
				break;
			case CLESS:
				printf("cless - %p\n", commands[b].var);
				break;
			case CMOREEQ:
				printf("cmoreeq - %p\n", commands[b].var);
				break;
			case CLESSEQ:
				printf("clesseq - %p\n", commands[b].var);
				break;
			case CJUMP:
				printf("cjump - %p\n", commands[b].var);
				break;
			case CNEGP:
				printf("cnegp - %p\n", commands[b].var);
				break;
			case CELSE:
				printf("celse - %p\n", commands[b].var);
				break;
		}
		b++;
	}
}

// runs command execution
void launching() {
	while (commands[cLaunch].command != CSTOP) {
		bufferCount = cLaunch;
		CDriver(commands[cLaunch], 0);
		cLaunch++;
	}
}
