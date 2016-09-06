#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, DEC

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
	{"==", EQ},						// equal
	{"[0-9][0-9]*",DEC}				// decmical number
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
					case NOTYPE:
						break;
					case '+':
						tokens[nr_token++].type = '+';
						break;
					case '-':
						tokens[nr_token++].type = '-';
						break;
					case '*':
						tokens[nr_token++].type = '*';
						break;
					case '/':
						tokens[nr_token++].type = '/';
						break;
					case '(':
						tokens[nr_token++].type = '(';
						break;
					case ')':
						tokens[nr_token++].type = ')';
						break;
					case DEC:
						tokens[nr_token].type = DEC;
						if (substr_len > 31)
							panic("buffer overflow");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						++nr_token;
						break;

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

bool check_parentheses(int p, int q) {
	int count = 0;
	bool once_empty = false;

	for (int ix = p; ix <= q; ix++) {
		if (tokens[ix].type == '(')
			++count;
		else if (tokens[ix].type == ')') {
			--count;
			if (count < 0)
				panic("Parentheses do not match");
			else if (count == 0)
				once_empty = true;
		}

	}

	if (count > 0)
		panic("Parentheses do not match");

	bool ret = tokens[p].type == '(' && tokens[q].type == ')' && !once_empty;	 
	Log("ret = %d", ret);
	return ret;
}

//return the position of dominant operator in the token expression;
int dominant_operator(int p, int q) {
	panic("Waiting for implemention");
}

uint32_t eval(int p, int q) {
	if(p > q) {
		/* Bad expression */
		assert(0);
	}
	else if(p == q) { 
		/* Single token.
		 * For now this token should be a number. 
		 * Return the value of the number.
		 */ 
		return atoi(tokens[p].str);
	}
	else if(check_parentheses(p, q) == true) {
		/* The expression is surrounded by a matched pair of parentheses. 
		 * If that is the case, just throw away the parentheses.
		 */
		return eval(p + 1, q - 1); 
	}
	else {
		//op = the position of dominant operator in the token expression;
		int op = dominant_operator(p, q);
		uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op + 1, q);

		switch(tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2; 
			case '/': return val1 / val2; 
			default: assert(0);
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	
	return eval(0, nr_token-1);
}

