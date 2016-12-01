#include "common.h"

#include "memory/cache-l1.h"
#include "memory/cache-template-end.h"
#include "cpu/reg.h"

#define PTE_SIZE 4

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

hwaddr_t page_translate(lnaddr_t addr) {
	union {
		struct {
			uint32_t offset : 12;
			uint32_t page	: 10;
			uint32_t dir	: 10;
		};
		lnaddr_t lnaddr;
	}lnaddr_fmt;

	lnaddr_fmt.lnaddr = addr;
	hwaddr_t dir_entry_add = (cpu.cr3.page_directory_base << 12) + PTE_SIZE * lnaddr_fmt.dir;
	PDE dir_entry;
    dir_entry.val = hwaddr_read(dir_entry_add, PTE_SIZE);
	// assert(dir_entry.present);
	Assert(dir_entry.present, "lnaddr=0x%x", addr);

	hwaddr_t tbl_entry_add = (dir_entry.page_frame << 12) + PTE_SIZE * lnaddr_fmt.page;
	PTE tbl_entry;
	tbl_entry.val = hwaddr_read(tbl_entry_add, PTE_SIZE);
	// assert(tbl_entry.present);
	Assert(tbl_entry.present, "lnaddr=0x%x", addr);

	return (tbl_entry.page_frame << 12) + lnaddr_fmt.offset;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	if (cpu.cr0.paging) {
		if ((addr & 0xfffff000) != ((addr+len-1) & 0xfffff000)) {
			// data cross page boundary
			assert(0);
		}
		else {
			hwaddr_t hwaddr = page_translate(addr);
			return hwaddr_read(hwaddr, len);
		}
	}
	else
		return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	if (cpu.cr0.paging) {
		if ((addr & 0xfffff000) != ((addr+len-1) & 0xfffff000)) {
			// data cross page boundary
			assert(0);
		}
		else {
			hwaddr_t hwaddr = page_translate(addr);
			hwaddr_write(hwaddr, len, data);
		}
	}
	else
		hwaddr_write(addr, len, data);
}

// segment translation in protect mode
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	if (!seg_reg(sreg).cache.valid) {
		assert(seg_reg(sreg).table_indicator == 0);
		assert(seg_reg(sreg).index != 0);
		// sizeof a segment descriptor is 64-bit
		lnaddr_t desc_add = cpu.gdtr.base + 8 * seg_reg(sreg).index;
		SegDesc theDesc;

		assert(8 * seg_reg(sreg).index + 7 <= cpu.gdtr.limit);
		// read the descriptor
		uint32_t temp;
		temp = lnaddr_read(desc_add, 4);
		memcpy(&theDesc, &temp, 4);
		temp = lnaddr_read(desc_add + 4, 4);
		memcpy((void *)&theDesc + 4, &temp, 4);

		lnaddr_t seg_base_add = 
			(((theDesc.base_31_24 << 8) + theDesc.base_23_16) << 16 ) + theDesc.base_15_0;

		uint32_t seg_limit = (theDesc.limit_19_16 << 16) + theDesc.limit_15_0;
		if (theDesc.granularity) { 
			// the limit is interpreted in units of 4 Kilobytes
			seg_limit <<= 12;
			seg_limit |= 0xfff;
		}

		// save limit and base in hidden part of segment register
		seg_reg(sreg).cache.valid = 1;
		seg_reg(sreg).cache.limit = seg_limit;
		seg_reg(sreg).cache.base = seg_base_add;

		assert(theDesc.present);
	}

	// check segment limit
	assert(addr + len <= seg_reg(sreg).cache.limit);
	return seg_reg(sreg).cache.base + addr;
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

