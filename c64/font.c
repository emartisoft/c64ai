#include "font.h"

#define CHARSET ((unsigned char*)0x3000)

void setFont()
{
	__asm__("sei");
	// disable BASIC
	__asm__("lda $01");
	__asm__("and #$FE");
	__asm__("sta $01");
	__asm__("lda $dd00");
	__asm__("and #$fc");
	__asm__("ora #$03");
	__asm__("sta $dd00");
	// $3000 charset, $0400 screen
	__asm__("lda #$1c");
	__asm__("sta $d018");
	// screen hi byte
	__asm__("lda #$04");
	__asm__("sta $0288");
	// disable SHIFT + C= 
	__asm__("lda #$80");
	__asm__("sta $0291");
	__asm__("cli");
	// copy font data to $3000-$3800
	memcpy(CHARSET, (char*)&zxfont, 0x0800);
}
