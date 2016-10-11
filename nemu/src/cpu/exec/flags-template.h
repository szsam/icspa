#include "cpu/exec/template-start.h"

bool concat(even_parity_, SUFFIX)(DATA_TYPE data) {
	int count = 0; //the number of '1'
	for (int i = 0; i<DATA_BYTE*8-1; i++) {
		count += data & 0x1;
		data >>= 1;
	}
	return count % 2 == 0;
	
}

#include "cpu/exec/template-end.h"
