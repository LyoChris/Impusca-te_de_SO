#!/bin/bash

logs_record(){
    local user=$1
    local luna_curenta=$(date | cut -f2 -d' ')
    logs_list=$(last -F "$user" | grep "$luna_curenta" | head -n 3)
    if [ -z "$logs_list" ];then
        echo "Utilizatorul $user nu s-a conectat niciodată!"
    else
        echo "Ultimele 3 conectări ale utilizatorului $user în luna curentă sunt:"
        echo "$logs_list"
    fi
}

while true; do
    read -p "Introduceti numele unui utilizator:" user_name

    if [ -z "$user_name" ];then
        echo "Finalizare script."
        break
    fi

    logs_record "$user_name"
done