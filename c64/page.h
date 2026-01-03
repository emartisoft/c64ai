
#ifndef __PAGE__
#define __PAGE__

#include <stdint.h>
#include <conio.h>
#include <peekpoke.h>
#include <stdbool.h>
#include "defines.h"
#include "aimem.h"
#include "c64sid.h"

#define PAGECOUNT   30

extern uint16_t page[PAGECOUNT]; // AI cevap PAGECOUNT sayfa ihtimali üzerine
extern int pagecursor;
extern uint16_t receivedLineCount;
extern uint16_t totalFileCount;
extern bool playingsid;
extern unsigned char song;

extern int sid_mute();
extern int sid_unmute();
extern void sid_pause_inirq2(void);
extern void sid_play_inirq2(void);

void __fastcall__ m_cls(void);
void __fastcall__ resetPage(void);
void __fastcall__ showPageNumber(void);
void __fastcall__ showPageNumberDir(void);
void __fastcall__ nextPage(void);
void __fastcall__ prevPage(void);
void __fastcall__ nextDirPage(void);
void __fastcall__ prevDirPage(void);
void __fastcall__ clear_sid_list(void);
void __fastcall__ print_sid_list(uint16_t idx);
void __fastcall__ writeStatus(char msg[24]);
void __fastcall__ sidinfo(void);
bool __fastcall__ isSidInfo(void);

#endif //__PAGE__