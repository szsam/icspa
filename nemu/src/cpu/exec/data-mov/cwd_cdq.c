#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(cwd_cdq) {
	if (ops_decoded.is_operand_size_16) {
		// CWD instruction
		cpu.gpr[R_DX]._16 = ((int16_t)cpu.gpr[R_AX]._16 < 0) ? 0xffff : 0x0;
		print_asm("cwtl");
	}
	else {
		// CDQ instruction
		cpu.edx =  ((int32_t)cpu.eax < 0) ? 0xffffffff : 0x0;
		print_asm("cltd");
			
	}

	return 1;
}
