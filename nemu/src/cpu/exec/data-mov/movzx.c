#include "cpu/helper.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(movzwl) {
	op_src->size = 16;
	int len = read_ModR_M(eip, op_src, op_dest);
	reg_l(op_dest->reg) = op_src->val;
	return len+1;
}

make_helper(movzbv) {
	op_src->size = 8;
	int len = read_ModR_M(eip, op_src, op_dest);

	if (ops_decoded.is_operand_size_16) {
		reg_w(op_dest->reg) = (uint16_t)op_src->val;
	}
	else {
		reg_l(op_dest->reg) = op_src->val;
	}
	return len+1;
}
