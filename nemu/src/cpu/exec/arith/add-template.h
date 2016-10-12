#include "cpu/exec/template-start.h"
#include "cpu/exec/flags.h"

#define instr add

static void do_execute() {
	DATA_TYPE result = op_src->val + op_dest->val;

	cpu.SF = MSB(result);
	cpu.ZF = (result == 0);
	cpu.PF = EVEN_PARITY(result);
    cpu.CF = !UADD_OK(op_dest->val, op_src->val);
    cpu.OF = !TADD_OK(op_dest->val, op_src->val);

	OPERAND_W(op_dest, result);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif


#include "cpu/exec/template-end.h"
