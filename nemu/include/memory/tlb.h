#ifndef __TLB_H__
#define __TLB_H__

#include "common.h"
#include "../../lib-common/x86-inc/mmu.h"

#define NR_TLB_BLOCK 64

typedef struct TLB {
	struct {
		PTE pte;
		uint32_t vpn	: 20;
		uint32_t valid	: 1;
	}blocks[NR_TLB_BLOCK];

	bool (*read)(struct TLB *const this, uint32_t vpn, PTE *pte);
	void (*fill)(struct TLB *const this, uint32_t vpn, PTE pte);
	void (*flush)(struct TLB *const this);
}TLB;

extern TLB tlb;

#endif
