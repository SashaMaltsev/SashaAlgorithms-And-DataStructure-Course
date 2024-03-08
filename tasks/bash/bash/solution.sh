#!/bin/sh

suffix=$1
file1=$2
size=$3  
dir=${4:-$(pwd)}

if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
    echo "Usage: task_bash.bash [--help] suffix file1 size [dir]"
    echo "Удлинение файлов с указанным суффиксом до заданной длины путем присоединения к ним необходимого числа копий некоторого файла"
    echo "Опции:"
    echo "--help, -h         Показать помощь"
    exit 0
fi

if [ -z "$suffix" ] || [ -z "$file1" ] || [ -z "$size" ]; then
    echo " USAGE, i.e. -h"
    exit 1
fi

size_file1=$(wc -c <$file1)
data=$(cat <"$file1")

if [ $size_file1 -eq 0 ]; then 
    echo "Размер копируемого файла не может быть 0"
    exit 1
fi

echo -e "\tфайл\t\tразмер до\t\tразмер после"

count=0

for file in $(find $dir -name "*.$suffix");
do 
    size_file2=$(wc -c <$file)
    count=$((count+1))

    if [ "$file1" != "$(basename $file)" ]; then 
        echo -e "$count\t$(basename $file)\t$(basename $size_file2)\c"

        while [ $size_file2 -lt $size ]; do
                echo "$data" >> $file
                size_file2=$((size_file2+size_file1)) 
        done 
        
        echo -e "\t\t\t$size_file2"
    fi
done 