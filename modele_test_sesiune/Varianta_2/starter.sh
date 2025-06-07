#!/bin/bash

if [ $# -eq 1 ]; then
    p=$1
else
    echo "Numar incorec de argumente"
fi

dir_path=$(dirname $(realpath $0))
path_to_worker1="$dir_path/subordinates/worker1"
path_to_worker2="$dir_path/subordinates/worker2"
path_to_data="$dir_path/in_data.txt"
path_to_fifo="$dir_path/fifo1"

if [ "$p" -eq 1 ]; then
    $path_to_worker1 $path_to_data &
    sleep 2
    $path_to_worker2
fi

if [ "$p" -eq 2 ]; then
    $path_to_worker2 &
    sleep 3
    $path_to_worker1 $path_to_data
fi

wait

od -t d4 /dev/shm/shm_w1_w2

rm /dev/shm/shm_w1_w2
rm $path_to_fifo