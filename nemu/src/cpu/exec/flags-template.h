#include "cpu/exec/template-start.h"

bool concat(even_parity_, SUFFIX)(DATA_TYPE data) {
	int count = 0; //the number of '1'
	for (int i = 0; i<DATA_BYTE*8-1; i++) {
		count += data & 0x1;
		data >>= 1;
	}
	return count % 2 == 0;
	
}

bool concat(uadd_ok_, SUFFIX)(DATA_TYPE a, DATA_TYPE b) {
    DATA_TYPE sum = a + b;
    return sum >= a;
}

bool concat(tadd_ok_, SUFFIX)(DATA_TYPE_S a, DATA_TYPE_S b) {
    DATA_TYPE_S sum = a + b;
    bool neg_over = a<0 && b<0 && sum>=0;
    bool pos_over = a>=0 && b>=0 && sum<0;
    return !neg_over && !pos_over;
}

bool concat(usub_ok_, SUFFIX)(DATA_TYPE a, DATA_TYPE b) {
    return a >= b;
}

bool concat(tsub_ok_, SUFFIX)(DATA_TYPE_S a, DATA_TYPE_S b) {
    DATA_TYPE_S diff = a - b;
    bool neg_over = a<0 && b>=0 && diff>=0;
    bool pos_over = a>=0 && b<0 && diff<0;
    return !neg_over && !pos_over;
}

#include "cpu/exec/template-end.h"
