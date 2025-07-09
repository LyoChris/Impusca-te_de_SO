#!/bin/bash

if [ $# -eq 1 ]; then
    dir=$1
else
    echo "Not enougj argumente"
    exit 1
fi

dir_path=$(dirname $(realpath $0))
proces2="$dir_path/listdir.sh"

$proces2 $1 | sort -t ':' -k2,2rn | head -n 5 | cut -f1 -d':'