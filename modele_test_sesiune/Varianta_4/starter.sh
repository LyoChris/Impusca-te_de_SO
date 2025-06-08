#!/bin/bash

if ! [ $# -eq 1 ]; then
    echo "Numar de argumente insuficent."
    exit 1
else 
    p=$1
fi

path_dir=$(dirname $(realpath $0))
path_worker2="$path_dir/main_app/procesators/worker2"
path_sup="$path_dir/main_app/supervisor"
path_arg="$path_dir/data/input.txt"
path_shm="/dev/shm/shm_sex"
path_fifo="$path_dir/fifosw"

if [ $p -eq 0 ]; then
    $path_worker2 &
    sleep 1
    $path_sup $path_arg
fi

if [ $p -eq 1 ]; then
    $path_sup $path_arg &
    sleep 2
    $path_worker2
fi

wait

cat $path_shm
echo 

rm $path_fifo
rm $path_shm

