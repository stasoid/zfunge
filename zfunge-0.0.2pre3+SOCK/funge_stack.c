#include "config.h"
/* I didn't mean to make this look so confusing.. it really isn't,
	I swear... */

#include <stdlib.h>

/* I'm too lazy to change this, just think of it as
   always manipulating a single "long **stack;" */
#define stack (*stack)

void fst_init(long **stack) {
	stack = malloc(sizeof(*stack)*2);
	*(long*)stack = 1;
	stack[1] = malloc(sizeof(*stack[1]));
	*stack[1] = 0;
}

long fst_pop(long **stack, long stackstack) {
	long retval;

	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack ||
		!*stack[stackstack]) return 0;

	/* Pop the value */
	retval = stack[stackstack][*stack[stackstack]];

	stack[stackstack] = realloc(stack[stackstack],
		sizeof(*stack[stackstack])*(--(*stack[stackstack]) + 1));

	return retval;
}

char *fst_popstring(long **stack, long stackstack) {
	long ch, t;
	char *retval = malloc(1);
	*retval = 0;
	while ((ch = fst_pop(&stack, stackstack))) {
		retval = realloc(retval, t = strlen(retval)+2);
		retval[t-1] = 0;
		retval[t-2] = ch;
	}
	return retval;
}

void fst_pushstring(long ***stack, long stackstack, char *string) {
	long i;
	for (i = strlen(string); i >= 0; i--)
		fst_push(&stack, stackstack, string[i]);
}

int fst_push(long **stack, long stackstack, long value) {
	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack) return 1;

	stack[stackstack] = realloc(stack[stackstack],
		sizeof(*stack[stackstack])*(++(*stack[stackstack]) + 1));

	stack[stackstack][*stack[stackstack]] = value;
	return 0;
}

long fst_peek(long **stack, long stackstack, long num) {
	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack ||
		!*stack[stackstack]) return 0;

	num = *stack[stackstack] - num;
	if (num < 1 || num > *stack[stackstack]) return 0;

	return stack[stackstack][num];
}

int fst_poke(long **stack, long stackstack, long num, long value) {
	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack) return 1;

	num = *stack[stackstack] - num;
	if (num < 1 || num > *stack[stackstack]) return 1;

	stack[stackstack][num] = value;
	return 0;
}

int fst_copycreatestack(long **stack, long numvals) {
	long i, z;

	stack = realloc(stack, sizeof(*stack)*(++(*(long*)stack) + 1));
	stack[*(long*)stack] = malloc(sizeof(*stack[*(long*)stack]));
	*stack[*(long*)stack] = 0;

	if (numvals < 0) {
		z = -numvals;
		i = 0;
	} else {
		z = 0;
		i = numvals;
	}

	for (; i > 0; i--)
		fst_push(&stack, 0, fst_peek(&stack, 1, i-1));

	for (i = 0; i < z; i++)
		fst_push(&stack, 0, 0);

	return 0;
}

int fst_copydeletestack(long **stack, long numvals) {
	long stackstack = *(long*)stack, soss = stackstack - 1, i, z;

	if (soss < 1) return 1;

	free(stack[soss]);
	stack[soss] = malloc(sizeof(*stack[soss]));
	*stack[soss] = 0;

	i = numvals;
	if (i < 0) { z = -i; i = 0; } else z = 0;

	if (numvals < 0) {
		z = -numvals;
		i = 0;
	} else {
		z = 0;
		i = numvals;
	}

	for (; i > 0; i--)
		fst_push(&stack, 1, fst_peek(&stack, 0, i-1));

	for (i = 0; i < z; i++)
		fst_push(&stack, 0, 0);

	stack = realloc(stack, sizeof(*stack)*(--(*(long*)stack) + 1));

	return 0;
}

long fst_stack_size(long **stack, long stackstack) {
	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack) return 0;

	return *stack[stackstack];
}

void fst_clear_stack(long **stack, long stackstack) {
	stackstack = *(long*)stack - stackstack;
	if (stackstack < 1 || stackstack > *(long*)stack) return;

	stack[stackstack] = realloc(stack[stackstack], sizeof(*stack[stackstack]));
	*stack[stackstack] = 0;
}

long **fst_duplicate_stack(long **stack) {
	long **newstack, i;

	newstack = malloc(sizeof(*newstack) * (*(long*)stack + 1));
	*(long*)newstack = *(long*)stack;
	for (i = 1; i <= *(long*)stack; i++) {
		newstack[i] = malloc((*stack[i] + 1) * sizeof(*newstack[i]));
		memcpy(newstack[i], stack[i], (*stack[i] + 1) * sizeof(*newstack[i]));
	}

	return newstack;
}

void fst_delete_stackstack(long **stack) {
	long i;
	for (i = 1; i <= *(long*)stack; i++) free(stack[i]);
	free(stack);
}

/*
#undef stack

#define showstack()	{for (i = 0; i < 20 && i < *stack[*(long*)stack]; i++)\
				printf("%3d ", fst_peek(&stack, 0, i));\
			printf("\n");}

long **stack;

int main() {
	long i;

	fst_init(&stack);

	for (i = 0; i < 9; i++)
		fst_push(&stack, 0, i);

	showstack();

	fst_copycreatestack(&stack, 12);

	showstack();

	fst_copydeletestack(&stack, 5);

	showstack();
}
*/
