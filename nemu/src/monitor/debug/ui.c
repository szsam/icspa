#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step one instruction exactly", cmd_si },
	{ "info", "Generic command for showing things about the program being debugged", cmd_info },
	{ "x", "Examine memory: x N EXPR", cmd_x },
	{ "p", "Print value of expression EXPR", cmd_p }

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");

	if(arg == NULL) {
		/* no argument given */
		cpu_exec(1);
	}
	else {
		int count = atoi(arg);
		cpu_exec(count);
	}
	return 0;
}


static int cmd_info(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");

	if (!strcmp(arg, "r")) {
		/* list registers and their contents */
		printf("eax\t\t0x%x\t\t%d\n", cpu.eax, cpu.eax);
		printf("ecx\t\t0x%x\t\t%d\n", cpu.ecx, cpu.ecx);
		printf("edx\t\t0x%x\t\t%d\n", cpu.edx, cpu.edx);
		printf("ebx\t\t0x%x\t\t%d\n", cpu.ebx, cpu.ebx);
		printf("esp\t\t0x%x\t\t%d\n", cpu.esp, cpu.esp);
		printf("ebp\t\t0x%x\t\t%d\n", cpu.ebp, cpu.ebp);
		printf("esi\t\t0x%x\t\t%d\n", cpu.esi, cpu.esi);
		printf("edi\t\t0x%x\t\t%d\n", cpu.edi, cpu.edi);
		printf("eip\t\t0x%x\t\t%d\n", cpu.eip, cpu.eip);
	}
	else if (!strcmp(arg, "w")) {
	   //watchpoint
	}

	return 0;
}

static int cmd_x(char *args) {
	/* extract the first two arguments */
	char *arg1 = strtok(NULL, " ");
	char *arg2 = strtok(NULL, " ");
	int num = atoi(arg1);
	swaddr_t addr;
   	sscanf(arg2, "%x", &addr);

	for (int i = 0; i < num; i++) {
		uint32_t data = swaddr_read(addr, 4);

		if (i % 4 == 0)
		{
			printf("0x%x:\t", addr);
		}
		printf("0x%x\t", data);
		if ((i + 1) % 4 == 0)
			printf("\n");

		addr += 4;

	}
	printf("\n");
	return 0;
}

static int cmd_p(char *args) {
	bool success;
	uint32_t res = expr(args, &success);
	
	if (success)
		printf("%u\n", res);
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
