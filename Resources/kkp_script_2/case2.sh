#!/bin/bash
echo "is it morning? please answer yes or no"
read timeofday
case "$timeofday" in
	yes | y | YES | Yes) echo "good morning";;
	n* | N* ) echo "good afternoon";;
	y) echo "good monring";;
	*) echo "ans not recognized";;
esac
