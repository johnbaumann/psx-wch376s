#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <memory.h>

#include "ps1/graphics.h"
#include "ps1/wch376s.h"

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
    InitGraphics();

    uint8_t result;
    uint8_t buffer[1024];
    uint8_t buffer_length;
    DiskInfo disk_info;
    uint32_t disk_size_temp;
    uint8_t path[1024] = {'/', '\0'};

    uint8_t USBDirectory[255][32]; // holds the directory listing - max 255 files

    CH376_ResetAll();

    if (!CH376_CheckIfExists())
    {
        printf("No CH376 found\n");
    }
    else
    {
        printf("Found device, checking version...\n");
        PrintDeviceInfo();

        CH376_SetUSBMode(Mode_Enabled_USBHost_SOF);
        CH376_SetUSBMode(Mode_Enabled_USBHost_Reset);

        if (CH376_SetUSBMode(Mode_Enabled_USBHost_SOF))
        {
            printf("USB mode set\n");
            if (CH376_MountDisk())
            {
                printf("Disk mounted, fetching disk id\n");
                buffer_length = CH376_ReadUSBData(buffer);
                if (buffer_length > 0)
                {
                    for (int i = 8; i < buffer_length; i++)
                    {
                        printf("%c", buffer[i]);
                    }
                    printf("\n");
                }

                disk_info = CH376_QueryDisk();
                if (disk_info.capacity > 0)
                {
                    printf("Succesfully queried disk\n");
                    disk_size_temp = disk_info.available / 1000;
                    disk_size_temp *= 512;
                    disk_size_temp /= 1000;
                    printf("Available: %luMB\n", disk_size_temp);
                    disk_size_temp = disk_info.capacity / 1000;
                    disk_size_temp *= 512;
                    disk_size_temp /= 1000;
                    printf("Capacity: %luMB\n", disk_size_temp);

                    CH376_SetFileName(path, strlen(path));
                    printf("path = %s, strlen = %i\n", path, strlen(path));

                    if (CH376_FileOpen())
                    {
                        printf("Opened root directory, listing files\n");

                        buffer_length = CH376_ReadUSBData(buffer);
                        if (buffer_length > 0)
                        {
                            printf("buffer_length = %i\n", buffer_length);
                        }
                        else
                        {
                            printf("buffer_length = 0\n");
                        }



                    }
                    path[0] = '/';
                    path[1] = 'H';
                    path[2] = '/';
                    path[3] = '\0';
                    printf("path = %s, strlen = %i\n", path, strlen(path));
                    CH376_SetFileName(path, strlen(path));
                    if(CH376_CreateDirectory())
                    {
                        printf("Create directory returned true\n");
                    }
                    else
                    {
                        printf("Create directory returned false\n");
                    }
                }
                else
                {
                    printf("Failed to query disk\n");
                }
            }
            else
            {
                printf("Disk mount failed\n");
            }
        }
        else
        {
            printf("USB mode not set\n");
        }
    }

    printf("Exiting\n");

    while (1)
    {
        display();
    }
    return 0;
}