#!/bin/bash

read -p "Dati numarul de instante: " n1

director=$(dirname $(realpath $0))
proces="$director/sum"
binar="$director/data.bin"
fisier="$director/in.txt"
optiune="--words"

> $binar

if ! [ -f $proces ]; then
    gcc -Wall "$proces.c" -o $proces
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare"
        exit 1
    fi
fi

for i in $(seq 1, $n1); do
    $proces $binar $fisier &
done

wait

$proces $optiune $binar


