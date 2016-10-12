#include "cpu/exec/template-start.h"

#define instr call

// Actually, only 'call rel32' is implemented correctly
make_helper(concat(call_rel_, SUFFIX)) {
    DATA_TYPE_S offset = instr_fetch(eip + 1, DATA_BYTE);
    cpu.esp -= DATA_BYTE;
    swaddr_write(cpu.esp, DATA_BYTE, eip + DATA_BYTE + 1);
    cpu.eip += (int32_t)offset; 

	print_asm("call %x", eip + DATA_BYTE + 1 + (int32_t)offset);	

	return 1 + DATA_BYTE;
}

#include "cpu/exec/template-end.h"
