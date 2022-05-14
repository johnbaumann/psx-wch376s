#include "ps1/usbhostdemo.h"

#include "ps1/graphics.h"
#include "ps1/wch376s.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

void USB_Host_Init()
{
    uint8_t buffer[1024];
    uint8_t buffer_length;
    DiskInfo disk_info;
    uint64_t disk_size_temp;
    uint32_t file_size;
    uint8_t path[256] = {'*', '\0'};

    char device_name[256];

    uint8_t USBDirectory[255][32]; // holds the directory listing - max 255 files
    uint8_t file_count;            // number of files in the directory

    // text_out[0] = '\0';

    CH376_ResetAll();

    // Check if the device is connected
    if (!CH376_CheckIfExists())
    {
        // printf("No CH376 found\n");
    }
    else
    {
        // printf("Found device, checking version...\n");
        // PrintDeviceInfo();

        CH376_SetUSBMode(Mode_Enabled_USBHost_SOF);
        CH376_SetUSBMode(Mode_Enabled_USBHost_Reset);

        if (CH376_SetUSBMode(Mode_Enabled_USBHost_SOF))
        {
            // printf("USB mode set, mounting disk...\n");

            if (CH376_MountDisk())
            {
                // printf("Disk mounted, fetching disk id\n");
                buffer_length = CH376_ReadUSBData(buffer);
                if (buffer_length > 0)
                {
                    for (int i = 8; i < buffer_length; i++)
                    {
                        device_name[i - 8] = buffer[i];
                    }
                    device_name[buffer_length - 8] = '\0';
                    // printf("Device name: %s\n", device_name);
                    sprintf(text_out, "Device name: %s\n", device_name);
                }
                else
                {
                    sprintf(text_out, "No device found\n");
                }

                // printf("Query disk info\n");
                disk_info = CH376_QueryDisk();
                if (disk_info.capacity > 0)
                {
                    // printf("Succesfully queried disk\n");

                    // Calculate the disk size available in MB
                    disk_size_temp = (uint64_t)disk_info.available;
                    disk_size_temp *= 512;
                    disk_size_temp >>= 20;
                    sprintf(text_out + strlen(text_out), "Available: %iMB\n", (uint32_t)disk_size_temp);

                    // Calculate the disk capacity in MB
                    disk_size_temp = disk_info.capacity;
                    disk_size_temp *= 512;
                    disk_size_temp >>= 20;
                    sprintf(text_out + strlen(text_out), "Capacity: %iMB\n\n", (uint32_t)disk_size_temp);

                    CH376_SetFileName(path, strlen(path));
                    // printf("path = %s, strlen = %i\n", path, strlen(path));

                    // printf("Opening root directory and listing files\n");
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

                        if (file_count > 0)
                        {
                            sprintf(text_out + strlen(text_out), "Name     Ext   Size\n");
                            sprintf(text_out + strlen(text_out), "--------------------\n");
                        }
                        for (int x = 0; x < file_count; x++)
                        {
                            for (int i = 0; i < 32; i++)
                            {
                                if (i < 0xb)
                                    // if (USBDirectory[x][i] != ' ')
                                    sprintf(text_out + strlen(text_out), "%c", USBDirectory[x][i]);
                                if (i == 7)
                                {
                                    if (USBDirectory[x][i + 1] != ' ')
                                        sprintf(text_out + strlen(text_out), ".");
                                    else
                                        sprintf(text_out + strlen(text_out), " ");
                                }
                            }

                            file_size = USBDirectory[x][28];
                            file_size |= USBDirectory[x][29] << 8;
                            file_size |= USBDirectory[x][30] << 16;
                            file_size |= USBDirectory[x][31] << 24;
                            sprintf(text_out + strlen(text_out), "     %i\n", file_size);
                        }
                    }
                }
                else
                {
                    // printf("Failed to query disk\n");
                }
            }
            else
            {
                // printf("Disk mount failed\n");
            }
        }
        else
        {
            // printf("USB mode not set\n");
        }
    }
}