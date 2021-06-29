#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <iso646.h>
#include <string.h>

#define NUM_VARS	26
#define STACK_SIZE	32

enum token {
	TOKEN_START,
	TOKEN_VAL,
	TOKEN_LP,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
};

bool hard = false;
double vars[NUM_VARS] = {0};
char expr[1024];

void compute(void) {
	double stack_value[STACK_SIZE] = {0};
	enum token stack_token[STACK_SIZE] = {0};
	int sp = 1; //stack pointer, first free slot
	stack_token[0] = TOKEN_START;

	int pos = 0;
	int to_var = -1;
	while (isspace(expr[pos]))
		pos++;
	/* Determine if we're assigning to a variable.  */
	if (expr[pos] >= 'a' and expr[pos] <= 'z') {
		int pos_copy = pos + 1;
		while (isspace(expr[pos_copy]))
			pos_copy++;
		if (expr[pos_copy] == '=') {
			to_var = expr[pos] - 'a';
			pos = pos_copy + 1;
		}
	}
	while (1337) {
		while (isspace(expr[pos]))
			pos++;

		/* Perform reductions.  */
		// pop from the stack
		while (true) {
			if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_ADD and stack_token[sp-3] != TOKEN_VAL) {
				/* Unary +.  */
				stack_value[sp-2] = stack_value[sp-1];
				stack_token[sp-2] = TOKEN_VAL;
				sp--;
			} else if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_SUB and stack_token[sp-3] != TOKEN_VAL) {
				/* Unary -.  */
				stack_value[sp-2] = -stack_value[sp-1];
				stack_token[sp-2] = TOKEN_VAL;
				sp--;
			} else if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_MUL and stack_token[sp-3] == TOKEN_VAL) {
				/* Multiplication.  */
				stack_value[sp-3] *= stack_value[sp-1];
				sp -= 2;
			} else if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_DIV and stack_token[sp-3] == TOKEN_VAL) {
				/* Division.  */
				stack_value[sp-3] /= stack_value[sp-1];
				sp -= 2;
			} else if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_ADD and stack_token[sp-3] == TOKEN_VAL
			    and expr[pos] != '*' and expr[pos] != '/') {
				/* Addition.  */
				stack_value[sp-3] += stack_value[sp-1];
				sp -= 2;
			} else if (stack_token[sp-1] == TOKEN_VAL and stack_token[sp-2] == TOKEN_SUB and stack_token[sp-3] == TOKEN_VAL
			    and expr[pos] != '*' and expr[pos] != '/') {
				/* Subtraction.  */
				stack_value[sp-3] -= stack_value[sp-1];
				sp -= 2;
			} else {
				break;
			}
		}

		// push onto the stack
		if (expr[pos] == '(') {
			/* Left paren -- push on stack, wait for something more interesting.  */
			stack_token[sp++] = TOKEN_LP;
			pos++;
		} else if (expr[pos] == ')') {
			/* Right paren -- verify there's a matching right paren, move the value down the stack.  */
			if (stack_token[sp-2] != TOKEN_LP)
				goto syntax_err;
			stack_token[sp-2] = TOKEN_VAL;
			stack_value[sp-2] = stack_value[sp-1];
			sp--;
			pos++;
		} else if (expr[pos] == '+') {
			/* Addition.  */
			stack_token[sp++] = TOKEN_ADD;
			pos++;
		} else if (expr[pos] == '-') {
			/* Subtraction.  */
			stack_token[sp++] = TOKEN_SUB;
			pos++;
		} else if (expr[pos] == '*') {
			/* Multiplication.  */
			stack_token[sp++] = TOKEN_MUL;
			pos++;
		} else if (expr[pos] == '/') {
			/* Division.  */
			stack_token[sp++] = TOKEN_DIV;
			pos++;
		} else if (expr[pos] >= 'a' and expr[pos] <= 'z') {
			/* A variable.  Read it.  */
			stack_token[sp] = TOKEN_VAL;
			stack_value[sp++] = vars[expr[pos] - 'a'];   // paired with this
			pos++;
		} else if (expr[pos] == 0 or expr[pos] == '\n') {
			/* End of the line.  */
			if (stack_token[sp-2] != TOKEN_START)
				goto syntax_err;
			if (to_var == -1) { // NOT a var assignment
				printf("%.20e\n", stack_value[sp-1]);
				//printf("%x\n", stack_value[sp-1]);
				 if (hard)
					exit(0);
			} else { // update var's value
				vars[to_var] = stack_value[sp-1];    // Maybe this
			}
			return;
		} else {
			/* Constant or bust.  */
			char *endptr;
			stack_token[sp] = TOKEN_VAL;
			stack_value[sp] = strtod(expr + pos, &endptr);
			if (endptr == expr + pos)
				goto val_err;
			pos = endptr - expr;
			sp++;
		}
	}

syntax_err:
	printf("Syntax error at position %d :(\n", pos);
	exit(1);
val_err:
	printf("Cannot parse value: %s :(\n", expr + pos);
	exit(1);
}


int main(int argc, char **argv) {
	if (argc > 1 and !strcmp(argv[1], "--hard"))
		hard = true;
	setbuf(stdin, 0);
	setbuf(stdout, 0);
	setbuf(stderr, 0);
	while (fgets(expr, sizeof expr, stdin)) {
		compute();
	}
	return 0;
}
