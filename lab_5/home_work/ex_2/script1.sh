#!/bin/bash

if [ $# -eq 1 ]; then
    start_dir=$1
    found_dir=$(find / -type d -name "$start_dir" 2>/dev/null) # pot aduga 2>/dev/null pentru output mai clear

    if [ -z "$found_dir" ]; then
        echo "Directorul nu exista!"
        exit 1
    fi

    #if [ -d "$found_dir" ]; then
        if ! [ -r "$found_dir" ] || ! [ -x "$found_dir" ]; then
            echo "Nu pot accesa/citi directorul!"
            exit 2
        fi
    #fi

else
    echo "Nu am director in argumentul liniei de comanda!"
    exit 3
fi

subfolder="$PWD/subdir_sh"
if ! [ -d "$subfolder" ]; then
    echo "Subfolderul nu exista!"
    exit 1
fi

script2="$subfolder/script2.sh"


"$script2" "$found_dir" "$start_dir"

exit_code_1=$?
echo "Script2 finished with code: $exit_code_1."
