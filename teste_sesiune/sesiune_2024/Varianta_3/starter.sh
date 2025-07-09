#!/bin/bash

if [ $# -eq 1 ]; then
    p=$1
else
    echo "Numar incorec de argumente"
fi

path_dir=$(dirname $(realpath $0))
path_anal="$path_dir/app/analyzers/analyzer"
path_serv="$path_dir/app/services/service"
path_arg="$path_dir/data/input.txt"
path_fifo="$path_dir/fifosw"
path_shm="/dev/shm/anal_serv"

if [ $p -eq 1 ]; then
    $path_serv &
    sleep 2
    $path_anal $path_arg
fi

if [ $p -eq 0 ]; then
    $path_anal $path_arg &
    sleep 1
    $path_serv
fi

wait

od -c $path_shm

rm $path_fifo
rm $path_shm