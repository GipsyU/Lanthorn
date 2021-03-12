#!/bin/bash
function write()
{
    name=${1#*root}
    echo -n $name >> $obj/fileimg
    for ((i = ${#name}+1; i<= 32; ++i))
    do
        echo -n -e "\x0" >> $obj/fileimg
    done
    size=`wc -c < $1`
    size=`printf "%08x" $size`
    size="\x"${size:6:2}"\x"${size:4:2}"\x"${size:2:2}"\x"${size:0:2}
    echo -n -e $size >> $obj/fileimg
    cat $1 >> $obj/fileimg
}

function find(){
    for file in `ls $1`
    do
        if [ -d $1"/"$file ]
        then
            find $1"/"$file
        else
            write $1"/"$file
        fi
    done
}
echo -n "" > $obj/fileimg
find $srctree/$obj/root
ld -m elf_i386 -r -b binary $obj/fileimg -o $obj/fs.o
