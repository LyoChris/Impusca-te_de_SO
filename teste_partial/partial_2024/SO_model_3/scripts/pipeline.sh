#!/bin/bash

if [ $# -eq 0 ]; then
    echo "No arguments"
    exit 2
fi

real_dir=$(dirname $(realpath $0))
proces="$real_dir/listdir.sh"

if [ -f $proces ]; then
    if ! [ -x $proces ]; then
        echo "Nu pot executa $proces"
        exit 1
    fi
else
    echo "Nu exista $proces"
    exit 1
fi

$proces $1 | sed 's/+++/ ---> /g' | cut -f3,4,5 -d' '


echo "Total numbers of pages: $pages"