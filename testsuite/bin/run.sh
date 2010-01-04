#!/bin/bash
#
# This script run the dibol testsuite.
# It take all .dbl file.
# In the results subdirectory, you get :
#  - result.-timestamp-.csv which contains return code for all step of each program
#  - a subdirectory for each program with the output of each step
#

# Steps necessary to compile a dibol program
# We'll run theses command for each .dbl file

#INCLUDE=-I include/

# Step 1 : preprocessor
step_cmd[0]='dpp $INCLUDE $basename.dbl > $basename.dpp'
# Step 2 : translator
step_cmd[1]='d2c $basename.dpp'
# Step 3 : indentation
step_cmd[2]='indent $basename.c'
# Step 4 : compilation
step_cmd[3]='gcc -c $basename.c `glib-config --cflags`'
# Step 5 : linkage
# TODO
# Step 6 : run
# TODO
# Step 7 : run under valgrind
# TODO
# Step 8 : compare output
# TODO

date=$(date +%s)

resultsdir=$(pwd)/results/
mkdir -p results/

nb_test=0
for dbl in `find . -name '*.dbl' | sort`; do
	let nb_test+=1

	rc_step[0]=-1
	rc_step[1]=-1
	rc_step[2]=-1
	rc_step[3]=-1

	echo ==== $dbl
	test_source_file=$(basename $dbl)
	basename=${test_source_file%.dbl}

	step=0
	rc=0
	rm -rf $resultsdir/$basename/
	mkdir -p $resultsdir/$basename/
	# Move to the directory where the dibol source live
	# to avoid problems with include dir
	pushd $(dirname $dbl)
	until [ "$rc" -ne 0 -o "$step" -gt 4 ]; do
		echo ${step_cmd[$step]}
		eval ${step_cmd[$step]} > $resultsdir/$basename/step.$step.output 2>&1
		rc=$?
		rc_step[$step]=$rc
		if [ $rc -eq 0 ]; then
			let step_success[$step]+=1
		else
			let step_failure[$step]+=1
		fi
		let step+=1
	done

	popd

	echo "$dbl;${rc_step[0]};${rc_step[1]};${rc_step[2]};${rc_step[3]}" >> $resultsdir/result.$date.csv
done
