#!/bin/bash
keywords=()
readFile(){
	filename="$1"
	n=0
	while read line; do
		echo "$line"
		keywords[$n]="$line"
		n=$((n+1))
        mkdir -p "output_dir/$line"
		done < "$filename"
}



directory_tree(){
	cd "$1"
	for f in *
	do
		if [ -d "$f" ]; then
			x=$(expr $2 + 1)
			directory_tree "$f" $x $3
		elif [ -f "$f" ]; then
            for j in "${keywords[@]}" #"${!keywords[@]}"
			do
                echo "$f $j"
                var="$(grep -o $j $f | wc -l)"
                echo $var
                if [ $var != "0" ]; then
					echo $3
                    cp "$f" "$3/output_dir/$j/"
                    echo "copied"
                fi
            done
		fi
	done
	cd ../
}

readFile "$1" 
echo "${keywords[@]}"
directory_tree $2 1 ${PWD}
