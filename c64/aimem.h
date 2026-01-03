#ifndef __AIMEM__
#define __AIMEM__

#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include "defines.h"
#include "page.h"

#define AI_TEXT_ADDR    0xB000
#define AI_TEXT_END     0xD000

extern unsigned char *aitext;
extern unsigned char *aiend;
extern unsigned char *aicursor;
extern uint16_t aiCursorY;

void __fastcall__ write_ai_char(unsigned char c);
void __fastcall__ clear_aitext();
void __fastcall__ writeAIText(uint16_t idx);
void write_memory(const char* fmt, ...);

#endif //__AIMEM__