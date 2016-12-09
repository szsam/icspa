#include "cpu/exec/template-start.h"
#include "device/port-io.h"

#define instr out

make_helper(concat(out_a2i_, SUFFIX)) {
	ioaddr_t addr = instr_fetch(eip + 1, 1);
	pio_write(addr, DATA_BYTE, REG(R_EAX));

	print_asm("out %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 2;
}

make_helper(concat(out_a2d_, SUFFIX)) {
	ioaddr_t addr = reg_w(R_EDX);	// DX
	pio_write(addr, DATA_BYTE, REG(R_EAX));

	print_asm("out %%%s,(%%dx)", REG_NAME(R_EAX));
	return 1;
}


#include "cpu/exec/template-end.h"
