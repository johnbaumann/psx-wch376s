#include "ps1/pads.h"

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
        demo_active = false;
    }
    // Restart

    // Pause
    if (pad_mask & Pad1Start && !(old_pad_mask & Pad1Start))
    {
        //program_paused = !program_paused;
    }
    // Pause
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