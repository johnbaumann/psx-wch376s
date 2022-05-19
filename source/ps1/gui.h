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
extern uint8_t cursor_x_velocity;
extern uint8_t cursor_y_velocity;

extern char text_out[30*64];
extern Theme theme;

void GUI_DrawAllWindows(void);
void GUI_DrawWindow(Window window);
void GUI_InitCursor(void);
void GUI_InitTheme(void);
void GUI_InitWindows(void);

#endif // _GRAPHICS_H_