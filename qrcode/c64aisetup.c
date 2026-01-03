#include <stdio.h>
#include <conio.h>

#define QR_SIZE 24
#define OUTPUT_BYTES 69
#define SCREEN ((unsigned char*)0x0450)
#define COLOR ((unsigned char*)0xD800)

const unsigned char qr2[OUTPUT_BYTES] = {
    0x7F, 0x19, 0xFC, 
    0x41, 0x09, 0x04, 
    0x5D, 0x51, 0x74, 
    0x5D, 0x35, 0x74, 
    0x5D, 0x21, 0x74, 
    0x41, 0x19, 0x04, 
    0x7F, 0x55, 0xFC, 
    0x00, 0x68, 0x00, 
    0x77, 0xD7, 0x10, 
    0x18, 0x36, 0x44, 
    0x4F, 0x87, 0x5C, 
    0x1E, 0x4E, 0x48, 
    0x29, 0xEA, 0xA0, 
    0x00, 0x5E, 0xCC, 
    0x7F, 0x75, 0x5C, 
    0x41, 0x7E, 0x40, 
    0x5D, 0x54, 0xA4, 
    0x5D, 0x15, 0xE0, 
    0x5D, 0x64, 0x54, 
    0x41, 0x6A, 0xE8, 
    0x7F, 0x4E, 0x2C
};

const char* info[] = {
"Find the",
"C64AI-Setup",
"Wi-Fi network",
"on your device,", 
"connect with the", 
"password", 
"12345678, and", 
"then navigate to", 
"http://10.42.0.1",
"in your browser,", 
"or scan the QR", 
"code for app", 
"settings.",
" ",
"After connecting", 
"to your own",
"Wi-Fi network,", 
"you can access",
"the settings",
"page using the", 
"local IP address", 
"assigned to the",
"device."
};

int main(void)
{
    unsigned char  y, x, k;
  
    textcolor(0);
    bordercolor(15);
    bgcolor(15);
    clrscr();

    for (y = 0; y < QR_SIZE - 2; y++) {
        for (x = 0; x < QR_SIZE; x++) {
            // Calculate which byte in qr2 to use
            unsigned int byte_index = y * 3 + (x / 8);
            // Calculate which bit in that byte to use
            unsigned int bit_position = 7 - (x % 8);
            // Check if the bit is set and draw the appropriate character
            SCREEN[(y + 1) * 40 + x] = ((qr2[byte_index] >> bit_position) & 1) ? 0xA0 : 0x20;
        }
    }

    textcolor(COLOR_RED);
    cputsxy(10,0,"Initial Setup Wizard");

    textcolor(COLOR_GRAY1);
    for (k = 0; k < 23; k++) cputsxy(24, k+2, info[k]);
    for(k=0;k<11;k++) COLOR[0x0090+k] = COLOR_BLUE;
    for(k=0;k<8;k++) COLOR[0x0158+k] = COLOR_BLUE;
    for(k=0;k<16;k++) COLOR[0x01a8+k] = COLOR_BLUE;

    while(1);
}
