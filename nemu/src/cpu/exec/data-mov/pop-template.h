#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
/*
   IF OperandSize = 16
   THEN
     DEST ← (SS:ESP); (* copy a word *)
     ESP ← ESP + 2;
   ELSE (* OperandSize = 32 *)
     DEST ← (SS:ESP); (* copy a dword *)
     ESP ← ESP + 4;
   FI;
*/
//	if (ops_decoded.is_operand_size_16) {
//		swaddr_write(cpu.esp, 2, op_src->val);
//		cpu.esp += 2;
//	}	
//	else {
//		swaddr_write(cpu.esp, 4, op_src->val);
//		cpu.esp += 4;
//	}

	/* the following implementation should also work. */
	MEM_W(cpu.esp, op_src->val);
	cpu.esp += DATA_BYTE;

	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)


#include "cpu/exec/template-end.h"
