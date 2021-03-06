#include "cpu/exec/template-start.h"
#include "device/port-io.h"

#define instr in

make_helper(concat(in_i2a_, SUFFIX)) {
	ioaddr_t addr = instr_fetch(eip + 1, 1);
	REG(R_EAX) = pio_read(addr, DATA_BYTE);

	print_asm("in 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 2;
}

make_helper(concat(in_d2a_, SUFFIX)) {
	ioaddr_t addr = reg_w(R_EDX);	// DX
	REG(R_EAX) = pio_read(addr, DATA_BYTE);

	print_asm("in (%%dx),%%%s", REG_NAME(R_EAX));
	return 1;
}


#include "cpu/exec/template-end.h"
