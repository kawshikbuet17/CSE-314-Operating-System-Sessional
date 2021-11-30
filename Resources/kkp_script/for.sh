#!/bin/bash
echo "the number of args is $#"
a=1
for i in $*
do
	echo "the $a No arg is $i"
	a=`expr $a + 1`
done

