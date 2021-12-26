#! /bin/bash
id_arr=()
deg_arr=()
inst_arr=()

declare -A visited

filename="$1"
index=$((0))
while IFS="," read -r id deg inst
do
    if [ $index -eq 0 ]; then
        index=$(($index+1))
    else
        echo "id: $id"
        echo "deg: $deg"
        echo "inst: $inst"
        id_arr[$index]=$(($id))
        deg_arr[$index]="$deg";
        inst_arr[$index]="$inst" 
        index=$(($index+1))
    fi
done < $filename

n=$(($index))
echo "n=$n"
flag=1;
for (( i = 1; i < $n; i++ ))
do
    flag=0;
    for ((j = 1; j < $n-$i; j++ ))
    do
        if [[ ${id_arr[$j]} -gt ${id_arr[$j+1]} ]]
        then
            temp=${id_arr[$j]};
            id_arr[$j]=${id_arr[$j+1]};
            id_arr[$j+1]=$temp;

            temp=${deg_arr[$j]};
            deg_arr[$j]=${deg_arr[$j+1]};
            deg_arr[$j+1]=$temp;

            temp=${inst_arr[$j]};
            inst_arr[$j]=${inst_arr[$j+1]};
            inst_arr[$j+1]=$temp;
            flag=1;
        fi
    done

    if [[ $flag -eq 0 ]]; then
          break;
    fi
done
echo "${id_arr[@]}"
echo "${deg_arr[@]}"
echo "${inst_arr[@]}"

echo "student id,no of degree,degree,institution">>output.csv
for (( i = 1; i < $n; i++ ))
do
    echo "i=$i id:${id_arr[$i]} deg:${deg_arr[$i]} inst:${inst_arr[$i]}"
    cnt=$((0))
    for (( j = i + 1; j < $n; j++ ))
    do
        if [ ${id_arr[$i]} -eq ${id_arr[$j]} ]; then
            cnt=$(($cnt+1))
        else
            break
        fi
    done
    echo "${id_arr[$i]},$(($cnt+1)),${deg_arr[$i]},${inst_arr[$i]}">>output.csv
    if [ $cnt -gt 0 ]; then
        for (( k = 0; k < $cnt; k++))
        do
            echo ",,${deg_arr[$i+$k+1]},${inst_arr[$i+$k+1]}">>output.csv
        done
    fi
    i=$(($i+$cnt))
    echo "cnt=$cnt i=$i"
done