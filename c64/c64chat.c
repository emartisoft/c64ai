#include <serial.h>
#include "c64dir.h"
#include "c64mouse.h"
#include "c64sid.h"
#include "version.h"
#include "defines.h"
#include "page.h"
#include "aimem.h"
#include "textbox.h"
#include "font.h"

char appname[18];
bool modeask;

bool thinking = false;
char m_filename[17];

unsigned char avaliableDev, avaliableDevCount, deviceIdx;
unsigned char avaliableDevices[23];
void __fastcall__ nextDevice(void);

struct mouse_info mouseInfo;
unsigned char col, row;

static unsigned char thinkcounter = 0;
static char thinkingAnim[4][2] = { "\x9b\0","\x9c\0","\x9d\0","\x9c\0"};

char diskname[25];
unsigned char endofaitext = 0; 
// The value that is 0 after enter will become 2 if \n comes from the server twice in a row
// the value will increase twice. If \n does not come, the value will always remain 0.
// If it is 2, the text from ai is finished.

// Serial port parameters compatible with PC side
static const struct ser_params serialParams = {
    SER_BAUD_2400,      /* Baudrate */
    SER_BITS_8,         /* Number of data bits */
    SER_STOP_1,         /* Number of pause bits */
    SER_PAR_NONE,       /* Parity setting */
    SER_HS_NONE         /* Type of handshake to use */
};

char c;
char line_buffer[255];
unsigned char i;
unsigned char buffer_pos = 0;

// to keep the old vector
void (*old_irq)(void);           
void __fastcall__ (*get_irq(void))(void) {
    unsigned low = PEEK(0x0314);
    unsigned high = PEEK(0x0315);
    return (void (*)(void))(low | (high << 8));
}

extern int sid_init(char musicindex);
extern int load_A_File();
extern int load_A_File_Border();

extern void pseudosid(void);
extern void menu(void);

extern void sid_irq_start(void);
extern void sid_irq_stop(void);

extern void setup_screen(void);
extern void show_sprite17(void);
extern void hide_sprite17(void);
extern void changePaperColor(void);
extern void printC64AI(void);

extern void set_sid_load_addr(void);
extern void set_doc_load_addr(void);

void __fastcall__ m_sidfiles(bool clear);
void __fastcall__ m_play(void);
void __fastcall__ m_pause(void);
void __fastcall__ m_help(void);
void __fastcall__ m_load(void);
void __fastcall__ m_new(void);


void __fastcall__ send(const char prompt[255])
{
    unsigned char i;
    // Send every character in buffer to PC
    for (i = 0; i < strlen(prompt); ++i) {
        ser_put(prompt[i]);
    }
    // Send end of line characters
    ser_put('\r');
    ser_put('\n');
}

