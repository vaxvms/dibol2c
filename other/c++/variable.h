#ifndef VARIABLE_H
#define VARIABLE_H

#define tRECORD		1
#define tALPHA		2
#define tDECIMAL	3

#include <iostream.h>

typedef long long int64;

class Variable {
public:
	Variable();
	Variable(Variable& var);
	Variable(char* alpha);
	Variable(int64 decimal);
	Variable(int type, int size, int nbelts, char* data);
	virtual ~Variable();

	inline int gettype();
	inline int getsize();
	inline int getnbelts();
	inline char* getdata();

	void fromint(int64 value);

	operator int64 ();

	Variable& operator=(Variable& rhs);

	Variable& operator+=(Variable& rhs);
	Variable& operator-=(Variable& rhs);
	Variable& operator*=(Variable& rhs);
	Variable& operator/=(Variable& rhs);

	friend Variable operator+(Variable& lhs, Variable& rhs);
	friend Variable operator-(Variable& lhs, Variable& rhs);
	friend Variable operator*(Variable& lhs, Variable& rhs);
	friend Variable operator/(Variable& lhs, Variable& rhs);

	bool operator==(Variable& rhs);
	bool operator!=(Variable& rhs);
	bool operator<(Variable& rhs);

	friend ostream& operator<<(ostream& ostr, Variable& var);
private:
	char* _data;
	int _nbelts;
	int _size;
	int _type;
};

#endif
