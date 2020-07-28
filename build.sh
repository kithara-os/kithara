#!/bin/bash

function cleanup {
	rm -rf isotree
}

trap cleanup EXIT

echo "Building kernel..."
cd kernel
./build.sh
cd ..

echo "Copying files..."
mkdir isotree
mkdir isotree/boot
mkdir isotree/boot/grub
cp grub/grub.cfg isotree/boot/grub/grub.cfg
cp kernel/kernel.bin isotree/boot/kernel.bin

echo "Creating image..."
grub-mkrescue isotree -o KitharaOS.iso 2> /dev/null || exit

