#!/bin/bash
echo "is it morning? please answer yes or no"
read timeofday
case "$timeofday" in
	yes) echo "good morning";;
	no) echo "good afternoon";;
	y) echo "good monring";;
	n) echo "good afternoon";;
	*) echo "ans not recognized";;
esac
