#!/bin/bash

src="$1"
shift

if [ -f "$src.asm" ]; then
    echo prekladam $src.asm
    nasm -felf $src.asm || exit 1
    echo sestavuji $src.o
    gcc -o $src $src.o || exit 1
#    ld -m elf_i386 $src.o -o $src || exit 1
    rm $src.o
    echo spoustim ./$src "$@"
    echo --------------------------------------------------------
    ./$src "$@"
    echo --------------------------------------------------------
else
    echo `basename $0`:nenalezen soubor $src.asm
fi
