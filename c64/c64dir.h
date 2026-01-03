#ifndef __C64DIR__
#define __C64DIR__

#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <device.h>
#include <string.h>
#include <c64.h>
#include <cbm.h>

struct directory 
{
    char name[17];
    char type[4];
    unsigned int block;
    char access[3];
    char info[32];
};

typedef struct directory DIRECTORY;
extern DIRECTORY dir[32];
int getDir(unsigned char device);

extern unsigned char avaliableDevices[23];
unsigned char getAvaliableDevices(); // returns avaliable device count

#endif