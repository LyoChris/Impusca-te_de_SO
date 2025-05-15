#!/bin/bash

factorial() {
    local num=$1
    local result=1
    for ((i = 1; i <= num; i++)); do
        result=$((result * i))
    done
    echo $result
}

if [ $# -eq 0 ]; then
    read -p "Valoare n:" n
    read -p "Valoare k:" k
else
    n=$1
    k=$2
fi

if ! [[ "$n" =~ ^[0-9]+$ ]] || ! [[ "$k" =~ ^[0-9]+$ ]] || [ "$n" -lt 0 ] || [ "$k" -lt 0 ]; then
    echo "Invalid input"
    exit 1
fi

if [ "$k" -gt "$n" ]; then
	echo "Invalid input k>n"
	exit 1
fi

factor_n=$(factorial $n)
factor_n_k=$(factorial $((n - k)))

Aranjamente_n_k=$((factor_n / factor_n_k))

echo "Rezultatul A($n, $k) este: $Aranjamente_n_k"
