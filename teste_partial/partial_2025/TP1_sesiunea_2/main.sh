#!/bin/bash

read -p "Dati numarul de instante: " n

dir_name=$(dirname $(realpath $0))
proces="$dir_name/count_rwx"
binar="$dir_name/base.bin"
arg="-p"

if ! [ -f $proces ]; then
    gcc -Wall "$proces.c" -o $proces
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare $proces.c"
        exit 1
    fi
fi

director=

> $binar

for i in $(seq 1 $n); do
    $proces $binar $director &
done

wait

$proces $arg $binar