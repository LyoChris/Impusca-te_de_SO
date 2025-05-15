#!/bin/bash

read -a numere

if [ ${#numere[@]} -eq 0 ]; then
	echo "Input e null"
	exit 1
fi

suma=0


for numar in "${numere[@]}"; do
	suma=$((suma + numar * numar * numar))
done

echo "Rezultatul este $suma."
exit 0
