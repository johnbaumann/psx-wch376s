#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <memory.h>

#include "ps1/graphics.h"
#include "ps1/pads.h"
#include "ps1/usbhostdemo.h"
#include "ps1/wch376s.h"

volatile bool demo_active = true;



int main(void)
{
    // Initialize the graphics
    InitGraphics();

    // Initialize pads
    InitPads();

    // Initialize the CH376S, gather info, etc
    USB_Host_Init();

    while (demo_active)
    {
        HandleSystemPadEvents();
        display();
    }

    return 0;
}