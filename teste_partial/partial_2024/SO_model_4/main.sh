#!/bin/bash

real_dir=$(dirname $(realpath $0))

subdirector="$real_dir/app"
proces1="$subdirector/control_byte.sh"

subdirector2="$subdirector/source"
proces2="$subdirector2/first_and_last_byte.c"

if [ -d $subdirector ]; then
    if [ -f $proces1 ]; then
        if ! [ -x $proces1 ]; then
            echo "Nu pot executa $proces1"
            exit 1
        fi
    else
        echo "Nu exista $proces1"
        exit 1
    fi
else
    echo "Nu exista $subdirector"
    exit 1
fi

if [ -d $subdirector2 ]; then
    if [ -f $proces2 ]; then
        if ! [ -r $proces2 ]; then
            echo "Nu pot citi $proces2"
            exit 2
        fi
    else
        echo "Nu exista $proces2"
        exit 2
    fi
else
    echo "Nu exista $subdirector2"
    exit 2
fi

if ! [ -d $1 ] && ! [ -f $1 ]; then
    echo "$1 nu este nici director, nici fisier normal"
    exit 3
fi

$proces1 $1

echo "Programul $proces1 a terminat cu codul $?."