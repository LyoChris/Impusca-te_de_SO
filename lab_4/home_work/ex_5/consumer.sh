#!/bin/bash

secventa=$secventa1001

if [ -z "$secventa" ]; then
	echo "Consumer: computed sum is NULL."
	exit 1
fi

sum=0
for num in $secventa; do
	sum=$((sum + num))
done

echo "Consumer: computed sum is $sum."

exit 0
