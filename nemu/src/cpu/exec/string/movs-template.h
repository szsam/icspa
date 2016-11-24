#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_, SUFFIX)) {
	DATA_TYPE data = MEM_R(cpu.esi, R_ES);
	MEM_W(cpu.edi, data, R_ES);
	int IncDec = cpu.DF ? -DATA_BYTE : DATA_BYTE;
	cpu.esi += IncDec;
	cpu.edi += IncDec;

	print_asm(str(instr) str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
