#!/bin/bash

factorial(){
	local num=$1
	local result=1
	for (( i=1; i <= num; i++)); do
		result=$((result * i))
	done
	echo $result
}

if [ $# -eq 0 ]; then 
	read -p "Valoare n:" n
else
	n=$1
fi

if ! [[ $n =~ ^[0-9]+$ ]] || [ $n -lt 0 ]; then
	echo "Invalid input"
	exit 1
fi

factor_n=$(factorial $n)

echo "Factorial de n este: $factor_n."
