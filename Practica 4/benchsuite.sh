#!/usr/bin/env bash
size=10
size2=50
size3=100

case "$1" in
	t1)
		./vt $size
		;;
	t2)
		./vt $size2
		;;
	t3)
		./vt $size3
		;;

esac
