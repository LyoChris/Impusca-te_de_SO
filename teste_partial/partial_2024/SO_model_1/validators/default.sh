#!/bin/bash

if [ $# -ge 1 ]; then
    file=$1
else
    echo "Too few arguments"
    exit 2
fi

proces="$PWD/validators/validatorASCII"
proces1="$PWD/validators/validatorASCII.c"
if ! [ -f $proces ]; then
    gcc -Wall $proces1 -o $proces
    if ! [ $? -eq 0 ]; then
        echo "Nu am putut compila $proces1"
        exit 3
    fi
fi

$proces $file | tr ':' '>' 