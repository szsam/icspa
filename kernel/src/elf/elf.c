#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif

	elf = (void*)buf;

	/* TODO: fix the magic number with the correct one */
	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);

	/* Load each program segment */
	// panic("please implement me");
	ph = (Elf32_Phdr *)(buf + elf->e_phoff);
	int ix;
	int cnt = 0;
	for(ix = 0; ix < elf->e_phnum; ++ix, ++ph) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {

	//		nemu_assert( (ix==0 && ph->p_vaddr==0x800000) ||
	//				(ix==1 && ph->p_vaddr==0x801000) );
	//		nemu_assert( (ix==0 && ph->p_filesz==0x1ec) ||
	//				(ix==1 && ph->p_filesz==0x7c) );
	//		nemu_assert( (ix==0 && ph->p_memsz==0x1ec) ||
	//				(ix==1 && ph->p_memsz==0xf84) );
	//		nemu_assert( (ix==0 && ph->p_offset==0x0) ||
	//				(ix==1 && ph->p_offset==0x1000) );
			/* TODO: read the content of the segment from the ELF file 
			 * to the memory region [VirtAddr, VirtAddr + FileSiz)
			 */
			memcpy((void *)(ph->p_vaddr), buf + ph->p_offset, ph->p_filesz);
			 
			/* TODO: zero the memory region 
			 * [VirtAddr + FileSiz, VirtAddr + MemSiz)
			 */
			if (ph->p_memsz > ph->p_filesz)
				memset((void *)(ph->p_vaddr) + ph->p_filesz, 0, ph->p_memsz-ph->p_filesz);
			cnt++;
			// set_bp();
#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
		}
	}
	//nemu_assert(ix == 3);
	//nemu_assert(cnt == 2);

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
