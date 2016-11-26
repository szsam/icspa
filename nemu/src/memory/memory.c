#include "common.h"

#include "memory/cache-l1.h"
#include "memory/cache-template-end.h"
#include "cpu/reg.h"

extern Cache_level1 cache_l1;

//uint32_t dram_read(hwaddr_t, size_t);
//void dram_write(hwaddr_t, size_t, uint32_t);


/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	 // return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	 return cache_l1.read(&cache_l1, addr, len);
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// dram_write(addr, len, data);
	cache_l1.write(&cache_l1, addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	assert(seg_reg(sreg).table_indicator == 0);
	assert(seg_reg(sreg).index != 0);
	// sizeof a segment descriptor is 64-bit
	lnaddr_t desc_add = cpu.gdtr.base + 8 * seg_reg(sreg).index;
	SegDesc theDesc;

	assert(desc_add + 7 <= cpu.gdtr.limit);
	// read the descriptor
	uint32_t temp;
	temp = lnaddr_read(desc_add, 4);
	memcpy(&theDesc, &temp, 4);
	temp = lnaddr_read(desc_add + 4, 4);
	memcpy((void *)&theDesc + 4, &temp, 4);

	lnaddr_t seg_base_add = 
	(((theDesc.base_31_24 << 8) + theDesc.base_23_16) << 16 ) + theDesc.base_15_0;

	uint32_t seg_limit = (theDesc.limit_19_16 << 4) + theDesc.limit_15_0;
	if (theDesc.granularity) { 
		// the limit is interpreted in units of 4 Kilobytes
		seg_limit <<= 12;
		seg_limit |= 0xfff;
	}
	assert(addr + len <= seg_limit);

	assert(theDesc.present);

	return seg_base_add + addr;
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	if (cpu.cr0.protect_enable) {
		lnaddr_t lnaddr	= seg_translate(addr, len, sreg);
		return lnaddr_read(lnaddr, len);
	}
	else
		return hwaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	if (cpu.cr0.protect_enable) {
		lnaddr_t lnaddr	= seg_translate(addr, len, sreg);
		lnaddr_write(lnaddr, len, data);
	}
	else
		hwaddr_write(addr, len, data);
}

