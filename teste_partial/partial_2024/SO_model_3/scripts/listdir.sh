#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Improper use"
    exit 2
fi

real_dir=$(dirname $(dirname $(realpath $0)))
subfolder="$real_dir/programs"
proces="$subfolder/file_pages"
proces1="$real_dir/scripts/listdir.s~h"

if [ -d $subfolder ]; then
    if ! [ -f $proces ]; then
        gcc -Wall "$proces.c" -o $proces
        echo "Am compilat"
    fi
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare $proces.c"
        exit 3
    fi
else
    echo "Nu exista $subfolder."
    exit 3
fi

if [ -d $1 ]; then
    for fis in $1/* $1/.*; do
        $proces1 $fis
    done
elif [ -f $1 ]; then
    $proces $1
fi