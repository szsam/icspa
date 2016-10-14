#include "cpu/exec/template-start.h"

// #define instr call

make_helper(call_si_l) {
	int len = decode_si_l(eip+1);

	uint32_t offset = op_src->val;

    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip + 4 + 1);

    cpu.eip += offset; 

	print_asm("call %x", cpu.eip + 4 + 1);	

	return 1+len;
}

make_helper(call_rm_l) {
	int len = decode_rm_l(eip+1);

    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip + len + 1);

    cpu.eip = op_src->val; 

	print_asm("call *%s", op_src->str);	

	return 1+len;
}

//make_instr_helper(si)
//#if DATA_BYTE == 4
//make_instr_helper(rm)
//#endif

#include "cpu/exec/template-end.h"
