#include "ps1/graphics.h"

#include <sys/types.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <stdbool.h>
#include <strings.h>

#define VMODE MODE_NTSC // Video Mode : 0 : NTSC, 1: PAL / MODE_NTSC
#define OTLEN 1024      // Ordering Table Length

#define WINDOW_COUNT 1

#define GLYPH_WIDTH 8
#define GLYPH_HEIGHT 8

volatile uint8_t vsync_counter;
uint8_t vsyncs_per_second;

volatile uint8_t frame_count;
volatile uint8_t frame_rate;

int screen_height;
int screen_width;

Theme theme;
Window windows[WINDOW_COUNT];
u_char window_index;
u_char active_window = 1;

TILE window_tile[WINDOW_COUNT][3]; // 3 tiles per window: Border, bg, titlebar

DISPENV disp[2]; // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
u_long ot[2][OTLEN]; // double ordering table of length 8 * 32 = 256 bits / 32 bytes
uint8_t db = 0;      // index of which buffer is used, values 0, 1

int z_depth;

TIM_IMAGE test_image;
extern u_long font_data[];

char primbuff[2][32768] = {1};
char *nextpri = primbuff[0];
DR_TPAGE *tpage_4b;
SPRT *sprt_4b;

char menu_index = 0;
char log_buffer[20][64];

void InitWindows(void);

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
    setSprt(sprt_4b);
    setRGB0(sprt_4b, 128, 128, 128);
    setXY0(sprt_4b, x_pos, y_pos);
    setWH(sprt_4b, 8, 8);
    setUV0(sprt_4b, glyph_x, glyph_y);
    setClut(sprt_4b, test_image.crect->x, test_image.crect->y);
    addPrim(ot[db] + z_depth, sprt_4b);

    z_depth--;
    nextpri += sizeof(SPRT);
}

void DrawMessage(const char *message, int32_t x_pos, int32_t y_pos, bool auto_wrap)
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

        if (auto_wrap && ((_x + GLYPH_WIDTH) >= screen_width))
        {
            _y += GLYPH_HEIGHT + 1;
            _x = x_pos;
        }

        DrawChar(message[i], _x, _y);
        _x += (GLYPH_WIDTH + 1);
    }
}

void DrawFont(int x, int y)
{
    //
    sprt_4b = (SPRT *)nextpri;
    setSprt(sprt_4b);
    setRGB0(sprt_4b, 128, 128, 128);
    setXY0(sprt_4b, x, y);
    setWH(sprt_4b, 8, 8);
    setUV0(sprt_4b, 8, 0);
    setClut(sprt_4b, test_image.crect->x, test_image.crect->y);
    addPrim(ot[db], sprt_4b);

    nextpri += sizeof(SPRT);
}

void InitFont(void)
{
    LoadTexture(font_data, &test_image);

    //FntLoad(960, 0);
    //FntOpen(10, 10, SCREENXRES, SCREENYRES, 0, 280);
}

