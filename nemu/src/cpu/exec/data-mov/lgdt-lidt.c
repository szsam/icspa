#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(lgdt) {
//	uint8_t modrm = instr_fetch(eip + 1, 1);
//	assert(modrm == 0x15);
	int len = decode_rm_w(eip + 1);

	assert(op_src->type == OP_TYPE_MEM);
	swaddr_t addr = op_src->addr;
	cpu.gdtr.limit = op_src->val;
	cpu.gdtr.base = swaddr_read(addr + 2, 4, op_src->sreg);

	// Log("gdtr.limit=%d, base=0x%x", cpu.gdtr.limit, cpu.gdtr.base);

	print_asm("lgdt %s", op_src->str);
	return len + 1;
}

make_helper(lidt) {
	int len = decode_rm_w(eip + 1);

	assert(op_src->type == OP_TYPE_MEM);
	swaddr_t addr = op_src->addr;
	cpu.idtr.limit = op_src->val;
	cpu.idtr.base = swaddr_read(addr + 2, 4, op_src->sreg);

	print_asm("lidt %s", op_src->str);
	return len + 1;
}
