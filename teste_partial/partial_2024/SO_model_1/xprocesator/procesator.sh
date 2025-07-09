#!/bin/bash

if [ $# -ge 1 ]; then
    dir=$1
else
    echo "Too few arguments"
    exit 2
fi

proces="$PWD/validators/default.sh"
proces2="$PWD/xprocesator/procesator.sh"

if [ -f $proces ]; then
    if ! [ -x $proces ]; then
        echo "Nu pot sa execut fisierul default.sh" >&2
        exit 1
    fi
else
    echo "$proces nu exista" >&2
    exit 1
fi

if [ -d $dir ]; then
   if ! [[ -r $dir && -x $dir ]]; then
        echo "Nu pot traversa directorul dat"
        exit 3
    fi
else
    echo "Directorul dat nu exista"
    exit 3
fi

#parc_director(){
#    local arg_dir=$1
#    for fis in $arg_dir/* $arg_dir/.*; do
#        if [ -d "$fis" ]; then
#            parc_director "$fis"
#        elif [ -f "$fis" ]; then
#            
#            "$proces" "$fis" >> ./results.txt 2>&1
#        fi
#    done
#
#}

for fis in $dir/* $dir/.*; do
    if [ -d "$fis" ]; then
        "$proces2" $fis
    elif [ -f "$fis" ]; then           
        "$proces" "$fis" >> ./results.txt 2>&1
    fi
done

#parc_director $dir