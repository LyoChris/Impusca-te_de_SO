#!/bin/bash

if [ $# -gt 0 ]; then
	numere=("$@")
else
	echo "Nu avem numere"
	exit
fi

read -p "Dati p:" p

for numar in "${numere[@]}"; do
	if [ "$numar" -le "$p" ]; then
		echo -n "$numar "
	fi
done

echo
