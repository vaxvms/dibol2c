#!/bin/bash
# This script graph the result of the testsuite
# so we can see the evolution

rm graph.csv

for run in `ls -1 results/result.*.csv`; do
	timestamp=$(echo $run | cut -d. -f 2)
	nb_test=$(cat $run | wc -l)
	nb_pp_success=$(cat $run | awk -F";" '{ if ($2 == 0) print $0}' | wc -l)
	nb_trans_success=$(cat $run | awk -F";" '{ if ($3 == 0) print $0}' | wc -l)
	nb_indent_success=$(cat $run | awk -F";" '{ if ($4 == 0) print $0}' | wc -l)
	nb_cc_success=$(cat $run | awk -F";" '{ if ($5 == 0) print $0}' | wc -l)
	echo "$timestamp;$nb_test;$nb_pp_success;$nb_trans_success;$nb_indent_success;$nb_cc_success" >> graph.csv
done

gnuplot - << EOGP
set xlabel "Date"
set timefmt "%s"
set datafile separator ";"
set style data lines
set xdata time
set ylabel "Nb de tests"
set grid
set output "graph.png"
plot 'graph.csv' using 1:2 title "Nb total de tests"
replot 'graph.csv' using 1:3 title "preprocesseur"
replot 'graph.csv' using 1:4 title "traducteur"
replot 'graph.csv' using 1:5 title "indent"
replot 'graph.csv' using 1:6 title "gcc"
pause 100
EOGP
