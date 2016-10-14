#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.ZF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr
	

#define instr jne

static void do_execute() {
    uint32_t offset = op_src->val;

	if (!cpu.ZF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr
	
#define instr ja

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.CF == 0 && cpu.ZF == 0) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr

	
#define instr jae

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.CF == 0) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr

	
#define instr jb

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.CF == 1) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr

	
#define instr jbe

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.CF || cpu.ZF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr


#define instr jg

static void do_execute() {
    uint32_t offset = op_src->val;

	if (!cpu.ZF && cpu.SF == cpu.OF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr


#define instr jge

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.SF == cpu.OF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr

#define instr jl

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.SF != cpu.OF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr


#define instr jle

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.ZF || cpu.SF != cpu.OF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr


#define instr js

static void do_execute() {
    uint32_t offset = op_src->val;

	if (cpu.SF) {
		cpu.eip += offset;
	}
	print_asm(str(instr)" %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(si)

#undef instr

#include "cpu/exec/template-end.h"
