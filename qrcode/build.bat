@echo off
cc65 -O -t c64 .\c64aisetup.c
ca65 .\c64aisetup.s
ld65 -t c64 .\c64aisetup.o c64.lib -o .\c64aisetup.prg

