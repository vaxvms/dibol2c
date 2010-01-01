#!/bin/bash

# Return output of failed test for the latest run
# of the testsuite
#
# it use vim tab to display the output so you need
# a quite recent version (> 7.0)

function usage {
cat << EOU
 Return output of failed test for the latest run
 of the testsuite

Usage: $0 step

where step is 
 1 : preprocessor
 2 : translation
 3 : indentation
 4 : compilation
EOU
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi

#VIM=vim
VIM=gvim

let STEP=$1-1
let FIELD=$1+1
RESULTFILE=$(ls -1tr results/result.*.csv | tail -n 1)

cat $RESULTFILE | awk -F";" "{if (\$$FIELD > 0) print \$1}" | \
	sed "s#\.dbl#/step\.$STEP\.output#g" | \
	sed "s#^#results/#g" |
	xargs $VIM -R -n -p

