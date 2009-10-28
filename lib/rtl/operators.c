/**************************************************************************
 * operator.c from libdibol
 *
 * (c) 2001 Nicolas DUPEUX                      nicolas@dupeux.net
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 **************************************************************************/

#include "dibol.h"
#include <math.h>
#include <stdlib.h>

/*--------------------------------------------------------------------*/

/*
	calculate base^power
*/
int64 powint(int64 base,int power)
{
	int i;
	int64 result=base;
	for (i=0 ; i<power-1 ; i++)
		result*= base;
	return result;
}

/*
	Convert a DIBOL decimal to int64
*/
int64 decimal2integer(variable* var)
{
	int i;
	int64 value=0;
	char* data=getdata(var);

	for ( i=0 ; i<getsize(var) ; i++) {
		if ((*data >= '0') && (*data <= '9'))
			value=value*10+(*data-48);
		else if ((*data >= 'p') && (*data <= 'y') && (i==getsize(var)-1)) {
			value=value*10+(*data-112);
			value=-value;
		} else if (*data != ' ')
			error(20);
		data++;
	}

	return value;
}

/*
	Convert a int64 to DIBOL decimal format
*/
void integer2decimal(variable* var, int64 value)
{
	char* tmp;
	int size=getsize(var);

	asprintf(&tmp,"%0*i",size,llabs(value));
	memcpy(getdata(var),tmp,getsize(var));
	if (value<0) 
		*(getdata(var)+getsize(var)-1)+=64;

	free(tmp);
}

/*
	Convert a DIBOL alpha to char
	return a pointer to a newly allocated string
*/
char* alpha2char(variable* var)
{
	char* tmp;
	char* data=getdata(var);
	int i=getsize(var)-1;
	tmp=(char*)malloc((i+1)*sizeof(char));
	while ((data[i]==32) && (i>=0))
		i--;
	tmp[i+1]=0;
	while (i>=0) {
		tmp[i]=data[i];
		i--;
	}
	return tmp;
}

/*--------------------------------------------------------------------*/

/*
	just return the variable !
*/
variable* plus(variable* var)
{
	if ((gettype(var)!=tDECIMAL))
		return NULL;

	return imm(tDECIMAL,getsize(var),getdata(var));
}

/*
	return the opposite of the given variable
*/
variable* minus(variable* var)
{
	int64 calcul;
	char* result;
	if ((gettype(var)!=tDECIMAL))
		return NULL;
	calcul=-decimal2integer(var);
	integer2decimal(var,calcul);
	//asprintf(&result,"%d",calcul);
	return var;
	//return imm(tDECIMAL,strlen(result),result);
}

/*
	boolean not on var
*/
variable* not(variable* var)
{
	int64 number;
	if ((gettype(var)!=tDECIMAL))
		return NULL;
	number=decimal2integer(var);
	if (number==0)
		return imm(tDECIMAL,1,"1");
	else
		return imm(tDECIMAL,1,"0");
}

/*
	add two variables
*/
variable* add(variable* var1, variable* var2)
{
	int64 calcul;
	char* result;
	variable* var;
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;

	calcul=decimal2integer(var1)+decimal2integer(var2);
	asprintf(&result,"%d",calcul);
	//return imm(tDECIMAL,strlen(result),result);
	var=imm(tDECIMAL,strlen(result),result);
	free(result);
	return var;
}

/*
	subtract two variables
*/
variable* sub(variable* var1, variable* var2)
{
	int64 calcul;
	char* result;
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;

	calcul=decimal2integer(var1)-decimal2integer(var2);
	asprintf(&result,"%d",calcul);
	return imm(tDECIMAL,strlen(result),result);
}

/*
	Suppress the var2 left-most digit of var1 and round
*/
variable* rounding(variable* var1, variable* var2)
{
	int64 calcul;
	char* result;
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;

	calcul=llround(1.0*decimal2integer(var1)/(powint(10,decimal2integer(var2))));
	asprintf(&result,"%d",calcul);
	return imm(tDECIMAL,strlen(result),result);
}

/*
	multiply var1 with var2
*/
variable* mul(variable* var1, variable* var2)
{
	int64 calcul;
	char* result;
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;

	calcul=decimal2integer(var1)*decimal2integer(var2);
	asprintf(&result,"%d",calcul);
	return imm(tDECIMAL,strlen(result),result);
}

/*
	divide var1 by var2
*/
variable* divB(variable* var1, variable* var2)
{
	int64 calcul;
	char* result;
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;

	calcul=decimal2integer(var1)/decimal2integer(var2);
	asprintf(&result,"%d",calcul);
	return imm(tDECIMAL,strlen(result),result);
}

/*
	test if var1 == var2
*/
variable* eq(variable* var1, variable* var2)
{
	/* decimal comparaison */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1)==decimal2integer(var2))
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}

	/* alpha comparaison */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) < getsize(var2) ? getsize(var1) : getsize(var2))==0)
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}
	return NULL;
}

/*
	test if var1 != var2
*/
variable* ne(variable* var1, variable* var2)
{
	/* decimal comparaison */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1)==decimal2integer(var2))
			return imm(tDECIMAL,1,"0");
		else
			return imm(tDECIMAL,1,"1");
	}

	/* alpha comparaison */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) < getsize(var2) ? getsize(var1) : getsize(var2))==0)
			return imm(tDECIMAL,1,"0");
		else
			return imm(tDECIMAL,1,"1");
	}
	return NULL;
}

