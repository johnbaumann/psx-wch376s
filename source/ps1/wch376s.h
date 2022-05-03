#ifndef _WCH376S_H_
#define _WCH376S_H_

#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>

typedef struct
{
    uint32_t capacity;
    uint32_t available;
} DiskInfo;

typedef enum
{
    FW_375 = 0xB7,    // Standard CH375 Module from Itead
    FW_376 = 0x41,    // Standard CH376 Module
    FW_376A = 0x42,   // Standard CH376 Module
    FW_376_SD = 0x44, // CH376 Module with SDCard Slot - * NOTE:  Some USB only models also report this without an SDcard slot.
    FW_376_SD2 = 0x43,
} CH376_FW_Version;

typedef enum
{
    // SD card or USB transaction or transmission operation or file operation is successful
    Int_Success = 0x14,

    // A USB device connection event is detected
    Int_Connect = 0x15,

    // A USB device disconnection event is detected
    Int_Disconnect = 0x16,

    // The transmitted data is wrong or the buffer overflows due to too many data
    Int_Overflow = 0x17,

    // USB device has been initialized (USB address has been assigned)
    Int_Ready = 0x18,

    // Storage device performs read operation, and requests data read
    Int_Disk_Read = 0x1D,

    // Storage device performs write operation, and requests data write
    Int_Disk_Write = 0x1E,

    // Storage device operation failed
    Int_Disk_Error = 0x1F
} CH376_InterruptStatus;

typedef enum
{
    Operation_Success = 0x51,
    Operation_Abort = 0x5F,
} CH376_OperationState;

typedef enum
{
    Mode_Disabled = 0x00,
    Mode_Enabled_USBDev_ExternalFW = 0x01,
    Mode_Enabled_USBDev_InternalFW = 0x02,
    Mode_Enabled_SDHost = 0x03,
    Mode_Disabled_USBHost = 0x04,
    Mode_Enabled_USBHost_NoSOF = 0x05,
    Mode_Enabled_USBHost_SOF = 0x06,
    Mode_Enabled_USBHost_Reset = 0x07,

} CH376_USB_Mode;

