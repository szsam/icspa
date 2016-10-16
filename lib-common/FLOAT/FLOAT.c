#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	// nemu_assert(0);
// 	FLOAT al = a & 0xff;
// 	FLOAT bl = b & 0xff;
// 	FLOAT ah = a >> 8;
// 	FLOAT bh = b >> 8;
// 	return ((al*bl) >> 16) + ((al*bh + ah*bl) >> 8) + ah*bh;
	long long prod = (long long)a * b;
	return prod >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */

	// nemu_assert(0);
	a <<= 8;
	b >>= 8;
	return a/b;
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */

	// nemu_assert(0);
	// Assume 'a' is normalized
	FLOAT res = 0;
	union {
		struct {
			unsigned frac : 23;
			unsigned exp : 8;
			unsigned sgn : 1;
		};
		float f;
	}un;
	un.f = a;

	un.exp += 16;	// multiply (2^16)

	int E = un.exp - 127;
	res = un.frac;
	if (E<23) res >>= (23-E);
	else res <<= (E-23);

	res |= (1<<E);

	if (un.sgn) res = -res;

	return res;
}

FLOAT Fabs(FLOAT a) {
	// nemu_assert(0);
	return a >= 0 ? a : -a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

