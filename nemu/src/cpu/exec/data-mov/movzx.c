#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(movzwl) {
	op_src->size = 2;
	int len = read_ModR_M(eip+1, op_src, op_dest);
	reg_l(op_dest->reg) = op_src->val;

	snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsl[op_dest->reg]);
	print_asm("movzwl %s, %s", op_src->str, op_dest->str);

	return len+1;
}

make_helper(movzbv) {
	op_src->size = 1;
	int len = read_ModR_M(eip+1, op_src, op_dest);

	if (ops_decoded.is_operand_size_16) {
		reg_w(op_dest->reg) = (uint16_t)op_src->val;
		snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsw[op_dest->reg]);
		print_asm("movzbw %s, %s", op_src->str, op_dest->str);
	}
	else {
		reg_l(op_dest->reg) = op_src->val;
		snprintf(op_dest->str, OP_STR_SIZE, "%%%s", regsl[op_dest->reg]);
		print_asm("movzbl %s, %s", op_src->str, op_dest->str);
	}
	return len+1;
}
