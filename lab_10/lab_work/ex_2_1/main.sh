#!/bin/bash

read -p "Dati numarul de instante: " n

director=$(dirname $(realpath $0))
proces="$director/MyCS_shmem"
optiune="-o"
optiune2="-i"

n1=10
n2=-50
n3=-30

if ! [ -f $proces ]; then
    gcc -Wall "$proces.c" -o $proces
    if [ $? -ne 0 ]; then
        echo "Eroare la compilare"
        exit 1
    fi
fi

$proces $optiune2

for i in $(seq 1, $n); do
    $proces $n1 $n2 $n3 &
done

wait

$proces $optiune
