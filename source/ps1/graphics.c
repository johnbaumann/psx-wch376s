#include "ps1/graphics.h"

#include "ps1/gui.h"

#include <sys/types.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <stdbool.h>
#include <strings.h>

#define VMODE MODE_NTSC // Video Mode : 0 : NTSC, 1: PAL / MODE_NTSC

volatile uint8_t vsync_counter;
uint8_t vsyncs_per_second;

volatile uint8_t frame_count;
volatile uint8_t frame_rate;

int screen_height;
int screen_width;

DISPENV disp[2]; // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
u_long ot[2][OTLEN]; // double ordering table of length 8 * 32 = 256 bits / 32 bytes
uint8_t db = 0;      // index of which buffer is used, values 0, 1

uint16_t z_depth = OTLEN - 1;
int32_t current_tpage = -1;

extern u_long font_data[];
TIM_IMAGE font_image;

char primbuff[2][1024 * 32];
char *nextpri = primbuff[0];
SPRT *sprt_4b;
DR_TPAGE *tpage_4b;

bool ChecKRECTBounds(RECT container, RECT item)
{
    if (item.x >= container.x && item.x + item.w <= container.x + container.w && item.y >= container.y && item.y + item.h <= container.y + container.h)
    {
        return true;
    }
    return false;
}

void FontTPage()
{
    nextpri += sizeof(SPRT);
    tpage_4b = (DR_TPAGE *)nextpri;
    SetDrawTPage(tpage_4b, 0, 1, getTPage(font_image.mode & 0x3, 0, font_image.prect->x, font_image.prect->y));
    
    AddPrim(&ot[db][z_depth], tpage_4b);
    nextpri += sizeof(DR_TPAGE);
}

void DrawChar(uint8_t character, int x_pos, int y_pos)
{
    uint16_t glyph_index;

    uint16_t glyph_x;
    uint16_t glyph_y;

    if (character < 32 || character > 126)
    {
        glyph_index = 0;
    }
    else
    {
        glyph_index = (character - 32);
    }

    glyph_x = (glyph_index % 19) * 8;
    glyph_y = (glyph_index / 19) * 8;

    sprt_4b = (SPRT *)nextpri;
    SetSprt(sprt_4b);
    setRGB0(sprt_4b, 128, 128, 128);
    setXY0(sprt_4b, x_pos, y_pos);
    setWH(sprt_4b, 8, 8);
    setUV0(sprt_4b, glyph_x, glyph_y);
    setClut(sprt_4b, font_image.crect->x, font_image.crect->y);
    AddPrim(&ot[db][z_depth], sprt_4b);
    FontTPage();
}

void DrawMessage(const char *message, int32_t x_pos, int32_t y_pos, uint16_t draw_width, uint16_t draw_height, bool auto_wrap)
{
    int32_t _y = y_pos;
    int32_t _x = x_pos;

    for (uint16_t i = 0; i < (uint16_t)strlen(message); i++)
    {
        if (message[i] == '\n')
        {
            _y += GLYPH_HEIGHT + 1;
            _x = x_pos;
            continue;
        }

        if (auto_wrap && ((_x + GLYPH_WIDTH) >= x_pos + draw_width))
        {
            _y += GLYPH_HEIGHT + 1;
            _x = x_pos;
        }

        if (_x + GLYPH_WIDTH < x_pos + draw_width && _y + GLYPH_HEIGHT < y_pos + draw_height)
        {
            DrawChar(message[i], _x, _y);
            _x += GLYPH_WIDTH;
        }
    }
}

void InitFont(void)
{
    LoadTexture(font_data, &font_image);
}

void InitGraphics(void)
{
    GUI_InitTheme();

    ResetGraph(0);

    screen_width = 512;
    if (*(char *)0xbfc7ff52 == 'E') // SCEE
    {
        SetVideoMode(MODE_PAL);
        // screen_height = 256;
        screen_height = 240;
        vsyncs_per_second = 50;
    }
    else
    {
        SetVideoMode(MODE_NTSC);
        screen_height = 240;
        vsyncs_per_second = 60;
    }

    InitGeom();
    SetGeomOffset(screen_width / 2, screen_height / 2);
    SetGeomScreen(screen_width / 2);

    SetDefDispEnv(&disp[0], 0, 0, screen_width, screen_height);
    SetDefDispEnv(&disp[1], 0, screen_height, screen_width, screen_height);

    SetDefDrawEnv(&draw[0], 0, screen_height, screen_width, screen_height);
    SetDefDrawEnv(&draw[1], 0, 0, screen_width, screen_height);

    setRGB0(&draw[0], theme.Background.R, theme.Background.G, theme.Background.B);
    setRGB0(&draw[1], theme.Background.R, theme.Background.G, theme.Background.B);

    draw[0].isbg = 1;
    draw[1].isbg = 1;

    SetDispMask(1);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    InitFont();
    GUI_InitCursor();
    GUI_InitWindows();
}

void display(void)
{
    char menu_glyph = ' ';
    char print_buffer[64];
    z_depth = OTLEN - 1;

    ClearOTagR(ot[db], OTLEN);

    GUI_DrawAllWindows();

    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    DrawOTag(&ot[db][OTLEN - 1]);
    db = !db;
    nextpri = primbuff[db];
}

void LoadTexture(u_long *tim_data, TIM_IMAGE *tim_image)
{
    OpenTIM(tim_data);
    ReadTIM(tim_image);

    LoadImage(tim_image->prect, tim_image->paddr);
    DrawSync(0);

    // Clut
    if (tim_image->mode & 0x8)
    {
        // LoadClut(tim_image.caddr, tim_image.crect->x, tim_image.crect->y);
        LoadImage(tim_image->crect, tim_image->caddr);
        DrawSync(0);
    }
}