/*
	test if var1 > var2
*/
variable* gt(variable* var1, variable* var2)
{
	// decimal comparaison
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1) > decimal2integer(var2))
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}

	// alpha comparaison
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) < getsize(var2) ? getsize(var1) : getsize(var2)) > 0)
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}
	return NULL;
}

/*
	test if var1 < var2
*/
variable* lt(variable* var1, variable* var2)
{
	/* decimal comparaison */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1) < decimal2integer(var2))
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}

	/* alpha comparaison */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) > getsize(var2) ? getsize(var1) : getsize(var2)) < 0)
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}
	return NULL;
}

/*
	test if var1 >= var2
*/
variable* ge(variable* var1, variable* var2)
{
	/* decimal comparaison */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1) >= decimal2integer(var2))
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}

	/* alpha comparaison */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) <= getsize(var2) ? getsize(var1) : getsize(var2)) >= 0)
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}
	return NULL;
}

/*
	test if var1 <= var2
*/
variable* le(variable* var1, variable* var2)
{
	// decimal comparaison
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL)) {
		if (decimal2integer(var1) <= decimal2integer(var2))
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}

	// alpha comparaison
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA)) {
		if (memcmp(getdata(var1),getdata(var2),getsize(var1) >= getsize(var2) ? getsize(var1) : getsize(var2)) <= 0)
			return imm(tDECIMAL,1,"1");
		else
			return imm(tDECIMAL,1,"0");
	}
	return NULL;
}

/* 
	/!\	Booleans operations implies that 0 is false
		and anything else is true.
*/

/*
	var1 or var2
*/
variable* or(variable* var1, variable* var2)
{
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;
	if ((decimal2integer(var1) || decimal2integer(var2)) == 0)
		return imm(tDECIMAL,1,"0");
	else
		return imm(tDECIMAL,1,"1");
}

/*
	var1 xor var2
*/
variable* xor(variable* var1, variable* var2)
{
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;
	if ((decimal2integer(var1) ^ decimal2integer(var2)) == 0)
		return imm(tDECIMAL,1,"0");
	else
		return imm(tDECIMAL,1,"1");
}

/*
	var1 and var2
*/
variable* and(variable* var1, variable* var2)
{
	if ((gettype(var1)!=tDECIMAL) || (gettype(var2)!=tDECIMAL))
		return NULL;
	if ((decimal2integer(var1) && decimal2integer(var2)) == 0)
		return imm(tDECIMAL,1,"0");
	else
		return imm(tDECIMAL,1,"1");
}

/*
 * Assign var2 to var1
 */
void assign(variable* var1, variable* var2)
{
	/* Assign a alpha to an alpha */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)<=tALPHA))
	{
		if (getsize(var1) > getsize(var2)) {
			// If the destination is longer than the source, we add space at the end
			memcpy(getdata(var1),getdata(var2),getsize(var2));
			memset(getdata(var1)+getsize(var2),32,getsize(var1)-getsize(var2));
		} else {
			// If the destination is shorter than the source, we copy the rightmost digit
			memcpy(getdata(var1),getdata(var2),getsize(var1));
		}
		return;
	}

	/* Assign a decimal to a decimal */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)==tDECIMAL))
	{
		if (getsize(var1) > getsize(var2)) {
			memcpy(getdata(var1)+getsize(var1)-getsize(var2),getdata(var2),getsize(var2));
			memset(getdata(var1),'0',getsize(var1)-getsize(var2));	
		} else {
			memcpy(getdata(var1),getdata(var2)+getsize(var2)-getsize(var1),getsize(var1));
		}
		return;
	}

	/* Assign a alpha to a decimal */
	if ((gettype(var1)==tDECIMAL) && (gettype(var2)<=tALPHA))
	{
		int sign=1;
		int value=0;
		int i;
		char* data=getdata(var2);
		char* tmp;

		/* count digit sign */
		for (i=0 ; i<getsize(var2) ; i++) {
			if (((*data < '0') || (*data > '9')) && ((*data!=' ') || (*data!='+') || (*data!='-'))) error(20);
			if (*data=='-') sign*=-1;
			if (*data=='+') sign*=1;
			if ((*data >= '0') && (*data <= '9')) break;
			data++;
		}
		/* calculate the value */
		for ( ; i<getsize(var2) ; i++) {
			if ( ((*data < '0') || (*data > '9')) && (*data!=' ')) error(20); 
			if ((*data >= '0') && (*data <= '9')) value=value*10+(*data-48);
			data++;
		}

		/* Format the value */
		asprintf(&tmp,"%018d",value);

		/* Add the sign if necessary */
		if (sign<0) tmp[17]+=64;

		memcpy(getdata(var1),(char*)tmp+18-getsize(var1),getsize(var1));

		free(tmp);

		return;
	}

	/* Assign a decimal to a alpha */
	if ((gettype(var1)<=tALPHA) && (gettype(var2)==tDECIMAL))
	{
		int64 value;
		char* tmp;

		value=decimal2integer(var2);

		asprintf(&tmp,"%i",value);

		if (getsize(var1) > strlen(tmp)) {
			memcpy(getdata(var1)+getsize(var1)-strlen(tmp),tmp,strlen(tmp));
			memset(getdata(var1),' ',getsize(var1)-strlen(tmp));	
		} else {
			memcpy(getdata(var1),tmp+strlen(tmp)-getsize(var1),getsize(var1));
		}

		free(tmp);
	}
}
