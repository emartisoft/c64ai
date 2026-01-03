#!/usr/bin/env bash
set -e

# ----- Build C and ASM sources -----

cc65 -O -t c64 c64chat.c
ca65 c64chat.s

ca65 charset.asm
ca65 sidfunc.asm

cc65 -O -t c64 c64dir.c -I.
ca65 c64dir.s

ca65 irq.asm

cc65 -O -t c64 c64mouse.c -I.
ca65 c64mouse.s

cc65 -O -t c64 c64sid.c -I.
ca65 c64sid.s

ca65 menu.asm
ca65 setupscreen.asm

cc65 -O -t c64 page.c -I.
ca65 page.s

cc65 -O -t c64 aimem.c -I.
ca65 aimem.s

cc65 -O -t c64 textbox.c -I.
ca65 textbox.s

cc65 -O -t c64 font.c -I.
ca65 font.s

# ----- Link -----

ld65 \
  c64chat.o charset.o menu.o irq.o c64mouse.o sidfunc.o \
  c64dir.o c64sid.o setupscreen.o page.o aimem.o textbox.o font.o \
  c64.lib -C c64-ai.cfg -o c64chat.prg

# ----- Write PRG to disk image -----
cp ./disk/blank.d64 c64ai.d64
c1541 -silent -attach ./c64ai.d64 -delete c64chat.prg || true
c1541 -silent -attach ./c64ai.d64 -write c64chat.prg
c1541 -silent -attach ./c64ai.d64 -delete help || true
c1541 -silent -attach ./c64ai.d64 -write help

echo "Build complete."
