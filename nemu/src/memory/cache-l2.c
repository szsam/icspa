#include <stdlib.h>
#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#include "memory/cache-l2.h"

static void cache_read_internal(Cache_level2 * const this, hwaddr_t addr, uint8_t *data, size_t len)
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
			// write back if necessary
			if (selected_set[lnIx].dirty) {
				MEM_ADDR evict_addr;
				evict_addr.tag = selected_set[lnIx].tag;
				evict_addr.set_index = madd.set_index;
				evict_addr.block_offset = 0;
				for (size_t i=0; i<BLOCK_SIZE/4; i++) {
					uint32_t temp;
					memcpy(&temp, selected_set[lnIx].block + 4*i, 4);
					dram_write(evict_addr.addr, 4, temp);
					evict_addr.block_offset += 4;
				}
			}	
		}
		// copy a block from main memory to cache
		selected_set[lnIx].valid = 1;
		selected_set[lnIx].tag = madd.tag;
		hwaddr_t begin_addr = addr & 0xffffffc0;
		for (size_t i=0; i<BLOCK_SIZE/sizeof(uint32_t); i++) {
			uint32_t temp = dram_read(begin_addr+4*i, 4);			
			memcpy(selected_set[lnIx].block + 4*i, &temp, 4);
		}
	}
	// read data from SRAM
	memcpy(data, selected_set[lnIx].block + madd.block_offset, len);
}

static uint32_t cache_l2_read(Cache_level2 * const this, hwaddr_t addr, size_t len)
{
	MEM_ADDR madd;
	madd.addr = addr;
	uint8_t temp[4];
	if (madd.block_offset + len <= BLOCK_SIZE) {
		cache_read_internal(this, addr, temp, len);
	}
	else {
		// data cross the boundary of cache block
		Log("data cross the boundary of cache block, addr=0x%x, len=%d", addr, len);
		int len1 = BLOCK_SIZE - madd.block_offset;	
		cache_read_internal(this, addr, temp, len1);
		cache_read_internal(this, addr+len1, temp+len1, len-len1);
	}
	// return unalign_rw(temp, len);
	switch (len) {
		case 1: return unalign_rw(temp, 1);
		case 2: return unalign_rw(temp, 2);
		case 4: return unalign_rw(temp, 4);
		default: assert(0);
	}
}


static void cache_write_internal(struct Cache_level2 * const this, 
		 hwaddr_t addr, size_t len, uint32_t data) {

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
	
	if (lnIx < LINES_PER_SET) {
		// hit
		memcpy(selected_set[lnIx].block+madd.block_offset, &data, len);
		selected_set[lnIx].dirty = 1;
	}
/*TODO*/
 }

static void cache_l2_write(Cache_level2 * const this, hwaddr_t addr, size_t len, uint32_t data)
{
	// write back, write allocate
	cache_write_internal(this, addr, len, data);
}

static void init_cache(struct Cache_level2 * const this) {
	for (int setIx = 0; setIx < SET_SIZE; ++setIx)
		for (int lnIx = 0; lnIx < LINES_PER_SET; ++lnIx)
			this->sets[setIx][lnIx].valid = 0;

}

// define an object of Cache_level2
// designated initialization, introduced in C11
Cache_level2 cache_l2 =
{	// install methods
	 .read = cache_l2_read, 
	 .write = cache_l2_write, 
	 .init = init_cache
};


#include "memory/cache-template-end.h"
