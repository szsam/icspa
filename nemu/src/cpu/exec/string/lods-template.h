#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
	DATA_TYPE data = MEM_R(cpu.esi, R_ES);
	REG(R_EAX) = data;
	int IncDec = cpu.DF ? -DATA_BYTE : DATA_BYTE;
	cpu.esi += IncDec;

	print_asm(str(instr) str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
