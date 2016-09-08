#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, DEC, HEX, REG,
	NEQ, AND, OR, NOT, DEREF

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},					// subtraction
	{"\\*", '*'},					// multiplication
	{"/", '/'},						// division
	{"\\(",'('},					// left-parenthese
	{"\\)",')'},					// right-parenthese
	{"0x[0-9a-fA-F]+", HEX},		// hex number, this rule must precede DEC's
	{"[0-9][0-9]*", DEC},			// decmical number
	{"\\$[A-Za-z]+", REG},			// register
	{"==", EQ},						// equal
	{"!=", NEQ},					// not equal
	{"&&", AND},					// logic and
	{"\\|\\|", OR},					// logic or
	{"!", NOT},						// logic NOT
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case '+':
						tokens[nr_token++].type = '+'; break;
					case '-':
						tokens[nr_token++].type = '-'; break;
					case '*':
						tokens[nr_token++].type = '*'; break;
					case '/':
						tokens[nr_token++].type = '/'; break;
					case '(':
						tokens[nr_token++].type = '('; break;
					case ')':
						tokens[nr_token++].type = ')'; break;
					case DEC:
						tokens[nr_token].type = DEC;
						if (substr_len > 31)
							panic("buffer overflow");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						++nr_token;
						break;
					case HEX:
						tokens[nr_token].type = HEX;
						if (substr_len > 31)
							panic("buffer overflow");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						++nr_token;
						break;
					case REG:
						tokens[nr_token].type = REG;
						if (substr_len > 31)
							panic("buffer overflow");
						// drop the leading '$'
						strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
						++nr_token;
						break;
					case EQ:
						tokens[nr_token++].type = EQ; break;
					case NEQ:
						tokens[nr_token++].type = NEQ; break;
					case AND:
						tokens[nr_token++].type = AND; break;
					case OR:
						tokens[nr_token++].type = OR; break;
					case NOT:
						tokens[nr_token++].type = NOT; break;
						
					default: panic("please implement me");
				}
				break;

			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

/*
 * return value:
 *  0  parentheses match, and the expr is surrounded by a pair of parentheses
 *  1  parentheses match, but the expr is NOT surrounded by a pair of parentheses
 * -1  parentheses do NOT match, bad expr
 */
int check_parentheses(int p, int q) {
	int ret;
	int count = 0;
	bool empty = false;

	for (int ix = p; ix <= q; ix++) {
		if (tokens[ix].type == '(')
			++count;
		else if (tokens[ix].type == ')') {
			--count;
			if (count < 0)
				return -1;
			else if (count == 0 && ix != q)
				empty = true;  //This means the expr is not surrounded by a pair of parentheses
		}

	}

	if (count > 0)
		return -1;

	if (tokens[p].type == '(' && tokens[q].type == ')' && !empty)
		ret = 0;
	else
		ret = 1;
	//Log("ret = %d", ret);
	return ret;
}

bool is_operator(int type) {
	switch (type) {
		case '+': case '-': case '*': case '/':
		case EQ: case NEQ: case AND: case OR: case NOT:
			return true;
		default:
			return false;
	}
}

bool inside_pare(int ix, int begin, int end) {
	/* parentheses are mathched, so we only need to check
	 * whether there is '(' on the left of the token indexed by ix
	 * [or ')' on the right] */
	for (--ix; begin <= ix; --ix) {
		int t = tokens[ix].type;
		if (t == '(')
			return true;
		else if (t == ')')
			return false;
	}
	return false;
}
	


int preced(int type) {   // the bigger the value, the lower the precedence
	switch (type) {
		case '!': case DEREF:
			return 1;
		case '*': case '/':
			return 2;
		case '+': case '-':
			return 3;
		case EQ: case NEQ:
			return 4;
		case AND: return 5;
		case OR: return 6;
		default:
			assert(0);
	}
}

//compare precedences of two operators
int preced_comp(int type1, int type2)		
{
	return preced(type1)-preced(type2);
}

enum {LEFT, RIGHT};

//associativity of a operator
int associat(int type)	
{
	switch (type) {
		case '+': case '-': case '*': case '/':
		case EQ: case NEQ: case AND: case OR:
			return LEFT;
		case '!': case DEREF:
			return RIGHT;
		default:
			assert(0);
	}
}

