@echo off
deno compile --allow-net --allow-read --allow-env --allow-write ask2ai.ts
copy /Y ask2ai.exe ..\server\
del /Q ask2ai.exe
