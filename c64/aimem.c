#include "aimem.h"

unsigned char *aitext   = (unsigned char*)AI_TEXT_ADDR;
unsigned char *aiend    = (unsigned char*)AI_TEXT_END;
unsigned char *aicursor = (unsigned char*)AI_TEXT_ADDR;
uint16_t aiCursorY;

void __fastcall__ write_ai_char(unsigned char c) {
    if (aicursor < aiend) {
        *aicursor = c;   // memorize the character
        aicursor++;      // move the cursor one step forward
    }
}

void __fastcall__ clear_aitext()
{
    uint16_t q;
    // clear ai text
    for (q = 0; q < AI_TEXT_END - AI_TEXT_ADDR; ++q) aitext[q] = 0x00;
    aicursor = aitext;
}

void __fastcall__ writeAIText(uint16_t idx)
{
    bool bWrite = true;
    unsigned char p, y = 0;
    unsigned char line = 0;
    textcolor(COLOR_BLACK);

    // To prevent text overflow during screen printing
    // Place 00 at the top limit.
    aitext[AI_TEXT_END-AI_TEXT_ADDR-1] = 0x00;

    while (line < receivedLineCount && line < 21)
    {
        char buffer[34];
        bool inverse;
        unsigned char bpos = 0;

        // PAPERWIDTH character buffer is cleared.
        memset(buffer, ' ', PAPERWIDTH);
        buffer[PAPERWIDTH] = 0;   // C string terminator
        inverse = false;


        // Fill in the line
        while (aitext[idx] != 0x0A || aitext[idx+1] != 0x0D)
        {
            if (aitext[idx] == 0) {
                bWrite = false;
                break;
            }
            else if(aitext[idx] == 0x12 || aitext[idx] == 0x97) { inverse= true; revers(1); }
            else if(aitext[idx] == 0x92) { revers(0);}
            else if (bpos < PAPERWIDTH)
                buffer[bpos++] = (char)aitext[idx];

            idx++;
        }

        // Skip 0A 0D
        idx += 2;
        aiCursorY = idx;

        cclearxy(2, y, PAPERWIDTH);
        // Listing in bullet points (for rows starting with *)
        if (buffer[0] == 0x2a && buffer[1] == 0x20 && buffer[2] == 0x20) 
        {
            buffer[0] = 0xBA;
            for (p = 1; p < bpos-2; p++)
            {
                buffer[p+1] = buffer[p+3];
            }
            buffer[bpos-2] = '\0';
        }

        if (bWrite)
            cputsxy(2, y, buffer);
        if (inverse)
        {
            revers(0);
            inverse = false;
        }

        y++;
        line++;
    }

    revers(0);
    showPageNumber();
    textcolor(COLOR_BLACK);
    gotoxy(1, 23);
}

void write_memory(const char* fmt, ...) 
{
    uint16_t j;
    va_list args;
    char buf[40];
    int len;

    va_start(args, fmt);
    len = vsprintf(buf, fmt, args);
    va_end(args);

    for(j = 0; j < len; j++) {
        write_ai_char(buf[j]);
    }
}
