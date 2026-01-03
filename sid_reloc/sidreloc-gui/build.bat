@echo off
g++ sidreloc-gui.cpp -o sidreloc-gui.exe -I"C:\FLTK64\include" -L"C:\FLTK64\lib" -lfltk -lgdiplus -lwinspool -lcomdlg32 -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32 -static -mwindows
upx --best --lzma sidreloc-gui.exe
pause
