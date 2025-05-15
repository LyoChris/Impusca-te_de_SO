#!/bin/bash

suma(){
	local vec=("$@")
	local sum=0
	for num in "${vec[@]}"; do
		sum=$((sum + num))
	done
	
	echo $sum
}

if [ $# -lt 1 ]; then
	echo "Empty Input"
	exit 1
fi

n=$1
shift

for arg in "$@"; do
	if ! [[ "$arg" =~ ^[0-9]+$ ]]; then
		echo "Invalid Input"
		exit 2
	fi
done

>output.txt

vec=()
count=0

for arg in "$@"; do
        vec+=($arg)  # Add the current argument to the vec array
        count=$((count + 1))  # Increment the counter

        # When we have N numbers in the array, calculate their sum
        if [ $count -eq $n ]; then
                sum=$(suma "${vec[@]}")  # Call the suma function with the current group of numbers
                echo $sum  # Output the sum to the screen
                echo $sum >> output.txt  # Write the sum to the output.txt file
                vec=()  # Reset the vec array for the next group
                count=0  # Reset the counter
        fi
done

if [ ${#vec[@]} -gt 0 ]; then
    sum=$(suma "${vec[@]}")
    echo $sum
    echo $sum >> output.txt
fi
