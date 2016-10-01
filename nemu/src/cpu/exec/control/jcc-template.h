#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	/* DATA_TYPE result = ~op_src->val; */
	/* OPERAND_W(op_src, result); */
    uint32_t offset = op_src->val;

	if (cpu.ZF) {
		cpu.eip += offset;
	}
	print_asm_template1();
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
