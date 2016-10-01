#include "cpu/helper.h"

make_helper(ret) {
	swaddr_t ret_addr = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	cpu.eip = ret_addr;
	return 0;
}
