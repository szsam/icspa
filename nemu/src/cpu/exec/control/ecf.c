#include <setjmp.h>
extern jmp_buf jbuf;

#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "monitor/monitor.h"

void raise_intr(uint8_t NO, uint32_t eflags, uint16_t cs, uint32_t eip) { 
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use `NO' to index the IDT.
	 */
	
	// save eflags, cs and eip on stack
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, eflags, R_SS);
	// Log("eflags=0x%x", eflags);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 2, cs, R_SS);
	// Log("CS=0x%x", cs);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, eip, R_SS);
	// Log("return address = 0x%x", eip);
	
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

	raise_intr(idt_index, cpu.EFLAGS, cpu.cs.val, eip + 2);

	print_asm("int 0x%x", idt_index);
	return 2;
}

make_helper(iret) {
	cpu.eip = swaddr_read(cpu.esp, 4, R_SS);
	// Log("return address = 0x%x", cpu.eip);
	cpu.cs.val = swaddr_read(cpu.esp + 4, 2, R_SS);
	cpu.cs.cache.valid = 0;
	// Log("CS=0x%x", cpu.cs.val);
	cpu.EFLAGS = swaddr_read(cpu.esp + 8, 4, R_SS);
	// Log("eflags=0x%x", cpu.EFLAGS);
	cpu.esp += 12;

	do_not_add_instr_len_to_eip = true;

	print_asm("iret");
	return 1;
}
