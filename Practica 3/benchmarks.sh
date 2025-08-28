#!/usr/bin/env bash
file=matmul_sup_static_tam_8_threads_results.log
touch $file
for season in 1; do
	for benchcase in t1 t2 t3 t4 t5 t6 t7 t8 t9 t10; do
		echo $benchcase >> $file
		for i in `seq 1 1 10`;
		do
			./benchsuite.sh $benchcase >> $file # results dumped
		done
	done
done

