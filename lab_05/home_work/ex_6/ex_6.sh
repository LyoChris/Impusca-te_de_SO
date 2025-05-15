#!/bin/bash
echo -n "Write usernames:"
read -a usernames

luna_curenta=$(date | cut -f2 -d' ')
echo "$luna_curenta"

for user in "${usernames[@]}"; do
    logs_list=$(last -F "$user" | grep "$luna_curenta" | head -n 3)
    
    if [ -z "$logs_list" ];then
        echo "Utilizatorul $user nu s-a conectat niciodată!"
    else
        echo "Ultimele 3 conectări ale utilizatorului $user în luna curentă sunt:"
        echo "$logs_list"
    fi

done
