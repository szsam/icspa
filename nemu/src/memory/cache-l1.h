#include <stdlib.h>
#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define SET_WIDTH 7
#define LINES_PER_SET 8
#define BLOCK_WIDTH 6
#define MEM_ADDR_WIDTH 32
#define CACHE_CLASS_NAME Cache_level1

#include "cache-template.h"

// define a Cache object
// Cache_level1 cache_l1;

void cache_read_internal(Cache_level1 * const this, hwaddr_t addr, uint8_t *data, size_t len)
{
	MEM_ADDR madd;
	madd.addr = addr;

	assert(madd.block_offset + len <= BLOCK_SIZE);

	// set selection
	Line *selected_set = this->sets[madd.set_index];

	// line matching
	int lnIx = 0;
	while ( lnIx < LINES_PER_SET &&
			 !(selected_set[lnIx].valid && selected_set[lnIx].tag == madd.tag)) {
		++lnIx;
	}

	if (lnIx == LINES_PER_SET) { // miss!
		lnIx = 0;
		while (lnIx < LINES_PER_SET && selected_set[lnIx].valid)
			++lnIx;
		if (lnIx == LINES_PER_SET) {
			// the cache set is full, random replacement algorithm is used
			lnIx = rand() % LINES_PER_SET;
		}
		// copy a block from main memory to cache
		selected_set[lnIx].valid = 1;
		selected_set[lnIx].tag = madd.tag;
		for (size_t i=0; i<BLOCK_SIZE/sizeof(uint32_t); i++) {
			uint32_t temp = dram_read(addr+4*i, 4);			
			memcpy(selected_set[lnIx].block + 4*i, &temp, 4);
		}
	}
	// read data from SRAM
	memcpy(data, selected_set[lnIx].block, len);
}

uint32_t cache_l1_read(Cache_level1 * const this, hwaddr_t addr, size_t len)
{
	MEM_ADDR madd;
	madd.addr = addr;
	uint8_t temp[4];
	if (madd.block_offset + len <= BLOCK_SIZE) {
		this->read_internal(this, addr, temp, len);
	}
	else {
		// data cross the boundary of cache block
		int len1 = BLOCK_SIZE - madd.block_offset;	
		this->read_internal(this, addr, temp, len1);
		this->read_internal(this, addr+len1, temp+len1, len-len1);
	}
	// return unalign_rw(temp, len);
	switch (len) {
		case 1: return unalign_rw(temp, 1);
		case 2: return unalign_rw(temp, 2);
		case 4: return unalign_rw(temp, 4);
		default: assert(0);
	}
}

void cache_l1_write(Cache_level1 * const this, hwaddr_t addr, size_t len, uint32_t data)
{
	// write through, no write-allocate
	dram_write(addr, len, data);
}

// install methods
// cache_l1.read = cache_l1_read;
// cache_l1.write = cache_l1_write;
// cache_l1.read_internal = cache_read_internal;

Cache_level1 cache_l1 =
{
	 .read = cache_l1_read, 
	 .write = cache_l1_write, 
	 .read_internal = cache_read_internal
};

void init_cache() {
	for (int setIx = 0; setIx < SET_SIZE; ++setIx)
		for (int lnIx = 0; lnIx < LINES_PER_SET; ++lnIx)
			cache_l1.sets[setIx][lnIx].valid = 0;

}
