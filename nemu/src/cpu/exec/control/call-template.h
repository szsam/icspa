#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	/* DATA_TYPE result = ~op_src->val; */
	/* OPERAND_W(op_src, result); */
    DATA_TYPE_S offset = op_src->simm;
    cpu.esp -= DATA_BYTE;
    swaddr_write(cpu.esp, DATA_BYTE, cpu.eip + DATA_BYTE);
    cpu.eip += offset; 
	print_asm_template1();
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
