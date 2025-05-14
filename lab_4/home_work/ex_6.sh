#!/bin/bash

is_prime() {
    	local num=$1
	if [ "$num" -le 1 ]; then
        	return 1
    	fi
    	for ((i=2; i*i<=num; i++)); do
        	if ((num % i == 0)); then
            		return 1
        	fi
    	done
    	return 0
}


sum_prime_digits() {
    local num=$1
    local sum=0

    
    if [ "$num" -eq 0 ]; then
        echo $sum
        return
    fi

    
    local digit=$((num % 10))
   	
    if is_prime "$digit"; then
        sum=$((sum + digit))
    fi

    next_sum=$(sum_prime_digits $((num / 10)))
    echo $((sum + next_sum))
}


if [ $# -eq 0 ]; then
    echo "Valaore n:"
    read n
else
    n=$1
fi

if [ "$n" -le 0 ]; then
    echo "Negative input not allowed."
    exit 1
fi

# Calculăm suma cifrelor prime și o afișăm
result=$(sum_prime_digits "$n")
echo "The sum is: $result."
