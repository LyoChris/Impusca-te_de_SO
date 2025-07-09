#!/bin/bash

real_dir=$(dirname $(realpath $0))

subfolder="$real_dir/scripts"
proces1="$subfolder/pipeline.sh"

subfolder2="$real_dir/programs"
proces2="$subfolder2/file_pages.c"

if [ -d $subfolder ]; then
    if [ -f $proces1 ]; then
        if ! [ -x $proces1 ]; then
            echo "Nu pot executa $proces1."
            exit 1
        fi
    else
        echo "Nu exista $proces1."
        exit 1
    fi
else
    echo "Nu exista $subfolder."
    exit 1
fi

if [ -d $subfolder2 ]; then
    if [ -f $proces2 ]; then
        if ! [ -r $proces2 ]; then
            echo "Nu pot citi $proces2."
            exit 2
        fi
    else
        echo "Nu exista $proces2."
        exit 2
    fi
else
    echo "Nu exista $subfolder2."
    exit 2
fi

if [ -d $1 ]; then
    if ! [ -r $1 ]; then
        echo "Nu pot citi $1."
        exit 3
    fi
else
    echo "$1 nu este un director"
    exit 3
fi

$proces1 $1

echo "Programul $proces1 s-a terminat cu codul $?."