#include "ps1/pads.h"

#include "ps1/gui.h"

#include <sys/types.h>
#include <libetc.h>
#include <stdbool.h>
#include <stdint.h>

u_long pad_mask;
u_long old_pad_mask;

extern bool demo_active;

bool PadHeld(u_long pad_bit)
{
    return (pad_mask & pad_bit);
}

bool PadPressed(u_long pad_bit)
{
    if (pad_mask & pad_bit)
    {
        if (!(old_pad_mask & pad_bit))
        {
            return true;
        }
    }
    return false;
}

bool PadReleased(u_long pad_bit)
{
    if (!(pad_mask & pad_bit))
    {
        if (old_pad_mask & pad_bit)
        {
            return true;
        }
    }
    return false;
}

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

    // Vertical
    if(PadHeld(Pad1Up))
    {
        cursor_y_velocity = -2;
    }
    else if(PadHeld(Pad1Down))
    {
        cursor_y_velocity = 2;
    }
    else
    {
        cursor_y_velocity = 0;
    }
    // Vertical

    // Horizontal
    if(PadHeld(Pad1Left))
    {
        cursor_x_velocity = -2;
    }
    else if(PadHeld(Pad1Right))
    {
        cursor_x_velocity = 2;
    }
    else
    {
        cursor_x_velocity = 0;
    }
    // Horizontal
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