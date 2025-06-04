#!/bin/bash

if [ $# -eq 1 ]; then
    p=$1
else
    echo "Nu am primit argument"
    exit 1
fi

dir_path=$(dirname $(realpath $0))

path_to_worker2="$dir_path/main_app/procesators/worker2"
path_to_supervisor="$dir_path/main_app/supervisor"

path_to_data="$dir_path/in_data.txt"

if [ "$p" -eq 0 ]; then
    $path_to_worker2 &
    sleep 1
    $path_to_supervisor $path_to_data
fi

if [ "$p" -eq 1 ]; then
    $path_to_supervisor $path_to_data &
    sleep 2
    $path_to_worker2
fi

wait

#cat /dev/shm/shm_worker1
#hexdump -c /dev/shm/shm_worker1
od -t d4 /dev/shm/shm_worker1
echo

rm /dev/shm/shm_worker1
rm "$dir_path/fifo1"