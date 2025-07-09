#!/bin/bash

if [ $# -eq 1 ]; then
    dir=$1
    if ! [ -d $dir ]; then
        echo "$dir nu este un director"
        exit 1
    fi
else
    echo "Not enough argumente"
    exit 1
fi

real_dir=$(dirname $(dirname $(realpath $0)))
proces="$real_dir/programs/build_scattered_number"

if [ -f $proces ]; then
    if ! [ -x $proces ]; then
        echo "Nu pot executa $proces."
        exit 2
    fi
else
    echo "Nu exita $proces"
    exit 2
fi

parc_dir(){
    local arg_dir=$1
    for fis in $arg_dir/* $arg_dir/.*; do
        if [ -d $fis ]; then
            parc_dir $fis
        fi
        if [ -f $fis ]; then
            if [[ $fis == *.txt ]]; then
                $proces $fis
                if [ $? -ne 0 ]; then
                    echo "$? $file"
                fi
            fi
        fi
    done
}

parc_dir $1

