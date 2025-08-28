#!/usr/bin/env bash
file=parallel_8_results.log
touch $file
for season in 1; do
	for benchcase in t1 t2 t3; do
		echo $benchcase >> $file
		for i in `seq 1 1 10`;
		do
			./benchsuite.sh $benchcase >> $file # results dumped
		done
	done
done