bool __fastcall__ send_line() {
    char cmd[32];
    uint16_t x;
    unsigned char cc;

    // If there is command submission (starting with /)
    if (textbox[0] == '/') {
        modeask = true;
        // lang=xxxx
        if(textbox[1] == 'l' && textbox[2] == 'a')// && textbox[3] == 'n' && textbox[4] == 'g')
        {
            char temp[2];
            strcpy(cmd, "");
            for (x = 0; x < buf_len; ++x) {
                if(textbox[x] == 0x20) break;
                temp[0] = textbox[x];
                temp[1] = '\0';
                strncat(cmd, temp, 1);  // add only 1 character
            }
            send(cmd);
            send("cevaplama dilini belirten cumle kur\0");
        }
        // new
        else if (textbox[1] == 'n' && textbox[2] == 'e')// && textbox[3] == 'w')
        {
            m_new();
        }
        // play
        else if (textbox[1] == 'p' && textbox[2] == 'l')// && textbox[3] == 'a' && textbox[4] == 'y')
        {
            m_play();
            return false;
        }
        // pause
        else if (textbox[1] == 'p' && textbox[2] == 'a')// && textbox[3] == 'u' && textbox[4] == 's' && textbox[5] == 'e')
        {
            m_pause();
            return false;
        }
        // help
        else if (textbox[1] == 'h' && textbox[2] == 'e')// && textbox[3] == 'l' && textbox[4] == 'p')
        {
            m_cls();
            m_help();
            return false;
        }
        // sidfiles
        else if (textbox[1] == 's' && textbox[2] == 'i')// &&  textbox[3] == 'd' && textbox[4] == 'f' && textbox[5] == 'i' && textbox[6] == 'l' && textbox[7] == 'e' && textbox[8] == 's')
        {
            m_sidfiles(true);
            return false;
        }
        // load
        else if (textbox[1] == 'l' && textbox[2] == 'o')// && textbox[3] == 'a' && textbox[4] == 'd')
        {
            char temp[2];
            strcpy(m_filename, "");
            for (x = 0; x < buf_len-6; ++x) {
                if(textbox[x+6] == 0x20) break;
                temp[0] = textbox[x+6];
                temp[1] = '\0';
                strncat(m_filename, temp, 1);  // add only 1 character
            }
            m_filename[strlen(m_filename)] = '\0';
            m_load();
            return false;
        }
        
        return true;
    }

    // Send every character in buffer to PC
    for (x = 0; x < buf_len; ++x) {
        cc = textbox[x];
        // If the incoming character is in the range A-Z, convert it to lowercase
        if (cc >= 'A' && cc <= 'Z') {
            cc = cc + ('a' - 'A');
        }
        ser_put(cc);
    }
    // Send end of line characters
    ser_put('\r');
    ser_put('\n');
    return true; // return true to delete aitext
}

unsigned char __fastcall__ loadAFile(char* filename, unsigned char device, unsigned char flashing, unsigned char flashMode)
{
    POKEW(0xfc, (int) &filename[0]);
    POKE(0xfe, device);
    POKE(0xfb, strlen(filename));
    if(flashing)
            load_A_File_Border(flashMode);
    else    
            load_A_File();
    return PEEK(0xfb);
}

unsigned char __fastcall__ load(char* filename, unsigned char device)
{
    unsigned char result = 0xff; // loaded a file

    if (strchr(filename, '*') != NULL) {
        writeStatus("Invalid characters");
        return 88;
    }
    switch((int)(loadAFile(filename, device, true, 1)))
    {
        case 0: 
                writeStatus("Loading is cancelled");
                result = 0;
                break;
        case 5: 
                writeStatus("Device not present");
                result = 5;
                break;
        case 4: 
                writeStatus("File not found");
                result = 4;
                break;
        case 0x1d: 
                writeStatus("Load error");
                result = 0x1d;
                break;
        default: 
                break;
    }
    return result;
}

void __fastcall__ pressEnter(void)
{
    bool is_sid_cmd = (textbox[0] == '/' && textbox[1] == 's' && textbox[2] == 'i');
    modeask = false;
    
    // hide cursor
    hide_sprite17();
    if (send_line())
    {
        receivedLineCount = 0; 
        endofaitext = 0;
        resetPage();
        clear_aitext();
    }

    if (is_sid_cmd) {
        show_sprite17(); // Let's just point the cursor and exit
        return;
    }

    if (textbox[0] != '/')
    {
        m_cls();
        writeTextbox();
    }

    setCloneTextbox();
    clearTextbox();
    
    // show cursor
    show_sprite17();

    if(!modeask)
    {
        writeStatus("Thinking...");
    }

    textcolor(COLOR_BLACK);
    gotoxy(1, 23);
}

