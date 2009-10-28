#ifndef CALL_H
#define CALL_H

#include <setjmp.h>

typedef struct _call_stack {
	jmp_buf ctx;
	int xcall;
	struct _call_stack *super;
} call_stack;

call_stack* _call;

void push_call();
void pop_call();

#define CALL(LABEL) \
	push_call(); \
	if (setjmp(_call->ctx)==0) goto LABEL

#define RETURN \
	if (_call->xcall==1) { \
		pop_call(); \
		return; \
	} else \
		pop_call()

#define XCALL \
	push_call(); \
	_call->xcall=1;

#endif
