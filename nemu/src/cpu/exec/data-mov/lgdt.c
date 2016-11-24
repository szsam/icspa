#include "cpu/helper.h"
#include "cpu/exec/helper.h"

/* By KISS rule, we assume ModR/M byte is 0x15,
 * followed by a disp32, and OperandSize = 32 */
make_helper(lgdt) {
	swaddr_t addr = instr_fetch(eip + 3, 4);
	cpu.gdtr.limit = swaddr_read(addr, 2);
	cpu.gdtr.base = swaddr_read(addr + 2, 4);
	print_asm("lgdt 0x%x", addr);
	return 7;
}
