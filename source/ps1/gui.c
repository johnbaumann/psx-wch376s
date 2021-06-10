#include "ps1/gui.h"

#include "ps1/variabletypes.h"

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

RECT mouse_cursor_rect;
GsIMAGE mouse_cursor_image;
GsSPRITE mouse_cursor_sprite;

volatile u_short priority = 10;

void DrawWindow(GsOT *gsot_header, int x, int y, u_short width, u_short height)
{
    GsBOXF window_boxes[3];
    u_short priority_local = priority;

    // Box 1
    window_boxes[0].w = width;
    window_boxes[0].h = height;
    window_boxes[0].x = x;
    window_boxes[0].y = y;
    window_boxes[0].r = 93;
    window_boxes[0].g = 93;
    window_boxes[0].b = 128;
    GsSortBoxFill(&window_boxes[0], gsot_header, priority_local--);
    // Box 1

    // Box 2
    window_boxes[1].w = window_boxes[0].w - 2;
    window_boxes[1].h = window_boxes[0].h - 2;
    window_boxes[1].x = window_boxes[0].x + 1;
    window_boxes[1].y = window_boxes[0].y + 1; 
    window_boxes[1].r = 71;
    window_boxes[1].g = 71;
    window_boxes[1].b = 150;
    GsSortBoxFill(&window_boxes[1], gsot_header, priority_local--);
    // Box 2

    // Box 3
    window_boxes[2].w = window_boxes[1].w;
    window_boxes[2].h = window_boxes[1].h - 10;
    window_boxes[2].x = window_boxes[1].x;
    window_boxes[2].y = window_boxes[1].y + 10;
    window_boxes[2].r = 0;
    window_boxes[2].g = 0;
    window_boxes[2].b = 0;
    GsSortBoxFill(&window_boxes[2], gsot_header, priority_local--);
    // Box 3

    GsDrawOt(gsot_header);
    DrawSync(0);
    GsClearOt(0, 0, gsot_header);

    priority = priority_local;
}