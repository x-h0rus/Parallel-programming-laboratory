#!/usr/bin/env bash
size=1000
size2=500000
size3=955000000
op=XOR
nthreads=7
case "$1" in
	st1)
		./prac1 $size $op
		;;
	mt1)
		./prac1 $size $op --multi-thread $nthreads
		;;
	st2)
		./prac1 $size2 $op
		;;
	mt2)
		./prac1 $size2 $op --multi-thread $nthreads
		;;
	st3)
		./prac1 $size3 $op
		;;
	mt3)
		./prac1 $size3 $op --multi-thread $nthreads
		;;
		
esac
