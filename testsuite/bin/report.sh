#!/bin/bash
#
# This script create a report of the latest run of
# the dibol testsuite
#

RESULTFILE=$(ls -1tr results/result.*.csv | tail -n 1)

awk -F';' '{
count+=1
for (i=2 ; i <=5; i++) {
	if ($i == 0) a[i]+=1
	if ($i > 0) b[i]+=1
}
}END{
OFS=";"
ORS=";"
print count
for (i=2 ; i <= 5 ; i++) {
print a[i],b[i]
}}' $RESULTFILE | ( IFS=";" read nb_test pp_success pp_failure t_success t_failure i_success i_failure c_success c_failure;

# Print a resume of the testsuite run
echo "========================================="
tee results/report.txt << EOR
Number of test program   : $nb_test
Preprocessor sucess/fail : $pp_success/$pp_failure
Translator sucess/fail   : $t_success/$t_failure
Indentation sucess/fail  : $i_success/$i_failure
Compilation sucess/fail  : $c_success/$c_failure
EOR

)
