#include "cpu/exec/template-start.h"
#include "cpu/exec/flags.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;

	/* TODO: Update EFLAGS. */
//	panic("please implement me");
	cpu.OF = !TADD_OK(op_src->val, 1);
	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = EVEN_PARITY(result);

	OPERAND_W(op_src, result);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
