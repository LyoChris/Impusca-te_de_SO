#!/bin/bash

#images=$(find find "/home/lyo/Pictures" -type f 2>/dev/null | grep -i)

#files=$(find / -type f 2>dev\null);
#file=$(cat $file | tr ' ' '\\')

#for fis in "$file"; do
#    fis=$(cat $fis | tr '\\' ' ')
#    if file $fis | grep -i "image"; then
#            fis=$(cat $fis | tr ' ' '\\')
#            images=$images$(echo $file)
#    fi
#done

#echo $images

#find "/home/lyo/Pictures" -type f 2>/dev/null | while read -r fis; do
#    if file $fis | grep -q -i "image"; then
#            images="$images    $fis"
            #images="$images$(printf '\n%s' "$fis")"
#    fi
#done

#echo -e "$images"

echo "<!DOCTYPE html>" > "galerie.html"
echo "<html lang='ro'>" >> "galerie.html"
echo "<head>" >> "galerie.html"
echo "    <meta charset='UTF-8'>" >> "galerie.html"
echo "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>" >> "galerie.html"
echo "    <title>Imagini din contul meu</title>" >> "galerie.html"
echo "</head>" >> "galerie.html"
echo "<body>" >> "galerie.html"
echo "    <h1>Imagini din contul meu</h1>" >> "galerie.html"
echo "    <ul>" >> "galerie.html"


mapfile -t images < <(find ~ -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" -o -iname "*.gif" -o -iname "*.bmp" -o -iname "*.tiff" -o -iname "*.webp" \) -exec stat --format="%Y %n" {} \; | sort -k1,1n | cut -f2 -d' ')

for fis in "${images[@]}"; do
    echo $fis
    echo "        <li><a href='$fis'>$fis</a></li>" >> "galerie.html"
done

echo "    </ul>" >> "galerie.html"
echo "</body>" >> "galerie.html"
echo "</html>" >> "galerie.html"