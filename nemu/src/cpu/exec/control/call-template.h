#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	uint32_t offset = op_src->val;

    cpu.esp -= DATA_BYTE;
    swaddr_write(cpu.esp, DATA_BYTE, cpu.eip + DATA_BYTE + 1);

    cpu.eip += offset; 

	print_asm("call %x", cpu.eip + DATA_BYTE + 1);	

}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
