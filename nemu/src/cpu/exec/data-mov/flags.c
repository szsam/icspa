#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(std) {
	cpu.DF = 1;
	print_asm("std");
	return 1;
}

make_helper(cld) {
	cpu.DF = 0;
	print_asm("cld");
	return 1;
}

make_helper(cli) {
	cpu.IF = 0;
	print_asm("cli");
	return 1;
}
