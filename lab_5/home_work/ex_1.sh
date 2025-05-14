#!/bin/bash

if [ $# -eq 0 ]; then
    read -p "Numele directorului de start:" start_dir
    if [ -z "$start_dir" ]; then
         start_dir="."
    fi
else
    start_dir=$1
fi

nr_txt=0



parc_director(){
    arg_dir=$1
    for fis in "$arg_dir"/* "$arg_dir"/.*; do
        cale=$fis
        #echo "$cale"
        if [ -d "$cale" ]; then
            parc_director "$cale"
        fi
        if [ -f "$cale" ]; then
            if file "$cale" | grep -q "ASCII text"; then
                let nr_txt++
                info=$(wc -L -w "$cale" | tr -s ' ' | cut -f1,2 -d' ')
                echo "$cale $info"
            fi
        fi
    done
}

real_start_dir=$(locate SO | grep -E "/SO/?$")
echo "$real_start_dir"

parc_director "$real_start_dir"

if [ $nr_txt -eq 0 ]; then
    echo "Nu am gasit fisiere txt."
    exit 0
fi