#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(popa) {
	// 16 bit version is not implemented
	assert(!ops_decoded.is_operand_size_16);

	for (int ix = R_EDI; ix >= R_EAX; --ix) {
		if (ix != R_ESP) {
			reg_l(ix) = swaddr_read(cpu.esp, 4, R_SS);
			//Log("%s=0x%x", regsl[ix], reg_l(ix));
		}
		// the ESP value is discarded
		cpu.esp += 4;
	}

	print_asm("popa");
	return 1;
}
