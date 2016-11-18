#include "common.h"

//#define SET_WIDTH 7
//#define LINES_PER_SET 8
//#define BLOCK_WIDTH 6
//#define MEM_ADDR_WIDTH 32
#define TAG_WIDTH (MEM_ADDR_WIDTH - SET_WIDTH - BLOCK_WIDTH)

#define BLOCK_SIZE (1 << BLOCK_WIDTH)
#define SET_SIZE (1 << SET_WIDTH)

typedef struct {
	struct {
		uint32_t valid : 1;
#ifdef WRITE_BACK
		uint32_t dirty : 1;
#endif
		uint32_t tag : TAG_WIDTH;
	};
	uint8_t block[BLOCK_SIZE];
}concat3(Line, _, CACHE_CLASS_NAME);

/* define a "class" Cache */
typedef struct CACHE_CLASS_NAME {
	/* define "attributes" members */
	concat3(Line, _, CACHE_CLASS_NAME) sets[SET_SIZE][LINES_PER_SET];

	 /* define "methods" */
	 // public :
	 uint32_t (*read)(struct CACHE_CLASS_NAME * const this, hwaddr_t, size_t);
	 void (*write)(struct CACHE_CLASS_NAME * const this, hwaddr_t, size_t, uint32_t);
	 void (*init)(struct CACHE_CLASS_NAME * const this);
	 // private:
	 // void (*read_internal)(struct CACHE_CLASS_NAME * const this, hwaddr_t, uint8_t *, size_t);
	 // void (*write_internal)(struct CACHE_CLASS_NAME * const this, hwaddr_t, size_t, uint32_t);
}CACHE_CLASS_NAME;


typedef union {
	struct {
		uint32_t block_offset : BLOCK_WIDTH;
		uint32_t set_index : SET_WIDTH;
		uint32_t tag : TAG_WIDTH;
	};
	hwaddr_t addr;
}concat3(MEM_ADDR, _, CACHE_CLASS_NAME);

#define Line concat3(Line, _, CACHE_CLASS_NAME)
#define MEM_ADDR concat3(MEM_ADDR, _, CACHE_CLASS_NAME)
