#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdbool.h>
#include <stdint.h>

#define GLYPH_WIDTH 8
#define GLYPH_HEIGHT 8

#define OTLEN 512 // Ordering Table Length

typedef struct ColorRGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ColorRGB;

extern int screen_height;
extern int screen_width;

extern char primbuff[2][1024 * 32];
extern char *nextpri;

extern uint8_t db;
extern u_long ot[2][OTLEN];
extern uint16_t z_depth;

void FontTPage();
void DrawChar(uint8_t character, int x_pos, int y_pos);
void DrawMessage(const char *message, int32_t x_pos, int32_t y_pos, uint16_t draw_width, uint16_t draw_height, bool auto_wrap);
void InitFont(void);
void InitGraphics(void);
void display(void);
void LoadTexture(u_long *tim_data, TIM_IMAGE *tim_image);

#endif // _GRAPHICS_H_