int main (void)
{
    uint16_t q;
    unsigned char counter = 0;
    unsigned char prevMouseButton = 0;
    bordercolor(BACKGROUND);
    changePaperColor();
    textcolor(COLOR_BLUE);
    clrscr();
    setFont();
    setup_screen();

    strcpy(appname, "C64AI ");
    strcat(appname, VER);

    textcolor(COLOR_BLUE);
    cputsxy(2, 1, appname);
    textcolor(COLOR_BROWN);
    cputsxy(2, 2,"\xac 2026  emarti, Murat \xa2zdemir");
    
    writeStatus("Press F1 to help");
    textcolor(COLOR_BLACK);

    gotoxy(2, 5);
    cprintf("Loading driver: %s", DRIVERNAME);

    deviceIdx = 0;
	avaliableDevCount = getAvaliableDevices();
	avaliableDev = avaliableDevices[deviceIdx]; // first device

    textcolor(COLOR_RED);
    // Install the serial port driver
    if (ser_load_driver(DRIVERNAME) != SER_ERR_OK) {
        cputsxy(2, 6, "Error: Unable to load driver");
        //return EXIT_FAILURE;
        while (true);
    }
    // Open port
    if (ser_open(&serialParams) != SER_ERR_OK) {
        cputsxy(2, 6, "Error: Unable to open port");
        ser_unload();
        //return EXIT_FAILURE;
        while (true);
    }
    
    textcolor(COLOR_BLACK);
    m_cls();

    for (q = 0; q < BUFFER_SIZE; ++q) cloneTextbox[q] = 0x20;

    modeask = true;
    pseudosid();
    menu();
    setCurPos();
    
    installMouse(); 
    old_irq = get_irq();
    mouse_move(320, 150);
    printC64AI();
    sid_irq_start();

    redrawLine();

    // In an infinite loop, listen to both the keyboard and the serial port
    while (true) {
        
        struct mouse_info mouseInfo;
        mouse_info (&mouseInfo);
        col = mouseInfo.pos.x/8;
		row = mouseInfo.pos.y/8;
        
        counter++;

        // thinking...
        if(!modeask && (counter % 100 == 0))
        {
            thinking = true;
            hide_sprite17();
            textcolor(COLOR_BLUE);
            cputsxy(34, 23, thinkingAnim[thinkcounter % 4]);
            thinkcounter++;  
        }

        // `ser_status()` checks if there is any data to read on the port
        if (ser_get(&c) == SER_ERR_OK) {
           
            if (!modeask){ 
                textcolor(COLOR_BLUE);
                modeask = true;
                writeStatus("Receiving data...");             
            }

            if (c == '\n') 
            {
                receivedLineCount++;
                write_ai_char('\r');
                write_ai_char('\n');
                 
                endofaitext++;
                if (endofaitext == 2)
                {
                    m_cls();
                    writeAIText(0);
                    cclearxy(24, 21, 11);
                    show_sprite17();
                    thinking = false;
                    textcolor(COLOR_BLUE);
                    cputsxy(34, 23, "\xb7\0"); // send icon
                    gotoxy(1, 23);
                    textcolor(COLOR_BLACK);
                    setCurPos();
                } 
            } 
            else 
            {
                write_ai_char(c);
                if(counter % 25 == 0)
                {
                    textcolor(COLOR_BLUE);
                    cputsxy(34, 23, thinkingAnim[thinkcounter % 4]);
                    gotoxy(1, 23);
                    thinkcounter++;
                }
                endofaitext = 0;
            }
            
            continue;
        }
        else
        {
             setCurPos();
        }
        // press any key?
        if (kbhit()) {
            
            c = cgetc();

            if (thinking || c == 19 || c == 147 || c == 148 ) 
                continue;

            switch (c)
            {
                // RETURN
                case KEY_ENTER:
                    if(buf_len > 1) pressEnter();
                    break;
                // DEL
                case 0x14: backspaceAtCursor(); break;
                // CURSOR LEFT & RIGHT
                case 0x9D: moveLeft(); break;
                case 0x1D: moveRight(); break;
                // CURSOR UP & DOWN
                case 0x11: {
                    switch (SCREEN_CHAR(21, 2))
                    {
                        case '\x74': nextPage(); break;
                        case '\x72': nextDirPage(); break;
                        default:
                            break;
                    }
                    break;
                }
                case 0x91: {
                    switch (SCREEN_CHAR(21, 2))
                    {
                        case '\x74': prevPage(); break;
                        case '\x72': prevDirPage(); break;
                        default:
                            break;
                    }
                    break;
                }
                // F7
                case 0x88: changePaperColor(); break;
                // F8
                case 0x8C: {
                    if (sendLastPrompt()) pressEnter();
                    break;
                }
                // F6
                case 0x8B: {
                    m_cls();
                    writeAIText(0);
                    pagecursor = 0;
                    showPageNumber();
                    textcolor(COLOR_BLACK);
                    gotoxy(1, 23);
                    break;
                }
                // F1
                case 0x85: {
                    m_cls();
                    clearTextbox();
                    m_help();
                    break;
                }
                // F3
                case 0x86: {
                    sidinfo();
                    break;
                }
                // F5
                case 0x87: { 
                    if (PEEKW(0x0f84)!=0x4449) continue;
                    song++;
                    song %= getSidSongs();
                    sid_init(song);
                    if (isSidInfo()) sidinfo();
                    break;
                }
                // F2
                case 0x89:
                    m_sidfiles(true);
                    break;

                // F4
                case 0x8A: {
                    if(PEEK(0x074a)==0x72) nextDevice();
                    break;
                }

                default:
                    break;
            }

            if(c==0x87||c==0x86||c==0x85||c==0x8B||c==0x8C||c==0x88||c==0x91||c==0x9D||c==0x1D||c==0x11||c==0x14) c=0x0d;


            if ((c >= ' ' && c <= '@') || (c >= 'A' && c <= 'Z') || (c >= '[' && c <= '`') || (c >= 'a' && c <= 'z') || (c >= '{' && c <= '~'))
                insertChar(c);
            

            adjustWindow();
            redrawLine();

            // cursor
            setCurPos();    
        } 
     
         if((mouseInfo.buttons & MOUSE_BTN_LEFT)&& !(prevMouseButton & MOUSE_BTN_LEFT)) 
         {
                unsigned char mc = SCREEN_CHAR(row, col);
                switch (mc)
                {
                    // page & dirpage control
                    case '\x74': prevPage(); break;
                    case '\x75': nextPage(); break;
                    case '\x72': prevDirPage(); break;
                    case '\x73': nextDirPage(); break;
                    case '\x77': if(buf_len>1){ 
                        pressEnter(); 
                        adjustWindow();
                        redrawLine();
                        setCurPos();  
                    }; break;
                    
                    default:
                        break;
                }
                
                if((PEEK(0x074a)==0x72) && (row==2 || row==1) && col<37) // if shows dir page
                {
                    nextDevice();
                }

                else if((SCREEN_CHAR(row, 14) == '.')&&(col<18)&&(col>1))
                {
                    // .sid 
                    if((SCREEN_CHAR(row,15)=='s'||0x40) && (SCREEN_CHAR(row,16)=='i'||0x40) && (SCREEN_CHAR(row,17)=='d'||0x40)) 
                    {
                        unsigned char cx, x;
                        char temp[2];
                        strcpy(m_filename, "");
                        for (x = 0; x < 16; ++x) {
                            cx = SCREEN_CHAR(row, x+2);
                            if( cx == 0x20) continue;
                            if (cx>=1 && cx<=26) { cx += 0x40;}
                            temp[0] = cx;
                            temp[1] = '\0';
                            strncat(m_filename, temp, 1);
                        }
                        m_filename[strlen(m_filename)] = '\0';
                        m_load();
                    }
                }
                else if (col>36) // menu command control
                {     
                    modeask = false;
                    if ((row>=1)&&(row<=3)) 
                    {
                        m_new();
                        textcolor(COLOR_BLACK);
                        gotoxy(1, 23);
                    }
                    else if ((row>=5)&&(row<=7))
                    {
                        changePaperColor();
                    }
                    else if ((row>=9)&&(row<=11))
                    {
                        m_sidfiles(true);
                    }
                    else if ((row>=12)&&(row<=14))
                    {
                        m_play();
                    }
                    else if ((row>=15)&&(row<=17))
                    {
                        m_pause();
                    }
                    else if ((row>=19)&&(row<=21))
                    {
                        m_cls();
                        clearTextbox();
                        m_help();
                    }
                    modeask = true;
                }
                
         }
        

        prevMouseButton = mouseInfo.buttons;
        // end of menu command control
        
        // blink cursor
        if (counter > 128){ __asm__("LDA #$30");} else { __asm__("LDA #$37");}
        __asm__("STA $07FF");
        
    }
}

