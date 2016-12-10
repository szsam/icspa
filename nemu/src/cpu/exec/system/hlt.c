#include "cpu/helper.h"
#include "device/i8259.h"

void device_update();
void raise_intr(uint8_t, uint32_t, uint16_t, uint32_t);

make_helper(hlt) {
	while (!(cpu.INTR & cpu.IF)) {
		device_update();
	}

	uint32_t intr_no = i8259_query_intr();
	i8259_ack_intr();
	raise_intr(intr_no, cpu.EFLAGS, cpu.cs.val, cpu.eip);

	panic("control should never reaches here");
	return 1;
}
