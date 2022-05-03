#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ColorRGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ColorRGB;

typedef struct Theme
{
    ColorRGB BG;
    ColorRGB Border;
    ColorRGB ActiveTitle;
    ColorRGB InactiveTitle;
} Theme;

typedef struct Window
{
    RECT rect;
    char title[64];
    bool visible;
} Window;

void DrawWindow(Window window);
void InitFont(void);
void InitGraphics(void);
void InitTheme(void);
void display(void);
void LoadTexture(u_long *tim_data, TIM_IMAGE * tim_image);

#endif // _GRAPHICS_H_