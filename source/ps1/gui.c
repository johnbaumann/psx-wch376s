#include "gui.h"

#include <libgpu.h>
#include <stdio.h>

#define WINDOW_COUNT 4

Theme theme;
Window windows[WINDOW_COUNT];
u_char window_index;
u_char active_window = 3;

char window_sort_list[WINDOW_COUNT];

TIM_IMAGE cursor_image;
DR_TPAGE *cursor_tpage;

SPRT *cursor;
TILE *tile;

RECT cursor_rect = {10, 10, 8, 10};
RECT screen = {0, 0, 512, 240};

char menu_index = 0;
char text_out[30 * 64];

RECT text_rect;

short cursor_x_velocity = 0;
short cursor_y_velocity = 0;

void GUI_DrawCursor(RECT *cursor_rect);

void GUI_DrawAllWindows(void)
{
    RECT temp;
    window_index = 0;
    for (int i = 0; i < WINDOW_COUNT; i++)
    {
        GUI_DrawWindow(windows[i]); // Draw all windows
        switch (i)
        {
        case 0:
        case 1:
        case 2:
            //DrawMessage("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus elementum lorem a massa egestas, id rhoncus eros semper.", windows[i].rect.x + 2, windows[i].rect.y + 12, windows[i].rect.w - 4, windows[i].rect.h - 14, true);
            // DrawMessage("Test.", windows[i].rect.x + 2, windows[i].rect.y + 12, windows[i].rect.w - 4, windows[i].rect.h - 14, true);
            break;

        case 3:
            // DrawMessage(text_out, windows[i].rect.x + 2, windows[i].rect.y + 12, screen_width, screen_height, false);
            break;
        }
        window_index++;
    }

    {
        temp = cursor_rect;

        temp.x += cursor_x_velocity;
        if (IsInBound(screen, temp))
        {
            cursor_rect.x = temp.x;
        }
        else
        {
            temp.x = cursor_rect.x;
        }
        temp.y += cursor_y_velocity;

        if (IsInBound(screen, temp))
        {
            cursor_rect.y = temp.y;
        }
    }

    GUI_DrawCursor(&cursor_rect);
}

void GUI_DrawCursor(RECT *cursor_rect)
{
    cursor = (SPRT *)nextpri;
    setSprt(cursor);
    setRGB0(cursor, 128, 128, 128);
    setXY0(cursor, cursor_rect->x, cursor_rect->y);
    setWH(cursor, cursor_image.prect->w * 4, cursor_image.prect->h);
    setUV0(cursor, cursor_image.prect->x * 4, cursor_image.prect->y);
    setClut(cursor, cursor_image.crect->x, cursor_image.crect->y);
    addPrim(&ot[db][z_depth], cursor);
    nextpri += sizeof(SPRT);

    // cursor_tpage = (DR_TPAGE *)nextpri;
    // SetDrawTPage(cursor_tpage, 0, 1, GetTPage(cursor_image.mode & 0x3, 0, cursor_image.prect->x, cursor_image.prect->y));

    // AddPrim(&ot[db][z_depth], cursor_tpage);
    // nextpri += sizeof(DR_TPAGE);
    z_depth--;
}

void GUI_DrawWindow(Window window)
{
    tile = (TILE *)nextpri;

    // Border box
    setTile(tile);
    setRGB0(tile, theme.Border.R, theme.Border.G, theme.Border.B);
    tile->x0 = window.rect.x;
    tile->y0 = window.rect.y;
    tile->w = window.rect.w;
    tile->h = window.rect.h;
    addPrim(&ot[db][z_depth], tile); // add poly to the Ordering table
    nextpri += sizeof(TILE);
    z_depth--;
    // Border box

    // Background box
    tile = (TILE *)nextpri;
    setTile(tile);
    setRGB0(tile, theme.Foreground.R, theme.Foreground.G, theme.Foreground.B);
    tile->x0 = window.rect.x + 1;
    tile->y0 = window.rect.y + 10;
    tile->w = window.rect.w - 2;
    tile->h = window.rect.h - 11;
    addPrim(&ot[db][z_depth], tile); // add poly to the Ordering table
    nextpri += sizeof(TILE);
    z_depth--;
    // Background box

    // Title Box
    tile = (TILE *)nextpri;
    setTile(tile);
    if (window_index == active_window)
    {
        setRGB0(tile, theme.ActiveTitle.R, theme.ActiveTitle.G, theme.ActiveTitle.B);
    }
    else
    {
        setRGB0(tile, theme.InactiveTitle.R, theme.InactiveTitle.G, theme.InactiveTitle.B);
    }
    tile->x0 = window.rect.x + 1;
    tile->y0 = window.rect.y + 1;
    tile->w = window.rect.w - 2;
    tile->h = 9;
    addPrim(&ot[db][z_depth], tile); // add poly to the Ordering table
    nextpri += sizeof(TILE);
    z_depth--;
    // Title Box

    // Title text
    DrawMessage(window.title, window.rect.x + 2 + 10, window.rect.y + 2, window.rect.w - 2, 10, false);

    // Close window button = x
    DrawChar('x', window.rect.x + window.rect.w - 2 - GLYPH_WIDTH, window.rect.y + 1);

    // Minimize window button = ^
    DrawChar('^', window.rect.x + 2, window.rect.y + 4);
}

void GUI_InitCursor(void)
{
    LoadTexture(cursor_data, &cursor_image);
}

void GUI_InitTheme(void)
{
    theme.Background.R = 0;
    theme.Background.G = 130;
    theme.Background.B = 130;

    theme.Border.R = 63;
    theme.Border.G = 63;
    theme.Border.B = 72;

    theme.ActiveTitle.R = 41;
    theme.ActiveTitle.G = 74;
    theme.ActiveTitle.B = 122;

    theme.InactiveTitle.R = 57;
    theme.InactiveTitle.G = 57;
    theme.InactiveTitle.B = 57;

    theme.Foreground.R = 15;
    theme.Foreground.G = 15;
    theme.Foreground.B = 15;
}

void GUI_InitWindows(void)
{
    for (int i = 0; i < WINDOW_COUNT; i++)
    {
        windows[i].rect.w = 320;
        windows[i].rect.h = 128;
        windows[i].rect.x = ((32 * i) + 8) % screen_width;
        windows[i].rect.y = ((24 * i) + 8) % screen_height;
        windows[i].visible = true;
        switch (i)
        {
        case 0:
        case 1:
        case 2:
            sprintf(windows[i].title, "Hello world");
            break;

        case 3:
            sprintf(windows[i].title, "USB Disk Info");
            break;
        }
    }
    window_index = 0;
}

short WhichWindow(RECT rect)
{
    for (int i = 0; i < WINDOW_COUNT; i++)
    {
        if (windows[i].visible)
        {
            /*if (CheckCollision(rect, windows[i].rect))
            {
                return i;
            }*/
        }
    }
    return -1;
}

void MoveToFront(char *item_list, int count, int index)
{
    char temp;

    if (count == 0 || index == 0 || index >= count)
        return; // derp?

    temp = item_list[index];

    for (int i = index; i > 0; i--)
    {
        item_list[i] = item_list[i - 1];
    }

    item_list[0] = temp;
}