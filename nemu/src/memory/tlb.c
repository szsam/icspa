#include <stdlib.h>
#include "memory/tlb.h"

// return true when TLB hit, false otherwise
static bool read(TLB *const this, uint32_t vpn, PTE *pte) {
	int ix = 0;
	
	while (ix < NR_TLB_BLOCK &&
			!(this->blocks[ix].valid && this->blocks[ix].vpn == vpn) )
	{
		++ix;  
	}

	if (ix == NR_TLB_BLOCK)
		return false;
	else
	{
		*pte = this->blocks[ix].pte;
		return true;
	}
}

static void fill(TLB *const this, uint32_t vpn, PTE pte)
{
	int ix = 0;

	// find an empty block if possible
	while (ix < NR_TLB_BLOCK && this->blocks[ix].valid)
		++ix;

	if (ix == NR_TLB_BLOCK) { 
		// TLB is full, thus random replacement algorithm is used
		ix = rand() % NR_TLB_BLOCK;
	}

	this->blocks[ix].valid = 1;
	this->blocks[ix].vpn = vpn;
	this->blocks[ix].pte = pte;
}

static void flush(TLB *const this)
{
	for (int ix = 0; ix < NR_TLB_BLOCK; ++ix)
		this->blocks[ix].valid = 0;
}

TLB tlb =
{
	.read = read,
	.fill = fill,
	.flush = flush
};
