#!/bin/bash
showarg()
{
	a=1
	for i in $*
		do
			echo "the $a no arg is $i"
			a=`expr $a + 1`
		done
}
echo "listing start"
showarg $*
echo "total: $#"
echo "listing end"
