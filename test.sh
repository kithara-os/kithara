#!/bin/bash

if [ -e /dev/kvm ];
then qemu-system-x86_64 -cdrom KitharaOS.iso -m 1G -accel kvm -M q35 -smp 64 2> /dev/null;
else qemu-system-x86_64 -cdrom KitharaOS.iso -m 1G -smp 64  2> /dev/null;
fi
