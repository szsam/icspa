#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(movswl) {
	op_src->size = 2;
	int len = read_ModR_M(eip+1, op_src, op_dest);
	reg_l(op_dest->reg) = ((int32_t)(op_src->val << 16)) >> 16;

	snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsl[op_dest->reg]);
	print_asm("movswl %s, %s", op_src->str, op_dest->str);

	return len+1;
}

make_helper(movsbv) {
	op_src->size = 1;
	int len = read_ModR_M(eip+1, op_src, op_dest);

	if (ops_decoded.is_operand_size_16) {
		reg_w(op_dest->reg) = (uint16_t)( ((int32_t)(op_src->val << 24)) >> 24 );
		snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsw[op_dest->reg]);
		print_asm("movsbw %s, %s", op_src->str, op_dest->str);
	}
	else {
		reg_l(op_dest->reg) = ((int32_t)(op_src->val << 24)) >> 24;
		snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsl[op_dest->reg]);
		print_asm("movsbl %s, %s", op_src->str, op_dest->str);
	}
	return len+1;
}
