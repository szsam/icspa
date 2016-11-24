#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	if (ops_decoded.is_operand_size_16) {
		cpu.esp -= 2;
		swaddr_write(cpu.esp, 2, op_src->val, R_SS);
	}
	else {
		cpu.esp -= 4;
		swaddr_write(cpu.esp, 4, op_src->val, R_SS);
	}
	print_asm_template1();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)
#endif

#if DATA_BYTE == 1
make_instr_helper(si)
#endif


#include "cpu/exec/template-end.h"
