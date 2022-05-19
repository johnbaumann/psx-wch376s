#include "ps1/pads.h"

#include "ps1/gui.h"

#include <sys/types.h>
#include <libetc.h>
#include <stdbool.h>
#include <stdint.h>

u_long pad_mask;
u_long old_pad_mask;

extern bool demo_active;

void HandleSystemPadEvents()
{
    UpdatePads();

    // Restart
    if (pad_mask & Pad1Select && !(old_pad_mask & Pad1Select))
    {
        // demo_active = false;
        ((void (*)())0xBFC00000)();
    }
    // Restart

    // Pause
    if (pad_mask & Pad1Start && !(old_pad_mask & Pad1Start))
    {
        // program_paused = !program_paused;
    }
    // Pause

    // Up
    if (pad_mask & Pad1Up && !(old_pad_mask & Pad1Up))
    {
        cursor_y_velocity = 1;
    }
    // Up

    // Down
    if (pad_mask & Pad1Down && !(old_pad_mask & Pad1Down))
    {
        cursor_y_velocity = -1;
    }
    // Down

    // Left
    if (pad_mask & Pad1Left && !(old_pad_mask & Pad1Left))
    {
        cursor_x_velocity = -1;
    }
    // Left

    // Right
    if (pad_mask & Pad1Right && !(old_pad_mask & Pad1Right))
    {
        cursor_x_velocity = 1;
    }
    // Right
}

void InitPads()
{
    PadInit(0);
    pad_mask = old_pad_mask = PadRead(0);
}

void UpdatePads()
{
    old_pad_mask = pad_mask;
    pad_mask = PadRead(0);
}