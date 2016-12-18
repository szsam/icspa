#include "cpu/helper.h"
#include "cpu/exec/helper.h"

void device_update();

make_helper(hlt) {
	while (!(cpu.INTR & cpu.IF));

	print_asm("hlt");
	return 1;
}
