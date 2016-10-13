#include "cpu/exec/template-start.h"

#define instr sete

static void do_execute() {
	OPERAND_W(op_src, cpu.ZF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr
	

#define instr setne

static void do_execute() {
	OPERAND_W(op_src, !cpu.ZF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr
	
#define instr seta

static void do_execute() {
	OPERAND_W(op_src, cpu.CF == 0 && cpu.ZF == 0);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr

	
#define instr setae

static void do_execute() {
	OPERAND_W(op_src, cpu.CF == 0);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr

	
#define instr setb

static void do_execute() {
	OPERAND_W(op_src, cpu.CF == 1);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr

	
#define instr setbe

static void do_execute() {
	OPERAND_W(op_src, cpu.CF || cpu.ZF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr


#define instr setg

static void do_execute() {
	OPERAND_W(op_src, !cpu.ZF && cpu.SF == cpu.OF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr


#define instr setge

static void do_execute() {
	OPERAND_W(op_src, cpu.SF == cpu.OF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr

#define instr setl

static void do_execute() {
	OPERAND_W(op_src, cpu.SF != cpu.OF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr


#define instr setle

static void do_execute() {
	OPERAND_W(op_src, cpu.ZF || cpu.SF != cpu.OF);
	print_asm(str(instr)" %s", op_src->str);
}

make_instr_helper(rm)

#undef instr

#include "cpu/exec/template-end.h"
