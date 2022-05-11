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
#include "ps1/wch376s.h"

volatile bool demo_active = true;

void PrintDeviceInfo()
{
    switch (CH376_GetVersion())
    {
    case FW_375:
        printf("FW_375\n");
        break;

    case FW_376:
        printf("FW_376\n");
        break;

    case FW_376A:
        printf("FW_376A\n");
        break;

    case FW_376_SD:
        printf("FW_376_SD\n");
        break;

    case FW_376_SD2:
        printf("FW_376_SD2\n");
        break;

    default:
        printf("Unknown version\n");
        break;
    }
}

int main(void)
{
    uint8_t result;
    uint8_t buffer[1024];
    uint8_t buffer_length;
    DiskInfo disk_info;
    uint64_t disk_size_temp;
    uint32_t file_size;
    uint8_t path[256] = {'*', '\0'};

    uint8_t USBDirectory[255][32]; // holds the directory listing - max 255 files
    uint8_t file_count;            // number of files in the directory

    // Initialize the graphics
    InitGraphics();

    // Initialize pads
    InitPads();
    
    // Initialize the CH376S by resetting the device
    
    /*
    CH376_ResetAll();

    // Check if the device is connected
    if (!CH376_CheckIfExists())
    {
        //printf("No CH376 found\n");
    }
    else
    {
        //printf("Found device, checking version...\n");
        //PrintDeviceInfo();

        CH376_SetUSBMode(Mode_Enabled_USBHost_SOF);
        CH376_SetUSBMode(Mode_Enabled_USBHost_Reset);

        if (CH376_SetUSBMode(Mode_Enabled_USBHost_SOF))
        {
            //printf("USB mode set, mounting disk...\n");

            if (CH376_MountDisk())
            {
                //printf("Disk mounted, fetching disk id\n");
                buffer_length = CH376_ReadUSBData(buffer);
                if (buffer_length > 0)
                {
                    for (int i = 8; i < buffer_length; i++)
                    {
                        printf("%c", buffer[i]);
                    }
                    printf("\n");
                }

                //printf("Query disk info\n");
                disk_info = CH376_QueryDisk();
                if (disk_info.capacity > 0)
                {
                    //printf("Succesfully queried disk\n");

                    // Calculate the disk size available in MB
                    disk_size_temp = (uint64_t)disk_info.available;
                    disk_size_temp *= 512;
                    disk_size_temp >>= 20;
                    printf("Available: %iMB\n", (uint32_t)disk_size_temp);

                    // Calculate the disk capacity in MB
                    disk_size_temp = disk_info.capacity;
                    disk_size_temp *= 512;
                    disk_size_temp >>= 20;
                    printf("Capacity: %iMB\n", (uint32_t)disk_size_temp);

                    CH376_SetFileName(path, strlen(path));
                    //printf("path = %s, strlen = %i\n", path, strlen(path));

                    //printf("Opening root directory and listing files\n");
                    if (CH376_FileOpen())
                    {
                        for (int x = 0; x < 255; x++)
                        {
                            buffer_length = CH376_ReadUSBData(USBDirectory[x]);
                            if (buffer_length > 0)
                            {
                                file_count++;
                            }
                            else
                            {
                                break;
                            }

                            CH376_FileEnumGo();
                        }

                        CH376_FileClose();

                        if(file_count > 0)
                        {
                            printf("Name    Ext     Size\n");
                            printf("--------------------\n");
                        }
                        for (int x = 0; x < file_count; x++)
                        {
                            for (int i = 0; i < 32; i++)
                            {
                                if (i < 0xb)
                                    //if (USBDirectory[x][i] != ' ')
                                        printf("%c", USBDirectory[x][i]);
                                if (i == 7)
                                {
                                    if (USBDirectory[x][i + 1] != ' ')
                                        printf(".");
                                }
                            }

                            file_size = USBDirectory[x][28];
                            file_size |= USBDirectory[x][29] << 8;
                            file_size |= USBDirectory[x][30] << 16;
                            file_size |= USBDirectory[x][31] << 24;
                            printf("\t%i\n", file_size);
                        }
                    }
                }
                else
                {
                    //printf("Failed to query disk\n");
                }
            }
            else
            {
                //printf("Disk mount failed\n");
            }
        }
        else
        {
            //printf("USB mode not set\n");
        }
    }
    */

    printf("Entering main loop\n");
    while (demo_active)
    {
        HandleSystemPadEvents();
        display();
    }

    printf("Exiting\n");
    return 0;
}