### Requirements
* cmake (https://cmake.org/download/)
* gcc (https://strawberryperl.com/)
  We use the **GCC compiler** that comes bundled with **Strawberry Perl**.
* git (optional)
* upx (optional) (https://github.com/upx/upx)

### Download FLTK

FLTK: https://github.com/fltk/fltk
or
```bash
git clone https://github.com/fltk/fltk.git
```

#### Build with cmake
```bash
cd fltk
mkdir build
cd build
```
for 64bit:
```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:\FLTK64 -DFLTK_BUILD_SHARED=OFF
cmake --build .
cmake --install .
```
to compile on Windows:
```bash
g++ sidreloc-gui.cpp -o sidreloc-gui.exe -I"C:\FLTK64\include" -L"C:\FLTK64\lib" -lfltk -lgdiplus -lwinspool -lcomdlg32 -lole32 -luuid -lcomctl32 -lgdi32 -lws2_32 -static -mwindows
```
optional:
```bash
upx --best --lzma sidreloc-gui.exe
```
