#!/bin/bash

export CC="x86_64-elf-gcc"
export AS="nasm"
export LD="x86_64-elf-gcc"

mkdir obj

function cleanup {
    rm -rf obj
    rm -rf apboot.bin
}

trap cleanup EXIT

echo "[AS] aptrampoline.s -> aptrampoline.bin"
nasm -f bin aptrampoline.s -o aptrampoline.bin || exit

cpaths="eval find src -type f -name '*.c'"
for csource in $($cpaths)
do
    objectPath="obj/c_$(basename "$csource" .c).o"
    echo "[CC] $csource -> $objectPath"

    $CC -c -o $objectPath $csource -Iinclude \
    -ffreestanding -Wall -Wextra -O2 -mno-sse -mno-sse2 -mno-sse3\
    -mcmodel=large -mno-red-zone -mno-red-zone -Wno-attributes || exit
done

asmpaths="eval find src -type f -name '*.s'"
for asmsource in $($asmpaths)
do
    objectPath="obj/asm_$(basename "$asmsource" .c).o"
    echo "[AS] $asmsource -> $objectPath"
    $AS -f elf64 $asmsource -o $objectPath -w-number-overflow || exit
done

echo "[LD] obj/* -> kernel.bin"
$LD -n -T ld/linker.ld obj/* -o kernel.bin -ffreestanding -O2 -lgcc -nostdlib