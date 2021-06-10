#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdint.h>

extern volatile uint8_t vsync_counter;
extern volatile uint8_t frame_count;
extern volatile uint8_t frame_rate;

void _VSyncCallback();
void InitGraphics();
void InitScreenTexture();
void InitSprite(GsIMAGE *im, GsSPRITE *sp);
void DisplayAll();
void UpdateScreenTexture();

#endif // _GRAPHICS_H_