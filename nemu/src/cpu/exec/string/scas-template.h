#include "cpu/exec/template-start.h"
#include "cpu/exec/flags.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
	DATA_TYPE data = MEM_R(cpu.edi);
	DATA_TYPE result = REG(R_EAX) - data;
	
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = EVEN_PARITY(result);
	cpu.CF = !USUB_OK(REG(R_EAX), data);
	cpu.OF = !TSUB_OK(REG(R_EAX), data);

	int IncDec = cpu.DF ? -DATA_BYTE : DATA_BYTE;
	cpu.edi += IncDec;

	print_asm(str(instr) str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
