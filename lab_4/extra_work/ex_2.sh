#!/bin/bash

if [ $# -lt 1 ]; then
    read -p "Dati n:" n
else
    n=$1
fi

prod=1

for k in $(seq 1 1 $n)
do
    prod=$((prod * k))
done

echo "Fact($n)=$prod"
