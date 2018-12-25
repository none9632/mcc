#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "launch.h"
#include "gen.h"
#include "launch.h"

int cLaunch = 0;  // counter in file launch
int bufferCount;

// executes commands from the list of commands
int *CDriver(Commands command, int *var) {
	switch (command.command) {
	case CNONE:
		if (var != NULL)
			*var = command.var;
		break;
	case CNONEP:
		if (var != NULL)
			*var = *command.var;
		break;
	case CNEG:
		if (var != NULL) {
			int *point = malloc(sizeof(int));
			CDriver(commands[--bufferCount], point);
			*var = -*point;
		}
		break;
	case CNEGP:
		if (var != NULL)
			*var = -*command.var;
		break;
	case CPLUS:
		if (var != NULL) {
			int bufferVar;
			CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			CDriver(commands[--bufferCount], var);
			*var += bufferVar;
		}
		break;
	case CMINUS:
		if (var != NULL) {
			int bufferVar;
			CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			CDriver(commands[--bufferCount], var);
			*var -= bufferVar;
		}
		break;
	case CMULT:
		if (var != NULL) {
			int bufferVar;
			CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			CDriver(commands[--bufferCount], var);
			*var *= bufferVar;
		}
		break;
	case CDIV:
		if (var != NULL) {
			int bufferVar;
			CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			CDriver(commands[--bufferCount], var);
			*var /= bufferVar;
		}
		break;
	case CMOD:
		if (var != NULL) {
			int bufferVar;
			CDriver(commands[--bufferCount], var);
			bufferVar = *var;
			CDriver(commands[--bufferCount], var);
			*var %= bufferVar;
		}
		break;
	case CJUMP: {
		cLaunch = *command.var;
		break;
	}
	case CIF: {
		int saveCommand = commands[--bufferCount].command,
			*point = malloc(sizeof(int)),
			*bufferPoint = malloc(sizeof(int)),
			buffer = 0;
		CDriver(commands[--bufferCount], point);
		CDriver(commands[--bufferCount], bufferPoint);
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
			*point = malloc(sizeof(int)),
			*bufferPoint = malloc(sizeof(int));
		CDriver(commands[--bufferCount], point);
		CDriver(commands[--bufferCount], bufferPoint);
		switch (saveCommand) {
		case CEQUAL:
			if (*bufferPoint == *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CNOTEQ:
			if (*bufferPoint != *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CMORE:
			if (*bufferPoint > *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CLESS:
			if (*bufferPoint < *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CMOREEQ:
			if (*bufferPoint >= *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		case CLESSEQ:
			if (*bufferPoint <= *point) {
				cLaunch += 2;
				launching();
				cLaunch = command.var;
			}
			break;
		}
		break;
	}
	case CPRINT:
		if (command.var == NULL)
			error("syntax error");
		printf("%d", *command.var);
		break;
	case CPRINTN: {
		int *point = malloc(sizeof(double));
		CDriver(commands[--bufferCount], point);
		printf("%d", *point);
		break;
	}
	case CPRINTS:
		printf("%s", command.var);
		break;
	case CPRINTLN:
		printf("\n");
		break;
	case CINPUT:
		scanf("%d", command.var);
		break;
	case CLOAD: {
		int *point = malloc(sizeof(double));
		CDriver(commands[--bufferCount], point);
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
		free(point);
		break;
	}
	}
	if (var == NULL)
		return 0;
	return *var;
}

// runs command execution
void launching() {
	while (commands[cLaunch].command != CSTOP) {
		bufferCount = cLaunch;
		CDriver(commands[cLaunch], NULL);
		cLaunch++;
	}
}
