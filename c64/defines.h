#ifndef __DEFINES__
#define __DEFINES__

// Max. block for SID file ($1000-$3000)
#define MAXBLOCK 32

#define DRIVERNAME  "c64-up2400.ser"

#define SCREEN_CHAR(row, col) (*(unsigned char*)(0x0400 + 40*(row) + (col)))
#define BACKGROUND  COLOR_BLUE
#define KEY_DEL     0x14
#define KEY_ENTER   0x0d
#define PAPERWIDTH  0x21

#endif //__DEFINES__