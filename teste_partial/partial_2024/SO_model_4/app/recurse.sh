#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Improper Use"
    exit 2
fi

real_dir=$(dirname $(realpath $0))
subfolder="$real_dir/source"
proces="$subfolder/first_and_last_byte"
proces1="$real_dir/recurse.sh"

if [ -d $subfolder ]; then
    if ! [ -f $proces ]; then
        gcc -Wall "$proces.c" -o $proces
    fi
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare $proces.c" 1>&2
        exit 3
    fi
else
    echo "Nu exista $subfolder." 1>&2
    exit 3
fi

parc_dir(){
    local arg=$1
    if [ -f $arg ]; then
        $proces $arg
    elif [ -d $arg ]; then
        for fis in $arg/* $arg/.*; do
            $proces1 $fis
        done
    fi
}

parc_dir $1