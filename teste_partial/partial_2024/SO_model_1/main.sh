#!/bin/bash

dir=$1

subfolder1="$PWD/xprocesator"
if [ -d $subfolder1 ]; then
    proces1="$subfolder1/procesator.sh"
    if [ -f $proces1 ]; then
        if ! [ -x $proces1 ]; then
            echo "nu pot executa fisierul procesator.sh" >&2
            exit 1
        fi
    else
        echo "fisierul procesator.sh nu exista" >&2
        exit 1
    fi
else
    echo "$subfolder1 nu exista" >&2
    exit 1
fi

subfolder2="$PWD/validators"
if [ -d $subfolder2 ]; then
    proces2="$subfolder2/validatorASCII.c"
    if [ -f $proces2 ]; then
        if ! [ -r $proces2 ]; then
            echo "nu pot citi fisierul procesator.sh" >&2
            exit 2
        fi
    else
        echo "fisierul validatorASCII.c nu exista" >&2
        exit 2
    fi
else
    echo "$subfolder2 nu exista" >&2
    exit 2
fi

if [ -d $dir ]; then
   if ! [ -r $dir ]; then
        echo "Nu pot citi directorul dat"
        exit 3
    fi
else
    echo "Directorul dat nu exista"
    exit 3
fi

"$proces1" $dir

echo "$proces1 s-a terminat cu codul $?"
