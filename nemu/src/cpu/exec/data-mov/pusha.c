#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(pusha) {
	// 16 bit version is not implemented
	assert(!ops_decoded.is_operand_size_16);

	uint32_t temp = cpu.esp;
	for (int ix = R_EAX; ix <= R_EDI; ++ix) {
		cpu.esp -= 4;
		if (ix != R_ESP) {
			swaddr_write(cpu.esp, 4, reg_l(ix), R_SS);
			Log("%s=0x%x", regsl[ix], reg_l(ix));
		}
		else
			swaddr_write(cpu.esp, 4, temp, R_SS);
	}

	print_asm("pusha");
	return 1;
}
