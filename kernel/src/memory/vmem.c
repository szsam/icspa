#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_VMEM_PTE ((SCR_SIZE + PAGE_SIZE) / PAGE_SIZE)
#define PTBL_INDEX_START ((VMEM_ADDR >> 12) & 0x3ff)

/* Use the function to get the start address of user page directory. */
PDE* get_updir();

static PTE uptable[NR_PTE] align_to_page;

void create_video_mapping() {
	/* TODO: create an identical mapping from virtual memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	// panic("please implement me");
	PDE *updir = get_updir();
	updir[VMEM_ADDR / PT_SIZE].val = make_pde(va_to_pa(uptable));
	assert(updir[0].present);
	
	uint32_t pframe_addr = VMEM_ADDR;
	for (int ix = PTBL_INDEX_START; 
			ix < PTBL_INDEX_START + NR_VMEM_PTE; ++ix) {
		uptable[ix].val = make_pte(pframe_addr);
		assert(uptable[ix].present);
		pframe_addr += PAGE_SIZE;
	}
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	//set_bp();
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
	//set_bp();
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

