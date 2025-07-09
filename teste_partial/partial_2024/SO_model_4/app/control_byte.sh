#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Immproper use of $0"
    exit 2
fi

real_dir=$(dirname $(realpath $0))
proces="$real_dir/recurse.sh"

if [ -f $proces ]; then
    if ! [ -x $proces ]; then
        echo "NU pot executa $proces" 1>&2
        exit 1
    fi
else
    echo "Nu exista $proces" 1>&2
    exit 1
fi

ok="0"

$proces $1 |   
while read -rN1 char; do
    if [[ $char == "=" ]]; then
        ok="1"
    fi
    if [[ $ok == "1" ]]; then
        if [[ $char =~ [0-9] ]]; then
            if [[ $char == "9" ]]; then
                echo -n "0"
            else
                next_char=$((char + 1))
                echo -n "$next_char"
            fi
        elif [[ $char == $'\n' ]]; then
            echo
            ok="0"
        else
            echo -n $char
        fi
    else
        if [[ $char == $'\n' ]]; then
            echo
            ok="0"
        else
            echo -n $char
        fi
    fi
done | sort -t '=' -k2,2n

exit $?