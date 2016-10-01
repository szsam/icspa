#include "cpu/helper.h"
#include "cpu/exec/helper.h"

make_helper(ret) {
	swaddr_t ret_addr = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	cpu.eip = ret_addr;

	print_asm_template1();
	return 0;
}
