#include <sys/types.h>
#include <libetc.h>

#include <stdio.h>
//#include <libapi.h> // Root counters
#include <stdint.h>

#include "ps1/filesystem.h"
#include "ps1/graphics.h"
#include "ps1/pads.h"
#include "ps1/variabletypes.h"

bool program_paused;

int main()
{
	int activeBuffer;

	program_paused = false;

	InitPads();
	InitGraphics();

	while (1)
	{
		UpdatePads();
		HandleSystemPadEvents();

		if (!program_paused)
		{

		}
		DisplayAll();
		FntPrint("Hello world\n");
	}
	return 0;
}
