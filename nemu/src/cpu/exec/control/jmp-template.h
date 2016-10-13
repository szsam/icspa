#include "cpu/exec/template-start.h"
#include "monitor/monitor.h"

#define instr jmp

static void do_execute() {
	if (op_src->type == OP_TYPE_IMM) {
		uint32_t offset = op_src->val;
		cpu.eip += offset;
		print_asm("jmp %x", cpu.eip + 1 + DATA_BYTE);
	}
	else {
		cpu.eip = op_src->val;
		do_not_add_instr_len_to_eip = true;
		print_asm("jmp *%s", op_src->str);
	}
}

make_instr_helper(si)
#if DATA_BYTE == 4
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"
