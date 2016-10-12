#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
    uint32_t offset = op_src->val;

	cpu.eip += offset;

	print_asm("jmp %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
