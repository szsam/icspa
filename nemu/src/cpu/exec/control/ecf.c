#include <setjmp.h>
extern jmp_buf jbuf;

#include "cpu/helper.h"
#include "cpu/exec/helper.h"

void raise_intr(uint8_t NO) { 
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use `NO' to index the IDT.
	 */
	
	// find the gate descriptor
	lnaddr_t gateDesc_addr = cpu.idtr.base + NO * sizeof(GateDesc);
	assert((NO + 1) * sizeof(GateDesc) - 1 <= cpu.idtr.limit);
	GateDesc theDesc;
	uint32_t temp;
	temp = lnaddr_read(gateDesc_addr, 4);
	memcpy(&theDesc, &temp, 4);
	temp = lnaddr_read(gateDesc_addr + 4, 4);
	memcpy((void *)&theDesc + 4, &temp, 4);

	// load the selector field into CS register
	cpu.cs.val = theDesc.segment;
	cpu.cs.cache.valid = 0;
	
	// get the address of entry point
	swaddr_t offset = (theDesc.offset_31_16 << 16) + theDesc.offset_15_0;
	cpu.eip = offset;

	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);

}

make_helper(interrupt) {
	size_t idt_index = instr_fetch(eip + 1, 1);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.EFLAGS, R_SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, cpu.cs.val, R_SS);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, eip + 2, R_SS);

	raise_intr(idt_index);

	print_asm("int 0x%x", idt_index);
	return 2;
}

make_helper(iret) {
	cpu.eip = swaddr_read(cpu.esp, 4, R_SS);
	cpu.cs.val = swaddr_read(cpu.esp + 4, 4, R_SS);
	cpu.cs.cache.valid = 0;
	cpu.EFLAGS = swaddr_read(cpu.esp + 8, 4, R_SS);
	cpu.esp += 12;

	print_asm("iret");
	return 1;
}
