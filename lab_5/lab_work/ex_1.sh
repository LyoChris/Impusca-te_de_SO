#!/bin/bash

if [ $# -eq 0 ]; then
    read -p "Da adancime maxima:" n
else
    n=$1
fi

home_dir="$HOME"

files=$(find "$home_dir" -maxdepth "$n" -type f -mtime -28)

if [ $? -ne 0 ]; then
    echo "Command find has failed!" 1>&2
    exit 1
fi

for file in $files; do
    if file "$file" | grep -q "C source"; then
        gcc "$file" -o "${file%.*}"

        if [ $? -eq 0 ]; then
            echo "$file -> Compilat ok!"
        else
            echo "$file -> Eroare la compilare!"
        fi
    else
        last_date=$(stat -c %y "$file")
        echo "$last_date $file"
    fi    

done
