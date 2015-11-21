#!/bin/sh
if [ "$1x" == x ]; then echo "Use Eclipse Manage Configurations: AutoMake!"; exit 1; fi
echo ------------------------------------------------------------------------------
python  ../esptool.py elf2image -o ../bin/ -ff 80m -fm qio -fs 4m $1
echo ------------------------------------------------------------------------------
echo Add rapid_loader...
mv -f ../bin/0x00000.bin ../bin/0.bin
dd if=../bin/rapid_loader.bin > ../bin/0x00000.bin
dd if=../bin/0.bin >> ../bin/0x00000.bin
