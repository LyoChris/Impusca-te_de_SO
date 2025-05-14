#!/bin/bash

if [ $# -gt 0 ]; then
	secventa=("$@")
else
	echo "Introduceti numere:"
	read -a secventa
fi

secventa1001=""
for num in "${secventa[@]}"; do
	if [ $num -gt 1000 ]; then
		secventa1001="$secventa1001 $num"
	fi
done

export  secventa1001="$secventa1001"

./consumer.sh

echo "Producer: consumer's exit code is $?" 
