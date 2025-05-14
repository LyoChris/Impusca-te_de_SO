#!/bin/bash

if [ $# -eq 2 ]; then
    dir=$1
    star=$2
else
    echo "No argument recieved"
    exit 1
fi

nr_linii_txt=0

parc_director(){
    arg_dir=$1
    for fis in $arg_dir/* $arg_dir/.*; do
        cale=$fis
        if [ -d "$cale" ]; then
            parc_director $cale
        fi
        if [ -f "$cale" ]; then
            if file "$cale" | grep -q "C++ source"; then
                nr_linii=$(wc -l "$cale" | cut -f1 -d' ')
                #mini_path=${cale#*$dir/}
                #minipath="$star/$mini_path"
                mini_path=$(realpath --relative-to="$dir" "$cale")

                mini_path="$star/$mini_path"
                echo "$nr_linii $mini_path"
                let nr_linii_txt+=nr_linii
            else
                mini_path=${cale#*$dir/}
                mini_path="$star/$mini_path"
                echo "$mini_path"
                echo "$(head -n 5 "$cale")"
                echo " "

            fi
        fi
    done
}

parc_director "$dir"
echo "Total lines in .cpp files: $nr_linii_txt"
