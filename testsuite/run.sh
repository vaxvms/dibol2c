#!/bin/bash

# This script run the test suite for d2c

find . -name "*.dbl" | while read line; do

	testsrc=$(basename $line)
	test=${testsrc%.dbl}
	pushd $(dirname $line)

# translation
	d2c $testsrc > /dev/null
	rc=$?
	translate_step=$rc

# identation of the resulting source code
	indent $test.c
	rc=$?
	indent_step=$rc

# compilation and linking of the translated code
	gcc -o $test $test.c `glib-config --cflags --libs` -ldibolcommon -ldibol -lmysqlclient
	rc=$?
	compile_step=$rc

# Run the program, capture terminal output
	./$test > $test.run
	rc=$?
	run_step=$rc
	valgrind --error-exitcode=29 --log-file=$test.valgrind ./$test > $test.run
	rc=$?
	valgrind_step=$rc

# Check if terminal output is what we should get
	diff $test.run $test.output > /dev/null
	rc=$?
	output_step=$rc

	popd

	echo "$test;$translate_step;$indent_step;$compile_step;$run_step;$valgrind_step;$output_step"
done
