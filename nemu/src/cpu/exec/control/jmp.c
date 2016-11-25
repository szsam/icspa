#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jmp-template.h"
#undef DATA_BYTE

/* #define DATA_BYTE 2 */
/* #include "jmp-template.h" */
/* #undef DATA_BYTE */

#define DATA_BYTE 4
#include "jmp-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */
/* make_helper_v(je_si) */

make_helper(ljmp) {
	// jmp intersegment, 6-byte immediate address
	cpu.eip = instr_fetch(eip + 1, 4);
	cpu.cs.val = instr_fetch(eip + 5, 2);
	do_not_add_instr_len_to_eip = true;
	print_asm("ljmp $0x%x,$0x%x", cpu.cs.val, cpu.eip);
	return 7;
}
