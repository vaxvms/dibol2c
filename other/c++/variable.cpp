#define _GNU_SOURCE 1

#include "variable.h"
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Stub for compilation
void error(int code)
{
	cerr << "Error code " << code << endl;
}

Variable::Variable()
{
	_nbelts=1;
	_data=NULL;
}

Variable::Variable(Variable& var)
{
	_data=strdup(var.getdata());
	_nbelts=var.getnbelts();
	_size=var.getsize();
	_type=var.gettype();
}

Variable::Variable(int64 decimal)
{
	_nbelts=1;
	_type=tDECIMAL;
	_size=asprintf(&_data,"%Li",decimal);
}

Variable::Variable(char* alpha)
{
	_nbelts=1;
	_type=tALPHA;
	_size=strlen(alpha);
	_data=strdup(alpha);
}

Variable::Variable(int type, int size, int nbelts, char* data)
{
	_type=type;
	_size=size;
	_nbelts=nbelts;
	_data=strdup(data);
}

Variable::~Variable()
{
}

int
Variable::gettype()
{
	return _type;
}

int
Variable::getsize()
{
	return _size;
}

int
Variable::getnbelts()
{
	return _nbelts;
}

char*
Variable::getdata()
{
	return _data;
}

void
Variable::fromint(int64 value)
{
	char* tmp;
	int size=getsize();

	asprintf(&tmp,"%0*Li",size,value);
	memcpy(getdata(),tmp,size);

	free(tmp);
}

Variable::operator int64()
{
	int i;
	int64 value=0;
	char* data=getdata();

	for ( i=0 ; i<getsize() ; i++) {
		if ((*data >= '0') && (*data <= '9'))
			value=value*10+(*data-48);
		else if ((*data >= 'p') && (*data <= 'y') && (i==getsize()-1)) {
			value=value*10+(*data-112);
			value=-value;
		} else
			error(20);
		data++;
	}

	return value;
}

Variable&
Variable::operator=(Variable& rhs)
{
	/* Assign a alpha to an alpha */
	if ((gettype()<=tALPHA) && (rhs.gettype()<=tALPHA))
	{
		if (getsize() > rhs.getsize()) {
			// If the destination is longer than the source, we add space at the end
			memcpy(getdata(),rhs.getdata(),rhs.getsize());
			memset(getdata()+rhs.getsize(),32,getsize()-rhs.getsize());
		} else {
			// If the destination is shorter than the source, we copy the rightmost digit
			memcpy(getdata(),rhs.getdata(),getsize());
		}
		return (*this);
	}

	/* Assign a decimal to a decimal */
	if ((gettype()==tDECIMAL) && (rhs.gettype()==tDECIMAL))
	{
		if (getsize() > rhs.getsize()) {
			memcpy(getdata()+getsize()-rhs.getsize(),rhs.getdata(),rhs.getsize());
			memset(getdata(),'0',getsize()-rhs.getsize());
		} else {
			memcpy(getdata(),rhs.getdata()+rhs.getsize()-getsize(),getsize());
		}
		return (*this);
	}

	/* Assign a alpha to a decimal */
	if ((gettype()==tDECIMAL) && (rhs.gettype()<=tALPHA))
	{
		int sign=1;
		int64 value=0;
		int i;
		char* data=rhs.getdata();
		char* tmp;

		/* count digit sign */
		for (i=0 ; i<rhs.getsize() ; i++) {
			if (((*data < '0') || (*data > '9')) && ((*data!=' ') || (*data!='+') || (*data!='-'))) error(20);
			if (*data=='-') sign*=-1;
			if (*data=='+') sign*=1;
			if ((*data >= '0') && (*data <= '9')) break;
			data++;
		}
		/* calculate the value */
		for ( ; i<rhs.getsize() ; i++) {
			if ( ((*data < '0') || (*data > '9')) && (*data!=' ')) error(20);
			if ((*data >= '0') && (*data <= '9')) value=value*10+(*data-48);
			data++;
		}

		/* Format the value */
		asprintf(&tmp,"%018L",value);

		/* Add the sign if necessary */
		if (sign<0) tmp[17]+=64;

		memcpy(getdata(),(char*)tmp+18-getsize(),getsize());

		free(tmp);

		return(*this);
	}

	/* Assign a decimal to a alpha */
	if ((gettype()<=tALPHA) && (rhs.gettype()==tDECIMAL))
	{
		int64 value;
		char* tmp;

		value=(int)rhs;

		asprintf(&tmp,"%L",value);

		if (getsize() > strlen(tmp)) {
			memcpy(getdata()+getsize()-strlen(tmp),tmp,strlen(tmp));
			memset(getdata(),' ',getsize()-strlen(tmp));
		} else {
			memcpy(getdata(),tmp+strlen(tmp)-getsize(),getsize());
		}

		free(tmp);
	}

	return (*this);
}

Variable&
Variable::operator+=(Variable& rhs)
{
	if ( (gettype()==tDECIMAL) && (rhs.gettype()==tDECIMAL)) {
		// Add two decimal variables
		int64 calcul;
		calcul=(int64)(*this)+(int64)rhs;
		this->fromint(calcul);
	} else
	if ((gettype()<=tALPHA) && (rhs.gettype()<=tALPHA)) {
		// Concat two alpha or record
		// TODO
	} else
		error(20);

	return (*this);
}

Variable
operator+(Variable& lhs,Variable& rhs)
{
	return Variable(lhs) += rhs;
}

bool
Variable::operator==(Variable& rhs)
{
	/* decimal comparaison */
	if ((gettype()==tDECIMAL) && (rhs.gettype()==tDECIMAL)) {
		if ((int64)(*this)==(int64)(rhs))
			return true;
		else
			return false;
	}

	/* alpha comparaison */
	if ((gettype()<=tALPHA) && (rhs.gettype()<=tALPHA)) {
		if (memcmp(getdata(),rhs.getdata(),getsize() < rhs.getsize() ? getsize() : rhs.getsize())==0)
			return false;
		else
			return true;
	}
	return false;
}

bool
Variable::operator!=(Variable& rhs)
{
	return !(operator==(rhs));
}

ostream&
operator<<(ostream& ostr, Variable& var)
{
	ostr.write(var.getdata(),var.getsize());
	return (ostr);
}

#ifdef TEST

int main(int argc, char** argv)
{
	Variable* a=new Variable(tDECIMAL,5,1,"12345");
	Variable* b=new Variable(tDECIMAL,5,1,"54321");
	Variable c(tDECIMAL,2,1,"10");

	cout << "a==" << *a << endl;
	cout << "b==" << *b << endl;
	cout << "c==" << c << endl;
	*b=*a;
	cout << "b=a ; b==" << *b << endl;
	cout << "a+b==" << (*a)+(*b) << endl;
	cout << "c+40==" << c+40 << endl;
	if (c==10) {
		cout << "c vaut 10 !!!" << endl;
	} 

	if (c!=15) {
		cout << "c ne vaut pas 15 !!!" << endl;
	}

	delete b;
	delete a;
}

#endif
