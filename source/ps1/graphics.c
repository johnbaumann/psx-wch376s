#include "ps1/graphics.h"

#include "ps1/gui.h"

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <libapi.h>
#include <stdint.h>

#define ORDERING_TABLE_LENGTH 10
#define PACKETMAX 300

enum color_mode
{
    sixteen_bit = 0,
    twentyfour_bit = 1
};

GsOT gsot_header[2];
GsOT_TAG gsot_tag[2][1 << ORDERING_TABLE_LENGTH];

PACKET packet_area[2][PACKETMAX];

volatile uint8_t vsync_counter;
uint8_t vsyncs_per_second;

volatile uint8_t frame_count;
volatile uint8_t frame_rate;

u_short display_width;
u_short display_height;

static void DetectConsoleRegion();

void _VSyncCallback()
{
    vsync_counter++;
    if (vsync_counter >= vsyncs_per_second)
    {
        frame_rate = frame_count;
        frame_count = 0;
        vsync_counter = 0;
    }
}

static void DetectConsoleRegion()
{
    display_width = 320;

    if (*(char *)0xbfc7ff52 == 'E') // SCEE
    {
        SetVideoMode(MODE_PAL);
        display_height = 256;
        vsyncs_per_second = 50; // 50Hz
    }
    else
    {
        SetVideoMode(MODE_NTSC);
        display_height = 240;
        vsyncs_per_second = 60; // 60Hz
    }
}   

void DisplayAll()
{
    int active_buffer = GsGetActiveBuff();
    priority = ORDERING_TABLE_LENGTH - 1;

    FntFlush(-1);
    GsSetWorkBase((PACKET *)packet_area[active_buffer]);
    GsClearOt(0, 0, &gsot_header[active_buffer]);

    // Begin Drawing

    // To Do, dynamic per window z-depth
    DrawWindow(&gsot_header[active_buffer], 10, 50, 100, 50);
    DrawWindow(&gsot_header[active_buffer], 0, 20, 100, 50);
    DrawWindow(&gsot_header[active_buffer], 100, 150, 100, 50);
    // End Drawing

    DrawSync(0);

    frame_count++;
    VSync(0);
    GsSwapDispBuff();
    GsSortClear(0, 0, 77, &gsot_header[active_buffer]);

    GsDrawOt(&gsot_header[active_buffer]);
}

void InitGraphics()
{
    DetectConsoleRegion();

    GsInitGraph(display_width, display_height, GsNONINTER | GsOFSGPU, 1, sixteen_bit);
    GsDefDispBuff(0, 0, 0, display_height);

    gsot_header[0].length = ORDERING_TABLE_LENGTH;
    gsot_header[1].length = ORDERING_TABLE_LENGTH;
    gsot_header[0].org = gsot_tag[0];
    gsot_header[1].org = gsot_tag[1];
    GsClearOt(0, 0, &gsot_header[0]);
    GsClearOt(0, 0, &gsot_header[1]);

    FntLoad(960, 256);
    FntOpen(0, 8, display_width, display_height, 0, 512);

    vsync_counter = 0;
    frame_count = 0;
    frame_rate = 0;
    VSyncCallback(_VSyncCallback);
}

void InitSprite(GsIMAGE *im, GsSPRITE *sp)
{
    int bits;
    int widthCompression;
    RECT myRect;

    bits = im->pmode & 0x03;
    if (bits == 0)
        widthCompression = 4;
    else if (bits == 1)
        widthCompression = 2;
    else if (bits == 2)
        widthCompression = 1;
    else if (bits == 3)
    {
        //printf("\nunsupported file format (24bit tim)!\n");
    }

    myRect.x = im->px;
    myRect.y = im->py;
    myRect.w = im->pw;
    myRect.h = im->ph;
    LoadImage(&myRect, im->pixel); //loads image data to frame buffer

    //printf("\nimage bit type =%d\n", bits);

    sp->attribute = (bits << 24);
    sp->w = im->pw * widthCompression;
    sp->h = im->ph;
    sp->tpage = GetTPage(bits, 0, im->px, im->py);
    sp->u = 0;
    sp->v = 0;
    if (bits == 0 || bits == 1)
    {
        //checks if image is 4 or 8 bit
        myRect.x = im->cx;
        myRect.y = im->cy;
        myRect.w = im->cw;
        myRect.h = im->ch;
        LoadImage(&myRect, im->clut); //loads clut to frame buffer if needed
        sp->cx = im->cx;
        sp->cy = im->cy;
    }
    sp->r = 128;
    sp->g = 128;
    sp->b = 128;
    sp->mx = (im->pw * widthCompression) / 2;
    sp->my = im->ph / 2;
    sp->scalex = 4096;
    sp->scaley = 4096;
    sp->rotate = 0 * 4096;
}