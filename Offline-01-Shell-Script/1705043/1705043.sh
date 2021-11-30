#!/bin/bash

readFile(){
	filename=$1
	n=0
	while read line; do
		echo "$line"
		ARRAY[$n]=$line
		n=$((n+1))
		done < $filename
}

skipFile(){
	filename=$1
	present=0
	for i in "${ARRAY[@]}"
	do
		if [ -f "$filename" ]; then
			if [ "${filename##*.}" = "$i" ]; then
				present=1
			fi
		fi
	done
}


tree_visiting(){
	cd "$1"
	for f in *
	do 
		if [ -d "$f" ]; then
			x=$(expr $2 + 1)
			echo "now in $f"
			tree_visiting "$f" $x
		elif [ -f "$f" ]; then
			echo "now in $f"
			extension="${f##*.}"

			skipFile $f
			if [ $present = 0 ]; then
				mkdir -p "$HOME/Documents/$extension"
				mv "$f" "$HOME/Documents/$extension/$f"
				echo "$(realpath $f)" >> "$HOME/Documents/$extension/$extension.txt"
				echo "matched with extention"
			else
				echo "skipping"
			fi
		fi
	done
	cd ../
}

echo "start"


echo "reading file $1"
readFile "$1"
echo "reading file $1 completed"


echo "printing array"
for i in "${ARRAY[@]}"
	do
		echo "$i"
	done
echo "printing array completed"


tree_visiting $2 1
echo "finish"
