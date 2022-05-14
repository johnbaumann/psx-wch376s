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

extern char text_out[30*64];
extern int screen_height;
extern int screen_width;

void DrawMessage(const char *message, int32_t x_pos, int32_t y_pos, uint16_t draw_height, uint16_t draw_width, bool auto_wrap);
void DrawWindow(Window window);
void InitFont(void);
void InitGraphics(void);
void InitTheme(void);
void display(void);
void LoadTexture(u_long *tim_data, TIM_IMAGE * tim_image);

#endif // _GRAPHICS_H_