typedef enum
{
    // 01H - Get the chip and firmware versions
    // Input data: none
    // Output data: Version
    //
    // This command is used to get the chip and firmware versions. One byte of data returned is the version number,
    // the bit 7 is 0, the bit 6 is 1, and the bits 5-0 are the version number. If the returned value is 41H, remove bits
    // 7 and 6, and the version number will be 01H.
    GET_IC_VER = 0x01,

    // 02H - Set serial communication baud rate
    // Input data: Frequency division coefficient, Frequency division constant
    // Output data: (Wait for 1mS) Operation status
    //
    // This command is used to set the baud rate of CH376 for serial communication. When CH376 works in serial
    // communication mode, the default communication baud rate is set by the level combination of BZ/D4,
    // SCK/D5 and SDI/D6 pins (refer to Section 6.4 of this datasheet) after reset. When these pins are suspended,
    // the baud rate is 9600bps by default. If MCU supports high communication speed, the serial communication
    // baud rate can be dynamically regulated through this command. The command requires the input of two data,
    // namely, baud rate frequency division coefficient and frequency division constant. The following table shows
    // the relationship with the baud rate.
    // Usually, the serial port communication baud rate is set within 1mS. After completion, CH376 outputs the
    // operation state at the newly set communication baud rate. Therefore, MCU shall adjust its own
    // communication baud rate in time after sending the command.
    SET_BAUDRATE = 0x02,

    // 03H - Enter the low-power sleep suspend state
    // Input data: none
    // Output data: none
    ENTER_SLEEP = 0x03,

    // 04H - Set USB bus speed
    // Input data:  Bus speed
    // Output data: none
    SET_USB_SPEED = 0x04,

    // 05H - Execute hardware reset
    // Input data: none
    // Output data: (Wait for 35mS)
    RESET_ALL = 0x05,

    // 06H - Test the communication interface and operation status
    // Input data: Any data
    // Output data: Bitwise NOT
    CHECK_EXIST = 0x06,

    // 0AH - Get the data rate type of the USB device
    // Input data: Data 07H
    // Output data: Data rate type
    GET_DEV_RATE = 0x0A,

    // 0AH - Read the specified 8-bit file system variables
    // Input data: Variable address
    // Output data: Data
    READ_VAR8 = 0x0A,

    // 0BH - Set the interrupt mode for SDO pin of Interrupt mode SPI
    // Input data: Data 16H, Interrupt mode
    // Output data: none
    SET_SD0_INT = 0x0B,

    // 0BH - Set the number of retries for USB transaction operations
    // Input data: Data 25H, Number of retries
    // Output data: none
    SET_RETRY = 0x0B,

    // 0BH - Set the specified 8-bit file system variables
    // Input data: Variable address, Data
    // Output data: none
    WRITE_VAR8 = 0x0B,

    // 0CH - Get the current file length
    // Input data: Data 68H
    // Output data: File length (4)
    GET_FILE_SIZE = 0x0C,

    // 0C - Read the specified 32-bit file system variables
    // Input data: Variable address
    // Output data: Data (4 bytes)
    READ_VAR32 = 0x0C,

    // 0DH - Set the specified 32-bit file system variables
    // Input data: Variable address, Data (4 bytes)
    // Output data: none

    // 0FH - Delay 100uS
    // Input data: none
    // Output data: Delay status
    DELAY_100US = 0x0F,

    // 13H - Set USB address
    // Input data: Address value
    // Output data: none
    SET_USB_ADDR = 0x13,

    // 15h - Set USB working mode
    // Input data: Mode code
    // Output data: (Wait for 10uS), Operation status
    SET_USB_MODE = 0x15,

    // 16H - Check the connection status of USB device
    // Input data: none
    // Output data: (Wait for 2uS), Connection status
    TEST_CONNECT = 0x16,

    // 17H - Abort the retry of the current NAK.
    // Input data: none
    // Output data: none
    ABORT_NAK = 0x17,

    //  22H - Get the interrupt status and cancel the interrupt request
    // Input data:
    // Output data:
    GET_STATUS = 0x22,

    //  -
    // Input data:
    // Output data:
    RD_USB_DATA0 = 0x27,

    //  -
    // Input data:
    // Output data:
    WR_USB_DATA = 0x2C,

    //  -
    // Input data:
    // Output data:
    WR_REQ_DATA = 0x2D,

    //  -
    // Input data:
    // Output data:
    WR_OFS_DATA = 0x2E,

    //  -
    // Input data:
    // Output data:
    SET_FILE_NAME = 0x2F,

    //  -
    // Input data:
    // Output data:
    DISK_CONNECT = 0x30,

    //  -
    // Input data:
    // Output data:
    DISK_MOUNT = 0x31,

    //  -
    // Input data:
    // Output data:
    FILE_OPEN = 0x32,

    //  -
    // Input data:
    // Output data:
    FILE_ENUM_GO = 0x33,

    //  -
    // Input data:
    // Output data:
    FILE_CREATE = 0x34,

    //  -
    // Input data:
    // Output data:
    FILE_ERASE = 0x35,

    //  -
    // Input data:
    // Output data:
    FILE_CLOSE = 0x36,

    //  -
    // Input data:
    // Output data:
    DIR_INFO_READ = 0x37,

    //  -
    // Input data:
    // Output data:
    DIR_INF0_SAVE = 0x38,

    //  -
    // Input data:
    // Output data:
    BYTE_LOCATE = 0x39,

    //  -
    // Input data:
    // Output data:
    BYTE_READ = 0x3A,

    //  -
    // Input data:
    // Output data:
    BYTE_RD_GO = 0x3B,

    //  -
    // Input data:
    // Output data:
    BYTE_WRITE = 0x3C,

    //  -
    // Input data:
    // Output data:
    BYTE_WR_GO = 0x3D,

    //  -
    // Input data:
    // Output data:
    DISK_CAPACITY = 0x3E,

    //  -
    // Input data:
    // Output data:
    DISK_QUERY = 0x3F,

    //  -
    // Input data:
    // Output data:
    DIR_CREATE = 0x40,

    //  -
    // Input data:
    // Output data:
    SEG_LOCATE = 0x4A,

    //  -
    // Input data:
    // Output data:
    SEC_READ = 0x4B,

    //  -
    // Input data:
    // Output data:
    SEC_WRITE = 0x4C,

    //  -
    // Input data:
    // Output data:
    DISK_BOC_CMD = 0x50,

    //  -
    // Input data:
    // Output data:
    DISK_READ = 0x54,

    //  -
    // Input data:
    // Output data:
    DISK_RD_GO = 0x55,

    //  -
    // Input data:
    // Output data:
    DISK_WRITE = 0x56,

    //  -
    // Input data:
    // Output data:
    DISK_WR_GO = 0x57,
} CH376_Command;

