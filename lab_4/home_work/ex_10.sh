#!/bin/bash

euclid() {
	local n=$1
	local m=$2
	
	if [ $n -eq $m ]; then
		echo $n
		return
	fi
	
	if [ $n -gt $m ]; then
		n=$((n - m))
	else
		m=$((m - n))
	fi

	euclid $n $m
}

if [ $# -lt 2 ]; then
	read -p "Valoare n:" n
	read -p "Valoare m:" m
else
	n=$1
	m=$2
fi

rezultat=$(euclid $n $m)

echo "CMMDC intre $n si $m este: $rezultat."
