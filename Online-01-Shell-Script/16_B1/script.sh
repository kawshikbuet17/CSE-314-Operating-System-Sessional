courses=()
studentids=()
declare -A mp

readFile(){
	filename="$1"
	n=0
	while read line; do
		echo "$line"
		courses[$n]="$line"
		n=$((n+1))
		done < "$filename"
}

readFile "$1"
echo "${courses[@]}"
echo "n=$n"
cntfile=$((n-1))
for (( i = 0; i < $cntfile; i++ ))
do
    filename="${courses[$i]}.txt"
    n=0
	while IFS=" " read -r studentid mark; do
		echo "|$studentid|$mark|"
        if (( ${#studentid} == 0 )); then
            continue
        fi
        studentids[$n]=$(($studentid))
        if [ "${mp[$studentid]}" ]; then
            mp[$studentid]=$(($mark+${mp[$studentid]}))
        else
            mp[$studentid]=$(($mark))
        fi
        n=$(($n+1))
		done < "$filename"
    for j in ${!mp[@]}
    do
        echo "$i ${mp[$j]}"
    done 
done

echo "all done"
for (( i = 0; i < $n; i++ ))
do
    echo "subject: $cntfile"
    echo "${studentids[$i]} -> ${mp[${studentids[$i]}]}"
    echo "${studentids[$i]} -> $((${mp[${studentids[$i]}]}/$cntfile))"
done