// menu commands
void __fastcall__ printf_sid_header(void)
{
    char strDev[3];
    revers(1);
    textcolor(COLOR_GRAY1);
    cputsxy(2, 1, diskname);
   
    textcolor(COLOR_GRAY2);
    cputsxy(2, 2, "        Filename Blocks          ");
    sprintf(strDev, "%2d", avaliableDev);
    cputsxy(32, 2, strDev);
    
    if(totalFileCount == 0) 
    {
        cputsxy(2, 20, "No SID file found                ");
    } 
    else 
    {
        gotoxy(2, 20);
        cprintf("%3d SID file%s found              ", totalFileCount, (totalFileCount>1)?"s":" ");
    }
    revers(0);
    textcolor(COLOR_BLACK);
    resetPage();
}

void __fastcall__ m_sidfiles(bool clear)
{
    unsigned char dirCount;    
    
    if (clear)
    { 
        setup_screen();
        m_cls();
    }
    
    clearTextbox();
    clear_aitext();

    mouse_hide();
    sid_pause_inirq2();
    sid_mute();

    totalFileCount = 0;
    strcpy(diskname, "NO DISK         ");

    dirCount = getDir(avaliableDev);

    if((dirCount)>0)
    {
        uint16_t s = 0;
        strcpy(diskname, dir[0].name);
        
        for(i = 1; i <= dirCount - 2; i++) {
            if(strstr(dir[i].name, ".sid") != NULL) {
                if(dir[i].block <= MAXBLOCK) {
                    s++;
                    write_memory("%16s %4d  \n", dir[i].name, dir[i].block);
                }
            }
        }
        totalFileCount = s;
        strcat(diskname, "          Device ");
    }

    printf_sid_header();
    print_sid_list(0);
    showPageNumberDir();
    textcolor(COLOR_BLACK);
    gotoxy(1, 23);

    if (playingsid)
    sid_play_inirq2();
    else
    sid_pause_inirq2();

    mouse_show();
}

