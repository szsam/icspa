#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_, SUFFIX)) {
	MEM_W(cpu.edi, REG(R_EAX), R_ES);
	int IncDec = cpu.DF ? -DATA_BYTE : DATA_BYTE;
	cpu.edi += IncDec;

	print_asm(str(instr) str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
