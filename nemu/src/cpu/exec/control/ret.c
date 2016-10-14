#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "monitor/monitor.h"

make_helper(ret) {
	swaddr_t ret_addr = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	cpu.eip = ret_addr;
	do_not_add_instr_len_to_eip = true;	

	print_asm("ret");
	return 1;
}

make_helper(ret_imm16) {
	// eip <- pop()
	swaddr_t ret_addr = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	cpu.eip = ret_addr;
	// esp = esp + imm16
	uint32_t num = instr_fetch(eip + 1, 2);
	cpu.esp += num;

	do_not_add_instr_len_to_eip = true;	

	print_asm("ret $%d", num);
	return 3;
}
