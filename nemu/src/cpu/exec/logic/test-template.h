#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;

	/* TODO: Update EFLAGS. */
	cpu.OF = cpu.CF = 0;
	cpu.SF = (result >> (DATA_BYTE*8-1)) & 0x1;
	cpu.ZF = (result == 0);

	int count = 0;	//the number of '1'
	for (int i = 0; i<DATA_BYTE*8-1; i++) {
		count += result & 0x1;
		result >>= 1;
	}
	cpu.PF = (count % 2 == 0);

	print_asm_template2();
}

/* make_instr_helper(i2a) */
/* make_instr_helper(i2rm) */
/* #if DATA_BYTE == 2 || DATA_BYTE == 4 */
/* make_instr_helper(si2rm) */
/* #endif */
make_instr_helper(r2rm)
/* make_instr_helper(rm2r) */

#include "cpu/exec/template-end.h"
