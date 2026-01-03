
#ifndef __C64SID__
#define __C64SID__

#include <string.h>
#include <peekpoke.h>
#include <stdbool.h>
#include <ctype.h>
#include <c64.h>

extern unsigned char get_sid_songs();
extern unsigned char get_sid_startsong();

typedef enum sidinfo
{
        NAME            = 0x0f98+00,
        AUTHOR          = 0x0f98+32,
        RELEASED        = 0x0f98+64,
} SidInfo;

char* getSidInfo(SidInfo adr);
unsigned char getSidSongs();
unsigned char getSidStartSong();

#endif