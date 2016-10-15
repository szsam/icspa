#include "trap.h"
#include "FLOAT.h"

FLOAT f(FLOAT x) { 
	/* f(x) = 1/(1+25x^2) */
	return F_div_F(int2F(1), int2F(1) + F_mul_int(F_mul_F(x, x), 25));
}

FLOAT computeT(int n, FLOAT a, FLOAT b, FLOAT (*fun)(FLOAT)) {
	int k;
	FLOAT s,h;
	h = F_div_int((b - a), n);
	s = F_div_int(fun(a) + fun(b), 2 );
	for(k = 1; k < n; k ++) {
		s += fun(a + F_mul_int(h, k));
	}
	s = F_mul_F(s, h);
	return s;
}

int main() { 
//	FLOAT a = computeT(10, f2F(-1.0), f2F(1.0), f);
//	FLOAT ans = f2F(0.551222);
//
//	nemu_assert(Fabs(a - ans) < f2F(1e-4));
	
	int a = F2int(f2F(12.0));
	nemu_assert(a == 12);
	FLOAT b = F_mul_int( int2F(a), 3);
	nemu_assert(b == f2F(36.0));

	nemu_assert(f2F(127.0) == int2F(127));
	nemu_assert(f2F(12345.0) == int2F(12345));

	nemu_assert(Fabs(b) == b);
	nemu_assert(Fabs(f2F(-34.56)) == f2F(34.56));

	nemu_assert(F_div_int(b, 9) == int2F(4));

	FLOAT c = F_mul_F(f2F(12.1),f2F(25.3));
	FLOAT d = f2F(306.13);
	nemu_assert(Fabs(c-d) < f2F(1e-2));

	return 0;
}
