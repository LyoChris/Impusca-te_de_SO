#!/bin/bash

al_n_termen() {
	local y0=$1
	local y1=$2
	local m=$3
	
	if [ $m -gt $n ]; then
		echo $y2
		return
	fi

	local y2=$((((m + 1)) * y1))
	y2=$((y2 +((((m + 2)) * y0))))

	al_n_termen $y1 $y2 $((m + 1))
}

if [ $# -eq 0 ]; then
	read -p "Valoare n:" n
else
	n=$1
fi


if [ "$n" -lt 2 ]; then
	echo "Input bellow 2."
	exit
fi


rezultat=$(al_n_termen 1 1 2)

echo "Termenul al $n-lea are valoarea: $rezultat."
