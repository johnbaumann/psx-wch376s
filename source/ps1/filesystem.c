#include "ps1/filesystem.h"

//#include <libsn.h>
#include "kernel/pcdrv.h" // nugget ftw!

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define USEPCDRV 1

bool pcdrv_active;


static int GetFileLengthCDROM(char *path);
static int GetFileLengthPCDRV(char *path);
static int InitCDROM();
static int InitPCDRV();
static int LoadFileCDROM(char *path, void *destination);
static int LoadFilePCDRV(char *path, void *destination);

int GetFileLength(char *path)
{
    if (pcdrv_active)
    {
        return GetFileLengthPCDRV(path);
    }
    else
    {
        return GetFileLengthCDROM(path);
    }
}

static int GetFileLengthCDROM(char *path)
{
    
}

static int GetFileLengthPCDRV(char *path)
{
    int file_handle;
    int file_length;

    printf("Load_ROM_PCDRV entry point\n");
    if (InitPCDRV() == 0)
    {
        printf("Finished PCinit(), proceed with loading file\n");
        file_handle = PCopen(path, 0, 0);

        if (file_handle < 0)
        {
            printf("Failed to open file\n");
        }
        else
        {
            printf("Handle = %i\n", file_handle);
            file_length = PClseek(file_handle, 0, 2);
            printf("file_length = %i\n", file_length);
            PClseek(file_handle, 0, 0);
            if (file_length > 0)
            {
                return file_length;
            }
            else
            {
                printf("Failed to retreive contents of file(file_length = 0)\n");
            }
            PCclose(file_handle);
        }
    }
}

void InitFilesystem(void)
{
    int pcinit_result;
    if (USEPCDRV)
    {
        pcinit_result = InitPCDRV();
        pcinit_result = 0; // Workaround bad return value for now
        if (pcinit_result < 0)
        {
            printf("Failed to load PCDRV, falling back to CDROM. pcinit_result = %i\n", pcinit_result);
            pcdrv_active = false;
            InitCDROM();
        }
        else
        {
            printf("PCDRV initialized\n");
            pcdrv_active = true;
        }
    }
    else
    {
        InitCDROM();
    }
}

static int InitCDROM()
{
}

static int InitPCDRV()
{
    return PCinit();
}

int LoadFile(char *path, void *destination)
{
    if (pcdrv_active)
    {
        return LoadFilePCDRV(path, destination);
    }
    else
    {
        return LoadFileCDROM(path, destination);
    }
}

static int LoadFileCDROM(char *path, void *destination)
{
    return 0;
}

static int LoadFilePCDRV(char *path, void *destination)
{
    int file_handle;
    int file_length;

    printf("Load_ROM_PCDRV entry point\n");
    if (InitPCDRV() == 0)
    {
        printf("Finished PCinit(), proceed with loading file\n");
        file_handle = PCopen(path, 0, 0);

        if (file_handle < 0)
        {
            printf("Failed to open file\n");
        }
        else
        {
            printf("Handle = %i\n", file_handle);
            file_length = PClseek(file_handle, 0, 2);
            printf("file_length = %i\n", file_length);
            PClseek(file_handle, 0, 0);
            if (file_length > 0)
            {
                if (PCread(file_handle, destination, file_length) == file_length)
                {
                    // Succesful Read
                    return file_length;
                }
                else
                {
                    printf("Failure, file length mismatch on read.\n");
                    return -1;
                }
            }
            else
            {
                printf("Failed to retreive contents of file(file_length = 0)\n");
            }
            PCclose(file_handle);
        }
    }
}