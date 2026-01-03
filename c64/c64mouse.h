#ifndef __C64MOUSE__
#define __C64MOUSE__

#include <mouse.h>
#include <cc65.h>
#include <c64.h>
#include <string.h>
#include <conio.h>

#define MULTICOLORMOUSE	// Comment this line for monochrome mouse pointer

void __fastcall__ mouseDriver(unsigned char Error);
void __fastcall__ installMouse();
void __fastcall__ uninstallMouse();
#endif
