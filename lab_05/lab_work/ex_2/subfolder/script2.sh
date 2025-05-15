#!/bin/bash

if [ $# -eq 1 ]; then
    dir=$1
else
    echo "No argument recieved"
    exit 1
fi

nr_bash_total=0

prac_director(){
    arg_dir=$1
    #for fis in $(ls -1A | tr " " / ); do
    for fis in $arg_dir/* $arg_dir/.*; do
        #cale=$(echo $fis | tr / " " )
        cale=$fis
        if [ -d "$cale" ]; then
            prac_director "$cale"
        else
            if [ -f "$cale" ]; then
                if file "$cale" | grep -q "Bourne-Again shell script"; then
                    let nr_bash_total++
                    sum=$(sha256sum "$cale" | cut -f1 -d' ')
                    echo "$cale    $sum"
                else
                    nr_linii=$(wc -l "$cale" | cut -f1 -d' ')
                    echo "$cale    $nr_linii" 1>&2  
                fi
            fi
        fi
    done
}

prac_director "$dir"

if [ $nr_bash_total -le 0 ]; then
    echo "Nu avem scripturi bash."
    exit -1
else
    echo "Nr. de scripturi bash: $nr_bash_total."
    exit 0
fi
