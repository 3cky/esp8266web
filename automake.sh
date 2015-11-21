#!/bin/sh
if [ "$1x" == x ]; then echo "Use Eclipse Manage Configurations: AutoMake!"; exit 1; fi
echo ------------------------------------------------------------------------------
python  ../esptool.py elf2image -o ../bin -ff 80m -fm qio -fs 4m $1
echo ------------------------------------------------------------------------------
