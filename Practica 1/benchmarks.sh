#!/usr/bin/env bash
file=7_xor_results.log
touch $file
for season in 1; do
	for benchcase in mt1 mt2 mt3; do
		echo $benchcase >> $file
		for i in `seq 1 1 10`;
		do
			./benchsuite.sh $benchcase >> $file # results dumped
			sleep 1
		done
	done
done

