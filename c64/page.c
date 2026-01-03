#include "page.h"

uint16_t page[PAGECOUNT];
int pagecursor;

uint16_t receivedLineCount = 0;
uint16_t totalFileCount;
bool playingsid;
unsigned char song;

void __fastcall__ resetPage(void)
{
    unsigned char u;
    for (u = 1; u < PAGECOUNT; u++)
    {
        page[u] = 0xffff;
    }
    page[0] = 0;
    pagecursor = 0;
}

void __fastcall__ showPageNumber(void)
{
    gotoxy(2, 21);
    textcolor(COLOR_BLUE);
    cprintf("\xb4%d/%d\xb5     ", pagecursor+1, (receivedLineCount/20)+1);
}

void __fastcall__ showPageNumberDir(void)
{
    gotoxy(2, 21);
    textcolor(COLOR_BLUE);
    cprintf("\xb2%d/%d\xb3     ", pagecursor+1, (totalFileCount/17)+1);
}

void __fastcall__ clear_sid_list(void)
{
    unsigned char y;
    for (y=0; y<17; y++)
    {
        cclearxy(2, y+3, 22);
    }
    gotoxy(1, 23);
}

void __fastcall__ nextPage(void)
{
    pagecursor++;
    if(pagecursor>=(receivedLineCount/20)+1) 
        pagecursor = (receivedLineCount/20);
    else
        page[pagecursor] = aiCursorY;

    if(page[pagecursor] < (AI_TEXT_END-AI_TEXT_ADDR))
        writeAIText(page[pagecursor]); 
    else
    {
        pagecursor--;
        if(pagecursor<0) pagecursor = 0;
        receivedLineCount = (pagecursor+1)*20-1;
        writeAIText(page[pagecursor]);
    }
}

void __fastcall__ prevPage(void)
{
    pagecursor--;
    if(pagecursor<0) pagecursor = 0;
    writeAIText(page[pagecursor]);
}

void __fastcall__ nextDirPage(void)
{
    pagecursor++;
    if(pagecursor>=(totalFileCount/17)+1) 
        pagecursor = (totalFileCount/17);
    else
        page[pagecursor] = aiCursorY;

    clear_sid_list();

    if(page[pagecursor] < (AI_TEXT_END-AI_TEXT_ADDR))
        print_sid_list(page[pagecursor]); 
    else
    {
        pagecursor--;
        if(pagecursor<0) pagecursor = 0;
        totalFileCount = (pagecursor+1)*17-1;
        print_sid_list(page[pagecursor]);
    }
}

void __fastcall__ prevDirPage(void)
{
    pagecursor--;
    if(pagecursor<0) pagecursor = 0;
    clear_sid_list();
    print_sid_list(page[pagecursor]);
}

void __fastcall__ print_sid_list(uint16_t idx)
{
    unsigned char x = 2;
    unsigned char y = 3;

    while (y < 20)  // 20. satıra kadar
    {
        unsigned char ch = aitext[idx];
        idx++;

        if (ch == 0)   // 00 → tamamen bitir
            break;

        if (ch == 0x0D || x > PAPERWIDTH)  // 0D → satır başına geç
        {
            y++;
            x = 2;
            continue;
        }

        cputcxy(x, y, ch);
        x++;
    }
    aiCursorY = idx;
    showPageNumberDir();
    textcolor(COLOR_BLACK);
    gotoxy(1, 23);
}

void __fastcall__ m_cls(void)
{
    unsigned char y;
    for (y=0; y<22; y++)
    {
        cclearxy(2, y, PAPERWIDTH);
    }
    gotoxy(1, 23);
}

void __fastcall__ writeStatus(char msg[24])
{
    textcolor(COLOR_BLACK);
    cclearxy(11, 21, 24);
    gotoxy(11+(24-strlen(msg)), 21);
    cprintf("%s", msg);
}

void __fastcall__ sidinfo(void)
{
    unsigned char ts;
    char next[32];
    if (PEEKW(0x0f84)!=0x4449) return; // PSID  ID yoksa çık

    sid_pause_inirq2();
    sid_mute();

    ts = getSidSongs();

    m_cls();
    textcolor(COLOR_GRAY1);
    gotoxy(2, 1);
    cprintf("SID Tune Info");
    gotoxy(2, 3);
    cprintf("Name");
    gotoxy(2, 6);
    cprintf("Author");
    gotoxy(2, 9);
    cprintf("Released");

    textcolor(COLOR_BLACK);
    cputsxy(2, 4, getSidInfo(NAME));
    cputsxy(2, 7, getSidInfo(AUTHOR));
    cputsxy(2, 10, getSidInfo(RELEASED));

    sid_unmute();
    if (playingsid)
        sid_play_inirq2();
    else
        sid_pause_inirq2();

    if (ts > 1)
    {
        sprintf(next, "Playing song %d of %d.", song+1, ts);
        cputsxy(2, 12, next);
        cputsxy(2, 13, "Press F5 for the next song.");
    }
    gotoxy(1, 23);
}

// show sid info page?
bool __fastcall__ isSidInfo(void)
{
    // 0x047a -> $4e
    // 0x042a -> $53
    // 0x04f2 -> $41
    return PEEK(0x047a) == 0x4e && PEEK(0x042a) == 0x53 && PEEK(0x04f2) == 0x41;
}
