#include "cpu/exec/template-start.h"
#include "cpu/exec/flags.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;

	/* TODO: Update EFLAGS. */
	cpu.OF = cpu.CF = 0;
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);

	cpu.PF = EVEN_PARITY(result);

	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(i2rm)
make_instr_helper(i2a)

#include "cpu/exec/template-end.h"
