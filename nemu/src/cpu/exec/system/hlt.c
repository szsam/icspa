#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "device/i8259.h"

void device_update();
void raise_intr(uint8_t, uint32_t, uint16_t, uint32_t);

make_helper(hlt) {
	while (!(cpu.INTR & cpu.IF)) {
		device_update();
	}

	print_asm("hlt");
	return 1;
}
