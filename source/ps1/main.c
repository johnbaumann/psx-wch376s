#include <sys/types.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>

#include "ps1/graphics.h"

int main(void)
{
    InitGraphics();

    while (1)
    {
        display();
    }
    return 0;
}