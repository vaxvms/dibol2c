#ifndef STACK_H
#define STACK_H

#include <glib.h>

typedef GSList Stack;

/* Add an element on top of the stack */
void push(Stack** stack, void* element);

/* Remove the element placed on top of the stack and return it */
void* pop(Stack** stack);

/* Return the element on top of the stack */
void* top(Stack** stack);

#endif
