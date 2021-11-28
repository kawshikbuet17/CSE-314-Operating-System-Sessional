#!/bin/bash
password="abc"
echo "enter password"
read pass 
while [ $pass != $password ]
	do
		echo "wrong password, try again"
		read pass
	done
	echo "right password"
