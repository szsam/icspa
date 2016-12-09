#include "cpu/helper.h"

make_helper(hlt) {
	while (!(cpu.INTR & cpu.IF));
	return 1;
}
