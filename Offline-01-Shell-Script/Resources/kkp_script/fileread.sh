#!/bin/bash
filename=$1
n=0
while read line; do
	echo "line no. $((n+1)) : $line"
	n=$((n+1))
	ARRAY[$n]=$line
done < $filename
# echo ${ARRAY[*]}
