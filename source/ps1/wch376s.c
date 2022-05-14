#include "wch376s.h"

#include <stdbool.h>
#include <stdint.h>

static void delay1us();
static void delay2us();

const uint32_t base_address = 0x1F060008;
static volatile uint8_t *command_register = (uint8_t *)(base_address + 1);
static volatile uint8_t *data_register = (uint8_t *)(base_address);

static uint8_t last_int;

// Timing unconfirmed
static void delay1us()
{
    volatile int i;
    for (i = 0; i < 2; i++)
        ;
}

// Timing unconfirmed
static void delay2us()
{
    volatile int i;
    for (i = 0; i < 4; i++)
        ;
}

// Timing unconfirmed
void DelayMilliseconds(uint32_t milliseconds)
{
    volatile int i;
    for (i = 0; i < (uint32_t)(1694 * milliseconds); i++)
        ;
}

static bool between(uint8_t val, uint8_t beg, uint8_t end)
{
    return (val >= beg && val <= end);
}

uint8_t CH376_GetLastInt()
{
    return last_int;
}

bool CH376_WaitInt(uint32_t retry_count)
{
    for (int i = 0; i < retry_count; i++)
    {
        last_int = CH376_GetStatus();

        /*if (between(last_int, 0x00, 0x0F))
        {
            // Please refer to CH372 Datasheet for the interrupt status in USB device mode.
            printf("%02X Please refer to CH372 Datasheet for the interrupt status in USB device mode.\n", result);
        }*/
        if (between(last_int, 0x10, 0x1F))
        {
            // Operation interrupt status in SD card or USB host mode

            switch (last_int)
            {
            // case USB_INT_SUCCESS:
            // case USB_INT_CONNECT:
            // case USB_INT_DISCONNECT:
            // case USB_INT_USB_READY:
            // case USB_INT_DISK_READ:
            // case USB_INT_DISK_WRITE:
            default:
                return true;
                break;

            case USB_INT_DISK_ERR:
            case USB_INT_BUF_OVER:
                return false;
                break;
            }
        }
        else if (between(last_int, 0x20, 0x3F))
        {
            // Communication failure status in USB host mode, used to analyze the cause of operation failure
            switch (last_int)
            {
            case ERR_OPEN_DIR:
            case ERR_FOUND_NAME:
                return true;
                break;

            case ERR_MISS_FILE:
            case ERR_DISK_DISCON:
            case ERR_LARGE_SECTOR:
            case ERR_TYPE_ERROR:
            case ERR_BPB_ERROR:
            case ERR_DISK_FULL:
            case ERR_FDT_OVER:
            case ERR_FILE_CLOSE:
                return false;
                break;
            }
        }
        else if (between(last_int, 0x40, 0x4F))
        {
            // File system warning codes in SD card or USB host file mode
        }
        else if (between(last_int, 0x80, 0xBF))
        {
            // File system error codes in SD card or USB host file mode
            return false;
        }

        // if (result & 0x20)
        // {
        //     printf("%02X Failure bit set\n", result);
        //     return false;
        // }

        DelayMilliseconds(1);
    }

    // printf("%02X\n", result);
    return false;
}

uint8_t CH376_ReadCommand()
{
    return (uint8_t)(*command_register);
}

uint8_t CH376_ReadData()
{
    delay1us();
    return (uint8_t)(*data_register);
}

void CH376_WriteCommand(CH376_Command cmd)
{
    delay2us();
    *command_register = cmd;
    delay2us();
}

void CH376_WriteData(uint8_t param)
{
    *data_register = param;
    delay1us();
}

uint8_t CH376_GetVersion()
{
    CH376_WriteCommand(GET_IC_VER);
    return CH376_ReadData();
}

uint8_t CH376_SetBaudRate(uint8_t coefficient, uint8_t divisor)
{
    CH376_WriteCommand(SET_BAUDRATE);
    CH376_WriteData(coefficient);
    CH376_WriteData(divisor);

    return CH376_ReadData();
}

void CH376_EnterSleep()
{
    CH376_WriteCommand(ENTER_SLEEP);
}

