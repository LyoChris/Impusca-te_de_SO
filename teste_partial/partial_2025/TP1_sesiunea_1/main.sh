#!/bin/bash

read -p "Dati numarul de instante: " n

dir_name=$(dirname $(realpath $0))
proces="$dir_name/count_rwx"
binar="$dir_name/base.bin"
arg="-o"

if ! [ -f $proces ]; then
    gcc -Wall "$proces.c" -o $proces
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare $proces.c"
        exit 1
    fi
fi

x=1; #y=1;

> $binar

for i in $(seq 1 $n); do
    $proces $binar "PATH TO FOLDER" &
done

wait

$proces $arg $binar
