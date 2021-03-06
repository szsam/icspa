#include "cpu/decode/modrm.h"
#include "memory/tlb.h"
#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX), R_DS);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr, R_DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE == 4
make_helper(mov_cr2r) {
	uint8_t temp = instr_fetch(eip + 1, 1);
	ModR_M modrm;
	modrm.val = temp;

	uint32_t control_reg;
	switch (modrm.reg) {
		case 0: control_reg = cpu.cr0.val; break;
		case 3: control_reg = cpu.cr3.val; break;
		default: assert(0);
	}

	reg_l(modrm.R_M) = control_reg;

	print_asm("mov %%cr%d,%%%s", modrm.reg, regsl[modrm.R_M]);
	return 2;
}

make_helper(mov_r2cr) {
	uint8_t temp = instr_fetch(eip + 1, 1);
	ModR_M modrm;
	modrm.val = temp;

	switch (modrm.reg) {
		case 0: cpu.cr0.val = reg_l(modrm.R_M); break;
		case 3: 
			cpu.cr3.val = reg_l(modrm.R_M);
			tlb.flush(&tlb);
			break;
		default: assert(0);
	}

	print_asm("mov %%%s,%%cr%d", regsl[modrm.R_M], modrm.reg);
	return 2;
}
#endif

#if DATA_BYTE == 2
make_helper(mov_rm2sr) {
	op_src->size = 2;
	int len = read_ModR_M(eip + 1, op_src, op_dest);
	seg_reg(op_dest->reg).val = (uint16_t)op_src->val;
	seg_reg(op_dest->reg).cache.valid = 0;
	print_asm("mov to segment register");
	return 1 + len;
}

#endif

#include "cpu/exec/template-end.h"