//return the position of dominant operator in the token expression;
int dominant_operator(int p, int q) {
	//panic("Waiting for implemention");
	
	/* find the first operator that is not inside parentheses*/	
	int ix = p;
	while (!is_operator(tokens[ix].type) || inside_pare(ix, p, q))
		++ix;
	assert(ix <= q);

	int ix_domin_oper = ix;

	for (++ix; ix <= q; ++ix) {
		if (!is_operator(tokens[ix].type) || inside_pare(ix, p, q))
			continue;

		/* current operator has lower precedence or
		* same precedence and left-associative */
		int cur_type = tokens[ix].type;
		int domin_type = tokens[ix_domin_oper].type;
		if (preced_comp(cur_type, domin_type)>0 ||
			(preced_comp(cur_type, domin_type)==0 && associat(cur_type)==LEFT) )
			ix_domin_oper = ix;
	}
	return ix_domin_oper;
}

uint32_t reg_val(const char *reg_name)
{
	int ix;
	for (ix = R_EAX; ix < R_EDI; ++ix)
	{
		if (!strcmp(reg_name, regsl[ix]))
			return reg_l(ix);
	}
	for (ix = R_AX; ix < R_DI; ++ix)
	{
		if (!strcmp(reg_name, regsw[ix]))
			return reg_w(ix);
	}
	for (ix = R_AL; ix < R_BH; ++ix)
	{
		if (!strcmp(reg_name, regsb[ix]))
			return reg_b(ix);
	}
	if (!strcmp(reg_name, "eip"))
		return cpu.eip;
	else
		panic("no such register");
}

uint32_t eval(int p, int q) {
	int state;	//store return value of check_parentheses()

	if(p > q) {
		/* Bad expression */
		//assert(0);
		panic("Bad expression");
	}
	else if(p == q) { 
		/* Single token.
		 * For now this token should be a decmical number,
		 *	hex number or name of a register. 
		 * Return the value of it.
		 */ 
		switch (tokens[p].type) {
			case DEC: return atoi(tokens[p].str);
			case HEX: 
			  {
				  uint32_t tmp;
				  sscanf(tokens[p].str, "%x", &tmp);
				  return tmp;
			  }
			case REG:
			  return reg_val(tokens[p].str);
			default:
			  assert(0);
		}
		
	}
	else if((state = check_parentheses(p, q)) == 0) {
		/* The expression is surrounded by a matched pair of parentheses. 
		 * If that is the case, just throw away the parentheses.
		 */
		return eval(p + 1, q - 1); 
	}
	else if (state == -1) {
		fprintf(stderr, "Parentheses do not match\n");
		return 0;
	}
	else {
		//op = the position of dominant operator in the token expression;
		int op = dominant_operator(p, q);

		if (op == '!' || op == DEREF)	//unary operators
		{
			uint32_t val = eval(op+1, q);
			switch(tokens[op].type) {
				case '!': return !val;
				case DEREF: 
					return swaddr_read(val, 4);
				default: assert(0);
			}


		}
		else  //binary operators
		{
			uint32_t val1 = eval(p, op - 1);
			uint32_t val2 = eval(op + 1, q);

			switch(tokens[op].type) {
				case '+': return val1 + val2;
				case '-': return val1 - val2;
				case '*': return val1 * val2; 
				case '/': return val1 / val2; 
				case EQ : return val1 == val2;
				case NEQ : return val1 != val2;
				case AND : return val1 && val2;
				case OR : return val1 || val2;
				
				default: assert(0);
			}

		}
	}
}

uint32_t expr(char *e, bool *success) {
	*success = true;
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	
	// distinguish between DEREF and '*'
	if (tokens[0].type == '*') {
		tokens[0].type = DEREF;
		Log("tokens[%d].type = DEREF",0);
	}

	for(int i = 1; i < nr_token; i ++) {
		if(tokens[i].type == '*' ) {
			int prev_t = tokens[i - 1].type;
			int is_mul = prev_t == DEC || prev_t == HEX || prev_t == REG || prev_t == ')'; 
			if (!is_mul) { 
				tokens[i].type = DEREF;
				Log("tokens[%d].type = DEREF",i);
			}
		}
	}

	return eval(0, nr_token-1);
}

