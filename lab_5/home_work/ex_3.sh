    #!/bin/bash

    if [ $# -lt 1 ];then
            read -p "Valoarea adancimii:" depth
    else
        depth=$1
    fi

    file_list=$(find ~/Desktop/Facultate -maxdepth "$depth" -type f)

    if [ $? -ne 0 ];then
        echo "The find command has failed!" >&2
        exit 2
    fi

    for fis in $file_list; do
        cale=$fis
        if file "$cale" | grep -q "Bourne-Again shell script"; then
            chmod +x "$cale"
            perm=$(ls -l "$cale" | cut -f1 -d' ')
            echo "$cale $perm"
        else
            perm_norm=$(stat -c "%a" "$cale")
            echo "$perm_norm $cale"
        fi
    done

