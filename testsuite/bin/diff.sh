#!/bin/bash
# This script output tests failing between the two lastest
# run of the testsuite

PREVIOUS=$(ls -1tr results/result.*.csv | tail -n 2 | head -n 1)
LATEST=$(ls -1tr results/result.*.csv | tail -n 1 )

echo "Latest run"
echo -e "\tPrevious run"
comm -3 $LATEST $PREVIOUS
