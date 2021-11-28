#!/bin/bash
yes_or_no(){
	echo "is your name $*?"
	echo "enter yes or no"
	read x
	case "$x" in
		y | yes) return 0;;
		n | no) return 1;;
	esac
}

if yes_or_no "$1"
	then
		echo "hi $1, nice name"
	else
		echo "never mind"
fi
