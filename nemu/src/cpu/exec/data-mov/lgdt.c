#include "cpu/helper.h"
#include "cpu/exec/helper.h"

/* By KISS rule, we assume ModR/M byte is 0x15,
 * followed by a disp32, and OperandSize = 32 */
make_helper(lgdt) {
	uint8_t modrm = instr_fetch(eip + 1, 1);
	assert(modrm == 0x15);

	swaddr_t addr = instr_fetch(eip + 2, 4);
	cpu.gdtr.limit = swaddr_read(addr, 2);
	cpu.gdtr.base = swaddr_read(addr + 2, 4);
	print_asm("lgdt 0x%x", addr);
	return 6;
}
