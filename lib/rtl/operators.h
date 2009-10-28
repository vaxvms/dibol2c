#ifndef OPERATORS_H
#define OPERATORS_H

#include "dibol.h"

/* 64 bit integer to store decimal because 32 bits are not enough to
 * store a 18 digits decimal */
typedef long long int int64;

int64 powint(int64 base,int power);
int64 decimal2integer(variable* var);
void integer2decimal(variable* var, int64 value);
char* alpha2char(variable* var);

variable* plus(variable* var);
variable* minus(variable* var);
variable* not(variable* var);

variable* add(variable* var1, variable* var2);
variable* sub(variable* var1, variable* var2);
variable* rounding(variable* var1, variable* var2);
variable* mul(variable* var1, variable* var2);
variable* divD(variable* var1, variable* var2);
variable* eq(variable* var1, variable* var2);
variable* ne(variable* var1, variable* var2);
variable* gt(variable* var1, variable* var2);
variable* lt(variable* var1, variable* var2);
variable* ge(variable* var1, variable* var2);
variable* le(variable* var1, variable* var2);
variable* or(variable* var1, variable* var2);
variable* xor(variable* var1, variable* var2);
variable* and(variable* var1, variable* var2);

variable* formatD(variable* nexp, variable* format_string);

void assign(variable* var1, variable* var2);

#endif
