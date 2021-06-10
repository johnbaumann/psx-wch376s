#ifndef _GUI_H_
#define _GUI_H_

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

extern volatile u_short priority;

void DrawWindow(GsOT *gsot_header, int x, int y, u_short width, u_short height);

#endif // _GUI_H_