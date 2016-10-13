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