void __fastcall__ m_play(void)
{
    if (playingsid) return;   
    sid_play_inirq2();
    sid_unmute();
    playingsid = true;
}

void __fastcall__ m_pause(void)
{
    if(!playingsid) return;
    sid_pause_inirq2();
    sid_mute();
    playingsid = false;
}

void __fastcall__ m_help(void)
{
    char msg[24];
    set_doc_load_addr();

    mouse_hide();
    sid_mute();
    sid_irq_stop();

    strcpy(m_filename, "help");
    sprintf(msg, "Loading %s", m_filename);
    writeStatus(msg);

    if (load(m_filename, avaliableDev)==0xff)
    {
        receivedLineCount = 87;
        m_cls();
        writeAIText(0);
        pagecursor = 0;
        showPageNumber();
    }

    sid_irq_start();
    m_play();
    mouse_show();

    textcolor(COLOR_BLACK);
    gotoxy(1, 23);

    set_sid_load_addr();
}

void __fastcall__ m_load(void)
{
    char msg[24];
    mouse_hide();
    sid_mute();
    sid_irq_stop();

    sprintf(msg, "Loading %s", m_filename);
    writeStatus(msg);

    if (load(m_filename, avaliableDev)==0xff)
    {
        song = getSidStartSong() - 1;
        sid_init(song);  
        cclearxy(11, 21, 24);  
    }
    sid_irq_start();
    m_play();
    mouse_show();

    if (isSidInfo()) sidinfo();
    
    textcolor(COLOR_BLACK);
    gotoxy(1, 23);
}

void __fastcall__ m_new(void)
{
    setup_screen();
    m_cls();
    send("yeni sohbet için hazirlan");
    clear_aitext();
    clearTextbox();
    send("/new");
    receivedLineCount = 0;
    resetPage();
    showPageNumber();
}

void __fastcall__ nextDevice(void)
{
    unsigned char y;
    deviceIdx++;
    deviceIdx = deviceIdx % avaliableDevCount;
	avaliableDev = avaliableDevices[deviceIdx];
    
    for (y=3; y<22; y++)
    {
        cclearxy(2, y, PAPERWIDTH);
    }
    m_sidfiles(false);
}
