#ifndef _GUI_H_
#define _GUI_H_

#include "ps1/graphics.h"

typedef struct Theme
{
    ColorRGB Background;
    ColorRGB Border;
    ColorRGB ActiveTitle;
    ColorRGB InactiveTitle;
    ColorRGB Foreground;
} Theme;

typedef struct Window
{
    RECT rect;
    char title[64];
    bool visible;
} Window;

extern u_long cursor_data[];
extern short cursor_x_velocity;
extern short cursor_y_velocity;

extern char text_out[30*64];
extern Theme theme;

void GUI_DrawAllWindows(void);
void GUI_DrawWindow(Window window);
void GUI_InitCursor(void);
void GUI_InitTheme(void);
void GUI_InitWindows(void);

void MoveToFront(char* item_list, int count, int index);

#endif // _GRAPHICS_H_