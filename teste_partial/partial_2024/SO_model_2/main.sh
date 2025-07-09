#!/bin/bash

if [ $# -eq 1 ]; then
    dir=$1
else
    echo "Not enougj argumente"
    exit 1
fi

dir_path=$(dirname $(realpath $0))
subfolder="$dir_path/scripts"
proces1="$subfolder/pipeline.sh"
proces2="$subfolder/listdir.sh"

if [ -d $subfolder ]; then
    if [ -f $proces1 ]; then
        if ! [ -x $proces1 ]; then
            chmod u+x $proces1
        fi
    else
        echo "Nu exista $proces1." 1>&2
        exit 2
    fi
    if [ -f $proces2 ]; then
        if ! [ -x $proces2 ]; then
            chmod u+x $proces2
        fi
    else
        echo "Nu exista $proces2." 1>&2
        exit 2
    fi
else
    echo "Nu exista $subfolder." 1>&2
    exit 2
fi

subfolder2="$dir_path/programs"
proces3="$subfolder2/build_scattered_number.c"
proces4="$subfolder2/build_scattered_number"

if [ -d $subfolder2 ]; then
    if [ -f $proces3 ]; then
        if ! [ -r $proces3 ]; then
            echo "Nu pot citi $proces3" 1>&2
            exit 3
        fi
    else
        echo "Nu exista $proces3." 1>&2
        exit 3
    fi
else
    echo "Nu exista $subfolder2." 1>&2
    exit 3
fi

if ! [ -f $proces4 ]; then
    gcc -Wall $proces3 -o $proces4
    if ! [ $? -eq 0 ]; then
        echo "Nu am putut compila $proces3" 1>&2
        exit 4
    fi
fi

$proces1 $dir

echo "Programul $proces1 s-a terminat cu codul $?."