static void delay1us();
static void delay2us();
void DelayMilliseconds(uint32_t milliseconds);

uint8_t CH376_ReadCommand();
uint8_t CH376_ReadData();
void CH376_WriteCommand(CH376_Command cmd);
void CH376_WriteData(uint8_t param);
bool CH376_CheckIfExists();

// GET_IC_VER
uint8_t CH376_GetVersion();

// SET_BAUDRATE
uint8_t CH376_SetBaudRate(uint8_t coefficient, uint8_t divisor);

// ENTER_SLEEP
void CH376_EnterSleep();

// SET_USB_SPEED
void CH376_SetUSBSpeed(uint8_t speed);
void CH376_ResetAll();
uint8_t CH376_CheckExists(uint8_t data);
// --
uint8_t CH376_GetStatus();
bool CH376_WaitInt(uint32_t delayMS);
bool CH376_SetUSBMode(CH376_USB_Mode mode);
uint8_t CH376_ReadUSBData(uint8_t *buffer);
void CH376_SetFileName(uint8_t *name, uint32_t length);
bool CH376_MountDisk();
bool CH376_FileOpen();
uint32_t CH376_DiskCapacity();
DiskInfo CH376_QueryDisk();
bool CH376_CreateDirectory();


const uint32_t base_address = 0x1F060008;
static volatile uint8_t *command_register = (uint8_t *)(base_address + 1);
static volatile uint8_t *data_register = (uint8_t *)(base_address);

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

bool CH376_WaitInt(uint32_t delayMS)
{
    for (int i = 0; i < delayMS; i++)
    {
        if (CH376_GetStatus() == Int_Success)
            return true;

        DelayMilliseconds(1);
    }

    return false;
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
            break;
    }
}

bool CH376_MountDisk()
{
    uint8_t result;

    CH376_WriteCommand(DISK_MOUNT);

    return CH376_WaitInt(3000);
}

bool CH376_FileOpen()
{
    CH376_WriteCommand(FILE_OPEN);

    if (CH376_WaitInt(3000))
        return true;
    else
        return false;
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
            disk_capacity |= buffer[i] << (i*8);
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

    if (CH376_WaitInt(3000))
    {
        buffer_length = CH376_ReadUSBData(buffer);

        out.capacity = 0;
        for (int i = 0; i < 4; i++)
        {
            out.capacity |= buffer[i] << (i*8);
        }

        out.available = 0;
        for (int i = 4; i < 8; i++)
        {
            out.available |= buffer[i] << (i*8);
        }
    }
    return out;
}

bool CH376_CreateDirectory()
{
    CH376_WriteCommand(DIR_CREATE);

    if (CH376_WaitInt(3000))
        return true;
    else
        return false;
}

#endif // _WCH376S_H_