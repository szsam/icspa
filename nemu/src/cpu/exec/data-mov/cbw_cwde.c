#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(cbw_cwde) {
	if (ops_decoded.is_operand_size_16) {
		// CBW instruction
		cpu.gpr[R_AX]._16 = (int16_t)(int8_t)cpu.gpr[R_AL]._8[0];
	}
	else {
		// CWDE instruction
		cpu.gpr[R_EAX]._32 = (int32_t)(int16_t)cpu.gpr[R_AX]._16;
	}

	return 1;
}
