#CXX=g++ -g -Wall -W -Wstrict-prototypes -felide-constructors
CXX=g++ -g
all: test

variable.o: variable.cpp variable.h
	$(CXX) -c variable.cpp -DTEST

test: variable.o
	$(CXX) variable.o
