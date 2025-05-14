#!/bin/bash

if [ -z "$1" ]; then
	read -p "Valoare n:" n
else
	n=$1
fi

if ! [[ "$n" =~ ^[0-9]+$ ]] || [ "$n" -lt 0 ]; then
	echo "Invalid input"
	exit 1
fi

fibonacci(){
	if [ "$1" -le 1 ]; then
		echo "$1"
	else
		prev1=$(($1 - 1))
		prev2=$(($1 - 2))
		fib1=$(fibonacci $prev1)
		fib2=$(fibonacci $prev2)
		echo $(($fib1 + $fib2))
	fi
}

resultat=$(fibonacci $n)

echo "Valoarea fibonacci de rang $n este: $resultat."
