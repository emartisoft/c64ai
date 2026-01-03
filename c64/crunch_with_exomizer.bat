@echo off
exomizer sfx basic -m 256 c64chat.prg -o c64chat_exo.prg
c1541.exe -silent on -attach .\c64ai.d64 -delete c64chat
c1541.exe -silent on -attach .\c64ai.d64 -write c64chat_exo.prg c64chat
