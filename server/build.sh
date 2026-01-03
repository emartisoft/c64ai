#!/usr/bin/env bash
gcc c64aiserver.c -o c64aiserver -O3 -march=native -flto -s -Wall -Wextra
