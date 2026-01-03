#ifndef __TEXTBOX__
#define __TEXTBOX__

#include <conio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <peekpoke.h>

#include "defines.h"

extern uint16_t buf_len;
extern uint16_t cursor_pos;
extern uint16_t window_start;
extern uint16_t vis_cursor;

extern volatile uint8_t *SPR7X;
extern volatile uint8_t *SPR7Y; 
extern volatile uint8_t *SPR_MSB_X;

#define PAPER       COLOR_GRAY3
#define BACKGROUND  COLOR_BLUE

#define VISIBLE_LEN     0x1f
#define BUFFER_SIZE     0x0200
#define BUFFER_ADDR     0x3800
#define CLONE_ADDR      0x0A00

extern unsigned char *textbox;
extern unsigned char *cloneTextbox;

void __fastcall__ setCurPos(void);
void __fastcall__ redrawLine();
void __fastcall__ adjustWindow();
bool __fastcall__ insertChar(unsigned char ch);
void __fastcall__ backspaceAtCursor();
void __fastcall__ clearTextbox();
void __fastcall__ moveLeft();
void __fastcall__ moveRight();
void __fastcall__ writeTextbox(void);
void __fastcall__ setCloneTextbox(void);
bool __fastcall__ sendLastPrompt(void);

#endif //__TEXTBOX__