#include "c64dir.h"

char* cbmFileTypes(unsigned char cft);
char* cbmFileAccess(unsigned char cfa);

DIRECTORY dir[32];

char info[6][32] =
{
    "couldn't read directory",
    "blocks free",
    "couldn't find start of file-name",
    "couldn't find end of file-name",
    "couldn't read file-type",
    "premature end of file"
};


int getDir(unsigned char device)
{
    register struct cbm_dirent cdirent;
    register unsigned char r, i;
    register bool loop;

    cbm_opendir(15, device);
    i=0;
    loop=true;
    while(loop){
        r = cbm_readdir(15, &cdirent);
        switch (r)
        {
            case 0: /* read a line for directory */
                if(cdirent.type==_CBM_T_HEADER)
                {
                    strcpy(dir[i].name, cdirent.name);
                }
                else
                {
                    //printf("%4d %16s %3s %2s\n\r", cdirent.size, cdirent.name, cbmFileTypes(cdirent.type), cbmFileAccess(cdirent.access));
                    strcpy(dir[i].name, cdirent.name);
                    strcpy(dir[i].type, cbmFileTypes(cdirent.type));
                    strcpy(dir[i].access, cbmFileAccess(cdirent.access));
                    dir[i].block = cdirent.size;
                }
                
                break;
                
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
                strcpy(dir[1].name, "");
                strcpy(dir[1].type, cbmFileTypes(0xff));
                strcpy(dir[1].access, cbmFileAccess(0xff));
                dir[1].block = 0;
                strcpy(dir[1].info, info[r-1]);
                i= -1;
                loop=false;
                break;
            case 2: /* blocks free */
                //printf("%d blocks free\n\r", cdirent.size);
                strcpy(dir[i].name, info[1]);
                strcpy(dir[i].type, "");
                strcpy(dir[i].access, "");
                dir[i].block = cdirent.size;
                loop=false;
                break;
            default:
                loop=false;
                break;
        }
        i++;
    }
    
    cbm_closedir(15);
    return i;
}

char* cbmFileTypes(unsigned char cft)
{
    switch (cft)
    {
        case _CBM_T_SEQ:        return "SEQ";
        case _CBM_T_PRG:        return "PRG";
        case _CBM_T_USR:        return "USR";
        case _CBM_T_REL:        return "REL";
        case _CBM_T_DEL:        return "DEL";
        case _CBM_T_OTHER:      return "???";
        case _CBM_T_HEADER:     return "DISK HEADER";
        case _CBM_T_CBM:        return "CBM"; // 1581 sub-partition
    
        default:
        break;
    }
    return "???";
}

char* cbmFileAccess(unsigned char cfa)
{
    switch (cfa)
    {
        case CBM_A_RO: return "RO";
        case CBM_A_WO: return "WO";
        case CBM_A_RW: return "RW";
        
        default:
            break;
    }

    return "??";
}

unsigned char getAvaliableDevices()
{
    register unsigned char dev, i;
    
    dev = getfirstdevice();
    i=0;
    while (dev!=INVALID_DEVICE) 
    {
        avaliableDevices[i] = dev; // 8-30 device number
        i++;
        dev=getnextdevice(dev); 
    }
    i=0;
    while (avaliableDevices[i]!=0)
    {
        i++;
    }
    
    return i;   
}