void CH376_SetUSBSpeed(uint8_t speed)
{
    CH376_WriteCommand(SET_USB_SPEED);
    CH376_WriteData(speed);
}

void CH376_ResetAll()
{
    CH376_WriteCommand(RESET_ALL);
    DelayMilliseconds(35);
}

uint8_t CH376_CheckExists(uint8_t data)
{
    CH376_WriteCommand(CHECK_EXIST);
    CH376_WriteData(data);

    return CH376_ReadData();
}

bool CH376_CheckIfExists()
{
    return (CH376_CheckExists((uint8_t)0x0F) == 0xF0);
}

uint8_t CH376_GetStatus()
{
    CH376_WriteCommand(GET_STATUS);
    return CH376_ReadData();
}

bool CH376_SetUSBMode(CH376_USB_Mode mode)
{
    unsigned char i;
    CH376_WriteCommand(SET_USB_MODE);
    CH376_WriteData(mode);
    for (i = 0; i <= 100; i++)
    {
        if (CH376_ReadData() == Operation_Success)
            return true;
    }
    return false;
}

uint8_t CH376_ReadUSBData(uint8_t *buffer)
{
    uint8_t length;
    CH376_WriteCommand(RD_USB_DATA0);
    DelayMilliseconds(1);
    length = CH376_ReadData();

    for (int i = 0; i < length; i++)
    {
        buffer[i] = CH376_ReadData();
    }

    return length;
}

void CH376_SetFileName(uint8_t *name, uint32_t length)
{
    CH376_WriteCommand(SET_FILE_NAME);

    for (int i = 0; i < length; i++)
    {
        CH376_WriteData(name[i]);
        if (name[i] == '\0')
            return;
    }

    CH376_WriteData('\0');
}

bool CH376_MountDisk()
{
    uint8_t result;

    CH376_WriteCommand(DISK_MOUNT);
    DelayMilliseconds(130);

    return CH376_WaitInt(3000);
}

bool CH376_FileOpen()
{
    CH376_WriteCommand(FILE_OPEN);
    // DelayMilliseconds(100);

    if (CH376_WaitInt(3000))
        return true;
    else
        return false;
    return true;
}

bool CH376_FileEnumGo()
{
    CH376_WriteCommand(FILE_ENUM_GO);
    DelayMilliseconds(100);

    /*if (CH376_WaitInt(USB_INT_DISK_READ, 3000))
        return true;
    else
        return false;
    return true;*/
}

uint32_t CH376_DiskCapacity()
{
    uint32_t disk_capacity;

    uint8_t buffer[256];
    uint8_t buffer_length;

    CH376_WriteCommand(DISK_CAPACITY);

    if (CH376_WaitInt(3000))
    {
        buffer_length = CH376_ReadUSBData(buffer);

        for (int i = 0; i < 4; i++)
        {
            disk_capacity |= buffer[i] << (i * 8);
        }
    }
    return disk_capacity;
}

DiskInfo CH376_QueryDisk()
{
    uint8_t buffer[256];
    uint32_t buffer_length;

    DiskInfo out;
    CH376_WriteCommand(DISK_QUERY);
    DelayMilliseconds(3);

    if (CH376_WaitInt(3000))
    {
        buffer_length = CH376_ReadUSBData(buffer);

        out.capacity = 0;
        for (int i = 0; i < 4; i++)
        {
            out.capacity |= buffer[i] << (i * 8);
        }

        out.available = 0;
        for (int i = 4; i < 8; i++)
        {
            out.available |= buffer[i] << (i * 8);
        }
    }
    return out;
}

bool CH376_FileErase()
{
    CH376_WriteCommand(FILE_ERASE);
    DelayMilliseconds(100);

    return CH376_WaitInt(3000);
}

bool CH376_FileClose()
{
    CH376_WriteCommand(FILE_CLOSE);
    CH376_WriteData(0);
    DelayMilliseconds(100);

    return CH376_WaitInt(3000);
}

bool CH376_CreateDirectory()
{
    CH376_WriteCommand(DIR_CREATE);
    DelayMilliseconds(100);

    if (CH376_WaitInt(100))
        return true;
    else
        return false;
}