void InitGraphics(void)
{
    InitTheme();

    ResetGraph(0);

    screen_width = 320;
    if (*(char *)0xbfc7ff52 == 'E') // SCEE
    {
        SetVideoMode(MODE_PAL);
        //screen_height = 256;
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
    SetGeomOffset(screen_width / 2, screen_width / 2);
    SetGeomScreen(screen_width / 2);

    SetDefDispEnv(&disp[0], 0, 0, screen_width, screen_height);
    SetDefDispEnv(&disp[1], 0, screen_height, screen_width, screen_height);

    SetDefDrawEnv(&draw[0], 0, 0, screen_width, screen_height);
    SetDefDrawEnv(&draw[1], 0, screen_height, screen_width, screen_height);

    setRGB0(&draw[0], theme.BG.R, theme.BG.G, theme.BG.B);
    setRGB0(&draw[1], theme.BG.R, theme.BG.G, theme.BG.B);

    draw[0].isbg = 1;
    draw[1].isbg = 1;

    SetDispMask(1);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    InitFont();
    InitWindows();
}

void InitTheme(void)
{
    theme.BG.R = 0;
    theme.BG.G = 0;
    theme.BG.B = 75;

    theme.Border.R = 93;
    theme.Border.G = 93;
    theme.Border.B = 128;

    theme.ActiveTitle.R = 71;
    theme.ActiveTitle.G = 71;
    theme.ActiveTitle.B = 150;

    theme.InactiveTitle.R = 57;
    theme.InactiveTitle.G = 57;
    theme.InactiveTitle.B = 57;
}

void InitWindows(void)
{
    for (int i = 0; i < WINDOW_COUNT; i++)
    {
        windows[i].rect.w = 128;
        windows[i].rect.h = 64;
        windows[i].rect.x = (16 * i) % screen_width;
        windows[i].rect.y = (16 * i) % screen_height;
                windows[i].visible = true;
        sprintf(windows[i].title, "Window #%i", i);
    }
    window_index = 0;
}

void display(void)
{
    char menu_glyph = ' ';
    char print_buffer[64];
    z_depth = OTLEN - 1;

    ClearOTagR(ot[db], OTLEN);

    window_index = 0;
    active_window = WINDOW_COUNT - 1;
    for (int i = 0; i < WINDOW_COUNT; i++)
    {
        DrawWindow(windows[i]); // Draw all windows
    }

    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    DrawOTag(ot[db] + OTLEN - 1);

    db = !db;
    nextpri = primbuff[db];
}

void DrawWindow(Window window)
{
    // Border box
    setTile(&window_tile[window_index][0]);
    setRGB0(&window_tile[window_index][0], theme.Border.R, theme.Border.G, theme.Border.B);
    window_tile[window_index][0].x0 = window.rect.x;
    window_tile[window_index][0].y0 = window.rect.y;
    window_tile[window_index][0].w = window.rect.w;
    window_tile[window_index][0].h = window.rect.h;
    addPrim(ot[db] + z_depth, &window_tile[window_index][0]); // add poly to the Ordering table
    z_depth--;
    // Border box

    // Background box
    setTile(&window_tile[window_index][2]);
    setRGB0(&window_tile[window_index][2], 0, 0, 0);
    window_tile[window_index][2].x0 = window.rect.x + 1;
    window_tile[window_index][2].y0 = window.rect.y + 9;
    window_tile[window_index][2].w = window.rect.w - 2;
    window_tile[window_index][2].h = window.rect.h - 10;
    addPrim(ot[db] + z_depth, &window_tile[window_index][2]); // add poly to the Ordering table
    z_depth--;
    // Background box

    // Title Box
    setTile(&window_tile[window_index][1]);
    if (window_index == active_window)
    {
        setRGB0(&window_tile[window_index][1], theme.ActiveTitle.R, theme.ActiveTitle.G, theme.ActiveTitle.B);
    }
    else
    {
        setRGB0(&window_tile[window_index][1], theme.InactiveTitle.R, theme.InactiveTitle.G, theme.InactiveTitle.B);
    }
    window_tile[window_index][1].x0 = window.rect.x + 1;
    window_tile[window_index][1].y0 = window.rect.y + 1;
    window_tile[window_index][1].w = window.rect.w - 2;
    window_tile[window_index][1].h = 10;
    addPrim(ot[db] + z_depth, &window_tile[window_index][1]); // add poly to the Ordering table
    z_depth--;
    // Title Box

    // Title text
    DrawMessage(window.title, window.rect.x + 2 + 10, window.rect.y + 2, false);

    // Close window button = x
    DrawChar('x', window.rect.x + window.rect.w - 2 - GLYPH_WIDTH, window.rect.y + 1);

    // Minimize window button = ^
    DrawChar('^', window.rect.x + 2, window.rect.y + 4);

    window_index++;
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
        //LoadClut(tim_image.caddr, tim_image.crect->x, tim_image.crect->y);
        LoadImage(tim_image->crect, tim_image->caddr);
        DrawSync(0);
    }
}