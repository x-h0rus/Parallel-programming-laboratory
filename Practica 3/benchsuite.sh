#!/usr/bin/env bash
size=1620

case "$1" in
	t1)
		./matmul_p $size 1
		;;
	t2)
		./matmul_p $size 10
		;;
	t3)
		./matmul_p $size 50
		;;
	t4)
		./matmul_p $size 75
		;;
	t5)
		./matmul_p $size 100
		;;
	t6)
		./matmul_p $size 200
		;;
	t7)
		./matmul_p $size 500
		;;
	t8)
		./matmul_p $size 1000
		;;
	t9)
		./matmul_p $size 1500
		;;
	t10)
		./matmul_p $size 2000
		;;
esac
