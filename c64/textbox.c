#include "textbox.h"

unsigned char *textbox  = (unsigned char*)BUFFER_ADDR;
unsigned char *cloneTextbox = (unsigned char*)CLONE_ADDR;

uint16_t buf_len = 0;
uint16_t cursor_pos = 0;
uint16_t window_start = 0;
uint16_t vis_cursor;

volatile uint8_t *SPR7X = (volatile uint8_t*)0xD00E;
volatile uint8_t *SPR7Y = (volatile uint8_t*)0xD00F;
volatile uint8_t *SPR_MSB_X = (volatile uint8_t*)0xD010;

void __fastcall__ setCurPos(void)
{
    uint16_t xcoord;
    xcoord = (wherex()+1)*8+24;

    if (xcoord > 255) {
        *SPR_MSB_X |= (1 << 7); // 7. sprite için MSB'yi 1 yap
        *SPR7X = xcoord - 256;
    }
    else {
        *SPR_MSB_X &= ~(1 << 7); // 7. sprite için MSB'yi 0 yap
        *SPR7X = xcoord;
    }

    *SPR7Y = wherey()*8+50;
}

void __fastcall__ redrawLine() {
    uint16_t idx;
    unsigned char z, ch;
    unsigned char row = 23;
    gotoxy(2, row);
    for (z = 0; z < VISIBLE_LEN; ++z) {
        idx = window_start + z;
        ch = ' ';
        if (idx < buf_len) ch = textbox[idx];
        textcolor(COLOR_BLACK);
        cputc(ch);
    
    }

    vis_cursor = cursor_pos - window_start;
    if (cursor_pos < window_start) vis_cursor = 0;
    if (cursor_pos > window_start + VISIBLE_LEN) vis_cursor = VISIBLE_LEN - 1;
    gotoxy(1 + vis_cursor, row);
}

void __fastcall__ adjustWindow() {
    if (cursor_pos < window_start)
        window_start = cursor_pos;
    else if (cursor_pos >= window_start + VISIBLE_LEN)
        window_start = cursor_pos - VISIBLE_LEN + 1;
    if (window_start + VISIBLE_LEN > BUFFER_SIZE)
        window_start = BUFFER_SIZE - VISIBLE_LEN;
}

bool __fastcall__ insertChar(unsigned char ch) {
    if (buf_len >= BUFFER_SIZE) {
        return false;
    }
	
	if (cursor_pos < buf_len)
		memmove(&textbox[cursor_pos + 1], &textbox[cursor_pos], buf_len - cursor_pos);
	textbox[cursor_pos] = ch;
	buf_len++;
	cursor_pos++;
    return true;
}

void __fastcall__ backspaceAtCursor() {
    if (cursor_pos == 0) { textcolor(BACKGROUND); cputc(0xb9); textcolor(COLOR_BLACK); return; }
    memmove(&textbox[cursor_pos - 1], &textbox[cursor_pos], buf_len - cursor_pos);
    textbox[--buf_len] = 0;
    cursor_pos--;
}


void __fastcall__ clearTextbox(){
    uint16_t q;
    // clear textbox
    textcolor(COLOR_BLACK);
    cclearxy(2, 23, PAPERWIDTH-2);
    gotoxy(1, 23);
    
    for (q = 0; q < BUFFER_SIZE; ++q) textbox[q] = 0x20;
    window_start = 0;
    buf_len = 0;
    cursor_pos = 0;
}

void __fastcall__ moveLeft() { 
    if (cursor_pos > 0) cursor_pos--; else {textcolor(BACKGROUND); cputc(0xb9); textcolor(COLOR_BLACK);} 

}
void __fastcall__ moveRight() {
    if (cursor_pos < buf_len) cursor_pos++; else {textcolor(BACKGROUND); cputc(0xb9); textcolor(COLOR_BLACK);}  
}

void __fastcall__ writeTextbox(void) {

    unsigned char h, w;
    uint16_t n;

    for ( h = 0; h < 16; h++)
    {
        for ( w = 0; w < PAPERWIDTH; w++)
        {
            n = w+h*PAPERWIDTH;
            if ((n > (BUFFER_SIZE-1)) || (textbox[n] == 0x00)) return;
            POKE(0x42A+w+h*40, textbox[n] & 0x3f);
        }
    }
}

// Textbox içeriği yedekleme
void __fastcall__ setCloneTextbox(void)
{
    uint16_t q;
    for (q = 0; q < BUFFER_SIZE; ++q) cloneTextbox[q] = textbox[q];
    POKEW(0x09fe, buf_len);
}

bool __fastcall__ sendLastPrompt(void)
{
    uint16_t q;
    bool isAvailable = false;  
    for (q = 0; q < BUFFER_SIZE; ++q)
    {
        textbox[q] = cloneTextbox[q];
        if(cloneTextbox[q] != 0x20)
            isAvailable = true;
    }
    buf_len = PEEKW(0x09fe);
    return isAvailable;
}