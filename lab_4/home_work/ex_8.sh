#!/bin/bash

al_n_termen() {
	local x0=$1
	local x1=$2
	local m=$3
	
	if [ $m -gt $n ]; then
		echo $x2
		return
	fi

	local x2=$((4 * x0))
	x2=$((x2 + x1))
	x2=$((x2 + m))

	al_n_termen $x1 $x2 $((m + 1))
}

if [ $# -eq 0 ]; then
	read -p "Valoare n:" n
else
	n=$1
fi

if [ "$n" -lt 2 ]; then
	echo "Input bellow 2."
	exit 1
fi

rezultat=$(al_n_termen 1 2 2)

echo "Termenul al $n-lea are valoarea: $rezultat."
