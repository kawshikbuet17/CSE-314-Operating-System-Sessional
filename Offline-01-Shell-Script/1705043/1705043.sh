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
				mkdir -p "$HOME/Documents/new_dir/$extension"
				mv "$f" "$HOME/Documents/new_dir/$extension/$f"
				echo "$(realpath $f)" >> "$HOME/Documents/new_dir/$extension/$extension.txt"
				echo "matched with extention"
			else
				echo "skipping"
			fi
		fi
	done
	cd ../
}

counting_files(){
	cnt=0
	dir=$1
	cd "$dir"
	for f in *
	do
		if [ -f "$f" ]; then
			cnt=$(expr $cnt + 1)
		fi
	done
	cnt=$(expr $cnt - 1)
	cd ../
}


echo "start"

mkdir -p "$HOME/Documents/new_dir"
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
echo "finished transfer"

cd "$HOME/Documents/new_dir"
for f1 in *
do
	if [ -d "$f1" ]; then
		counting_files $f1
		echo "$f1,$cnt" >> "list.csv"
	fi
done


