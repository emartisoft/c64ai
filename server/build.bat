@echo off
gcc c64aiserver.c -o c64aiserver.exe -O3 -march=native -flto -s -Wall -Wextra
