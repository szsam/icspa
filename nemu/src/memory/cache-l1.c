#include <stdlib.h>
#include "common.h"

// uint32_t dram_read(hwaddr_t, size_t);
// void dram_write(hwaddr_t, size_t, uint32_t);

#include "memory/cache-l2.h"
#include "memory/cache-template-end.h"
extern Cache_level2 cache_l2;

#include "memory/cache-l1.h"

uint32_t c1_hit = 0;
uint32_t c1_miss = 0;

// Line Matching
static int line_matching(Line *selected_set, MEM_ADDR madd) {
	int lnIx = 0;
	while ( lnIx < LINES_PER_SET &&
			 !(selected_set[lnIx].valid && selected_set[lnIx].tag == madd.tag)) {
		++lnIx;
	}
	return lnIx;
}

static void cache_read_internal(Cache_level1 * const this, hwaddr_t addr, uint8_t *data, size_t len)
{
	MEM_ADDR madd;
	madd.addr = addr;

	assert(madd.block_offset + len <= BLOCK_SIZE);

	// set selection
	Line *selected_set = this->sets[madd.set_index];

	// line matching
	int lnIx = line_matching(selected_set, madd);

	if (lnIx == LINES_PER_SET) { // miss!
		++c1_miss;
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
		hwaddr_t begin_addr = addr & 0xffffffc0;
		for (size_t i=0; i<BLOCK_SIZE/sizeof(uint32_t); i++) {
			uint32_t temp = cache_l2.read(&cache_l2, begin_addr+4*i, 4);			
			memcpy(selected_set[lnIx].block + 4*i, &temp, 4);
		}
	}
	else ++c1_hit;

	// read data from SRAM
	memcpy(data, selected_set[lnIx].block + madd.block_offset, len);
}

static uint32_t cache_l1_read(Cache_level1 * const this, hwaddr_t addr, size_t len)
{
	MEM_ADDR madd;
	madd.addr = addr;
	uint8_t temp[4];
	if (madd.block_offset + len <= BLOCK_SIZE) {
		cache_read_internal(this, addr, temp, len);
	}
	else {
		// data cross the boundary of cache block
		// Log("data cross the boundary of cache block, addr=0x%x, len=%d", addr, len);
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


static void cache_write_internal(struct Cache_level1 * const this, 
		 hwaddr_t addr, size_t len, uint32_t data) {

	MEM_ADDR madd;
	madd.addr = addr;

	assert(madd.block_offset + len <= BLOCK_SIZE);

	// set selection
	Line *selected_set = this->sets[madd.set_index];

	// line matching
	int lnIx = line_matching(selected_set, madd);
	
	if (lnIx < LINES_PER_SET) {
		// hit
		++c1_hit;
		memcpy(selected_set[lnIx].block+madd.block_offset, &data, len);
	}
	else ++c1_miss;

	// write through; in case of miss, no write allocate
	cache_l2.write(&cache_l2, addr, len, data);
 }

static void cache_l1_write(Cache_level1 * const this, hwaddr_t addr, size_t len, uint32_t data)
{
	// write through, not write allocate
	cache_write_internal(this, addr, len, data);
}

static void init_cache(struct Cache_level1 * const this) {
	for (int setIx = 0; setIx < SET_SIZE; ++setIx)
		for (int lnIx = 0; lnIx < LINES_PER_SET; ++lnIx)
			this->sets[setIx][lnIx].valid = 0;

}

// define an object of Cache_level1
// designated initialization, introduced in C11
Cache_level1 cache_l1 =
{	// install methods
	 .read = cache_l1_read, 
	 .write = cache_l1_write, 
	 .init = init_cache
};


#include "memory/cache-template-end.h"
