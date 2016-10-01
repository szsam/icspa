#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	uint32_t temp = swaddr_read(cpu.esp, DATA_BYTE);
	OPERAND_W(op_src, temp);
	cpu.esp += DATA_BYTE;

	print_asm_template1();
}

make_instr_helper(r)


#include "cpu/exec/template-end.h"
