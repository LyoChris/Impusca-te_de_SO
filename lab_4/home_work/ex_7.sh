#!/bin/bash

nr_complement() {
	local num=$1
	local rezultat=""
	
	if [ $num -le 0 ]; then
		return
	fi

	local digit=$((num % 10))
	local complement=$((9 - digit))

	rezultat="$complement$(nr_complement $((num / 10)))"

	echo -n $rezultat
}

if [ $# -eq 0 ]; then
	read -p "Valoare n:" n
else
	n=$1
fi

rezultat=$(nr_complement $n)

rezultat=$(echo $rezultat | sed 's/^0*//')

echo "Complementul lui $n este $rezultat."


