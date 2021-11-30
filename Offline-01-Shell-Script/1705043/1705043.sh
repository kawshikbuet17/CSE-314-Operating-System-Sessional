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

print_array(){
	echo "printing array"
	for i in "${ARRAY[@]}"
	do
		echo "$i"
	done
	echo "printing array completed"
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
            
            case $f in
            *.* ) exist="1" ;;
            * ) exist="0" ;;
            esac
            
            if [ "$exist" = "0" ]; then
                no_of_skip=$((no_of_skip+1))
                mkdir -p "$HOME/Documents/new_dir/others"
				cp "$f" "$HOME/Documents/new_dir/others/$f"
				echo "$(realpath $f)" >> "$HOME/Documents/new_dir/others/desc_others.txt"
				echo "no extension"

			else
                present="0"
                for i in "${ARRAY[@]}"
                do
                    echo "filename:$f / extension:${f##*.} / array:$i"
                    if [ "${f##*.}" = "$i" ]; then
                        present="1"
                        break
                    fi
                done
        
                if [ "$present" == "0" ]; then
    				mkdir -p "$HOME/Documents/new_dir/$extension"
    				cp "$f" "$HOME/Documents/new_dir/$extension/$f"
    				echo "$(realpath $f)" >> "$HOME/Documents/new_dir/$extension/desc_$extension.txt"
                    echo "not skipping"
                else
                    echo "skipping"
                fi
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

main(){
	mkdir -p "$HOME/Documents/new_dir"
	echo "reading file $1"
	readFile "$1"
	echo "reading file $1 completed"
	no_of_skip=0
	print_array

	tree_visiting $2 1
	echo "finished transfer"

	cd "$HOME/Documents/new_dir"
	echo "file_type, no_of_files" >> "output.csv"
	for f1 in *
	do
		if [ -d "$f1" ]; then
			counting_files $f1
			echo "$f1,$cnt" >> "output.csv"
		fi
	done
	echo "ignored, $no_of_skip" >> "output.csv"
}

echo "start"
if [ -z "$1" ]
then
      echo "input file name not provided. write ./1705043.sh input.txt root_dir/"
else
	if [ $# -eq 1 ]; then
		if [ -e "$1" ]; then
			echo "$1 exists."
			main "$1" "."

		else
			echo "Provide valid file name"
			read filenamePrompt
			main "$filenamePrompt" "."
		fi
	elif [ $# -eq 2 ]; then
		if [ -e "$1" ]; then
			echo "$1 exists."
			main "$1" "$2"

		else
			echo "Provide valid file name"
			read filenamePrompt
			main "$filenamePrompt" "$2"
		fi
	fi
fi
 
