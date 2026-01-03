#include "c64sid.h"

char* getSidInfo(SidInfo adr)
{
        char name[32];
        register unsigned char i;
        i=0;
        while(PEEK(adr) != '\0')
        {  
                name[i] = toupper(PEEK(adr));
                ++adr;
                i++;
        }
        name[i] = '\0';

        return name;
}

unsigned char getSidSongs()
{
        return PEEK(0x0f91);
}

unsigned char getSidStartSong()
{
        return PEEK(0x0f93);
}
