#ifndef _WCH376S_H_
#define _WCH376S_H_

#include <stdbool.h>
#include <stdint.h>

// Sources:

// USB Flash Disk and SD Card File Management Control Chip
// CH376
// Datasheet
// Version: 1A
// http://wch.cn
//
// USB Flash Disk and SD Card File Management Control Chip
// CH376
// Datasheet (II): Auxiliary Commands and USB Basic Transmission Commands
// Version: 1
// http://wch.cn

typedef struct DiskInfo
{
    uint32_t capacity;
    uint32_t available;
} DiskInfo;

typedef enum CH376_OperationState
{
    Operation_Success = 0x51,
    Operation_Abort = 0x5F,
} CH376_OperationState;

typedef enum CH376_FW_Version
{
    FW_375 = 0xB7,    // Standard CH375 Module from Itead
    FW_376 = 0x41,    // Standard CH376 Module
    FW_376A = 0x42,   // Standard CH376 Module
    FW_376_SD = 0x44, // CH376 Module with SDCard Slot - * NOTE:  Some USB only models also report this without an SDcard slot.
    FW_376_SD2 = 0x43,
} CH376_FW_Version;

typedef enum CH376_USB_Mode
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

typedef enum CH376_InterruptStatus
{
    // 03H - Detection of USB bus reset (the bit 1 and bit 0 of interrupt status value is 11)
    USB_INT_BUS_RESET1 = 0x03,
    // 07H - Detection of USB bus reset (the bit 1 and bit 0 of interrupt status value is 11)
    USB_INT_BUS_RESET2 = 0x07,
    // 0BH - Detection of USB bus reset (the bit 1 and bit 0 of interrupt status value is 11)
    USB_INT_BUS_RESET3 = 0x0B,
    // 0FH - Detection of USB bus reset (the bit 1 and bit 0 of interrupt status value is 11)
    USB_INT_BUS_RESET4 = 0x0F,
    // 0CH - Receiver of endpoint 0 successfully receive SETUP
    USB_INT_EP0_SETUP = 0x0C,
    // 00H - Receiver of endpoint 0 successfully receive OUT
    USB_INT_EP0_OUT = 0x00,
    // 08H - Transfer of endpoint 0 successfully transfer IN
    USB_INT_EP0_IN = 0x08,
    // 01H - Assistant endpoint/endpoint 1 receive data, OUT successfully
    USB_INT_EP1_OUT = 0x01,
    // 09H - Interrupt endpoint/endpoint 1 transfer data, IN successfully
    USB_INT_EP1_IN = 0x09,
    // 02H - Bulk endpoint/endpoint 2 receive data, OUT successfully
    USB_INT_EP2_OUT = 0x02,
    // 0AH - Bulk endpoint/endpoint 2 transfer data, IN successfully
    USB_INT_EP2_IN = 0x0A,
    // 05H - USB bus suspending transaction (if finish CHK_SUSPEND)
    USB_INT_USB_SUSPEND = 0x05,
    // 06H - Wake up from sleep (if finish ENTER_SLEEP)
    USB_INT_WAKE_UP = 0x06,

    // 14H - SD card or USB transaction or transmission operation or file operation is successful
    USB_INT_SUCCESS = 0x14,
    // 15H - A USB device connection event is detected
    USB_INT_CONNECT = 0x15,
    // 16H - A USB device disconnection event is detected
    USB_INT_DISCONNECT = 0x16,
    // 17H - The transmitted data is wrong or the buffer overflows due to too many data
    USB_INT_BUF_OVER = 0x17,
    // 18H - USB device has been initialized (USB address has been assigned)
    USB_INT_USB_READY = 0x18,
    // 1DH - Storage device performs read operation, and requests data read
    USB_INT_DISK_READ = 0x1D,
    // 1EH - Storage device performs write operation, and requests data write
    USB_INT_DISK_WRITE = 0x1E,
    // 1FH - Storage device operation failed
    USB_INT_DISK_ERR = 0x1F,

    // 41H = The directory of the specified path is opened
    ERR_OPEN_DIR = 0x41,
    // 42H - The file of the specified path is not found. It may be a file name error
    ERR_MISS_FILE = 0x42,
    // 43H - The matched filename is searched, or a directory is required to be opened but a file is opened actually
    ERR_FOUND_NAME = 0x43,
    // 82H - The disk is not connected. It may have been disconnected
    ERR_DISK_DISCON = 0x82,
    // 84H - The sector of the disk is too large, and only 512 bytes is supported per sector
    ERR_LARGE_SECTOR = 0x84,
    // 92H - The disk partition type is not supported. The disk shall be repartitioned by the disk management tool
    ERR_TYPE_ERROR = 0x92,
    // A1H - The disk is not formatted, or the parameter is wrong, The disk shall be reformatted by WINDOWS using default parameters
    ERR_BPB_ERROR = 0xA1,
    // B1H - The disk is full of files, there is too little free space or there is no free space
    ERR_DISK_FULL = 0xB1,
    // B2H - There are too many files in the directory, and there are no free directory entries. It is necessary to manage the disk, The number of files in the root directory of FAT12/FAT16 shall be less than 512
    ERR_FDT_OVER = 0xB2,
    // B4H - The file has been closed. If it is required to be used, reopen it
    ERR_FILE_CLOSE = 0xB4,
} CH376_InterruptStatus;


typedef enum CH376_Command
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
    //
    // This command enables CH376 in a low-power sleep suspended state. After entering the low power state, the
    // clock of CH376 stops vibrating, thus saving power. The low power state is not quitted until one of the
    // following two situations is detected: first, the signal is detected on the USB bus (such as the USB host
    // starting transmission or USB device plugging event). Second, MCU writes new commands to CH376
    // (commands without input data, such as CMD_GET_IC_VER or CMD_ABORT_NAK). For SPI interface
    // communication interface mode, active SCS chip selection will also cause CH376 to exit the low-power state,
    // so MCU shall immediately disable the SCS chip selection after sending the command
    // CMD_ENTER_SLEEP.
    ENTER_SLEEP = 0x03,

    // 04H - Set USB bus speed
    // Input data:  Bus speed
    // Output data: none
    //
    // This command is used to set USB bus speed. This command needs the input of 1 data to select the USB bus
    // speed. 00H corresponds to 12Mbps full speed mode, 01H corresponds to 1.5MBps full speed mode
    // (non-standard mode), and 02H corresponds to 1.5Mbps low speed mode. The USB bus speed of CH376 is
    // 12Mbps full speed by default, and will be restored to 12Mbps full speed mode after the command
    // CMD_SET_USB_MODE is executed to set USB working mode.
    SET_USB_SPEED = 0x04,

    // 05H - Execute hardware reset
    // Input data: none
    // Output data: (Wait for 35mS)
    //
    // This command enables CH376 to perform a hardware reset. Typically, hardware reset is completed within
    // 35mS. In the parallel communication mode, hardware reset is generally completed within 1mS.
    RESET_ALL = 0x05,

    // 06H - Test the communication interface and operation status
    // Input data: Any data
    // Output data: Bitwise NOT
    //
    // This command is used to test the communication interface and working state to check whether CH376 is
    // working properly. This command needs to input 1 data, which can be any data. If CH376 is working properly,
    // the output data of CH376 will be the bitwise reverse of the input data. For example, if the input data is 57H,
    // the output data will be A8H. In addition, CH376 with parallel communication mode normally reads the data
    // 00H from its parallel port before receiving no command after its reset.
    CHECK_EXIST = 0x06,

    // 0AH - Get the data rate type of the USB device
    // Input data: Data 07H
    // Output data: Data rate type
    //
    // This command is used to get the data rate type of the currently connected USB device. This command
    // requires to input one data 07H, output the data rate type. If the bit 4 is 1, the device will be 1.5Mbps low
    // speed USB device; otherwise, it will be 12Mbps full speed USB device. This command is only valid in USB
    // mode 5 (enabled USB host mode, not generating SOF package).
    GET_DEV_RATE = 0x0A,

    // 0AH - Read the specified 8-bit file system variables
    // Input data: Variable address
    // Output data: Data
    //
    // This command is used to read the specified 8-bit (single byte) file system variables. This command requires
    // to input 1 data specified variable address and output the data for this variable.
    READ_VAR8 = 0x0A,

    // 0BH - Set the interrupt mode for SDO pin of Interrupt mode SPI
    // Input data: Data 16H, Interrupt mode
    // Output data: none
    //
    // This command is used to set the interrupt mode for SDO pin of SPI interface. The command requires to first
    // input one data 16H, and then input one new interrupt mode. There are two interrupt modes: 10H disable
    // SDO pin is used for interrupt output, and the three-state output is disabled when the SCS chip selection is
    // invalid, so as to share the SPI bus of MCU with other devices; 90H set SDO pin is always in the output state.
    // When the SCS chip selection is invalid, it serves as the interrupt request output, which is equivalent to the
    // INT# pin for MCU to query the interrupt request status.
    SET_SD0_INT = 0x0B,

    // 0BH - Set the number of retries for USB transaction operations
    // Input data: Data 25H, Number of retries
    // Output data: none
    //
    // This command is used to set the number of retries for USB transaction operations. This command requires to
    // input two data, respectively the data 25H and the number of retries.
    // The bits 7 and 6 of the number of retries specify the processing mode when CH376 receives NAK response.
    // If the bit 7 is 1 and the bit 6 is 0, infinite retry will be performed (the current retry can be given up through
    // the command CMD_ABORT_NAK); if the bit 7 is 1 and the bit 6 is 1, finite retry will be performed for 3
    // seconds around; if the bit 7 is 0, NAK will be notified to MCU as the result or processed as an error. Bits 5-0
    // of the number of retries specify the number of retries CH376 after the USB device response timeout. If the
    // bit is 0, retry will not be performed after timeout.
    // The default number of retries is 8FH after chip reset or USB mode reset, so infinite retry will be performed
    // after NAK reply, and the response is received, and 15 retries will be performed after USB device response
    // timeout.
    SET_RETRY = 0x0B,

    // 0BH - Set the specified 8-bit file system variables
    // Input data: Variable address, Data
    // Output data: none
    //
    // This command is used to set the specified 8-bit (single byte) file system variables. This command requires to
    // input two data, respectively the specified variable address and the specified variable data.
    WRITE_VAR8 = 0x0B,

    // 0CH - Get the current file length
    // Input data: Data 68H
    // Output data: File length (4)
    //
    // This command is used to get the length of the current file, namely the number of bytes. The command
    // requires to input one data 68H and output the length of the file currently being opened, which is a
    // double-word data (32 bits) expressed by 4 bytes with low bytes in front.
    // To set the new file length, refer to the command CMD_WRITE_VAR32 in Datasheet (II) to set the variable
    // VAR_FILE_SIZE.
    GET_FILE_SIZE = 0x0C,

    // 0C - Read the specified 32-bit file system variables
    // Input data: Variable address
    // Output data: Data (4 bytes)
    //
    // This command is used to read the specified 32-bit (4 bytes) file system variables. This command requires to
    // input 1 data specified variable address and output the data for this variable. The variable data has a total of 4
    // bytes, which are the lowest byte of data, the lower byte of data, the higher byte of data, and the highest byte
    // of data.
    READ_VAR32 = 0x0C,

    // 0DH - Set the specified 32-bit file system variables
    // Input data: Variable address, Data (4 bytes)
    // Output data: none
    //
    // This command is used to set the specified 32-bit (4 bytes) file system variables. This command requires to
    // input 5 data, respectively specified variable address, the lowest byte of variable data, lower byte of data,
    // higher byte of data and the highest byte of data.
    WRITE_VAR32 = 0x0D,

    // 0FH - Delay 100uS
    // Input data: none
    // Output data: Delay status
    //
    // This command is used to delay for 100uS and does not support serial port mode. The output data is 0 during
    // delay, and is non-0 (usually the chip version number) at the end of the delay. MCU determines whether the
    // delay is ended according to the read data.
    DELAY_100US = 0x0F,

    // 13H - Set USB address
    // Input data: Address value
    // Output data: none
    SET_USB_ADDR = 0x13,

    // 15h - Set USB working mode
    // Input data: Mode code
    // Output data: (Wait for 10uS), Operation status
    //
    // This command is used to set the USB device address. This command requires to input 1 data to select the
    // address of the USB device being operated. After reset or after the USB device is connected or disconnected,
    // the USB device address is always 00H, and MCU communicates with the USB device through the default
    // address 00H. If MCU sets the address of the USB device through the standard USB request, it must set the
    // same USB device address through the command, so that CH376 can communicate with the USB device
    // through the new address.
    SET_USB_MODE = 0x15,

    // 16H - Check the connection status of USB device
    // Input data: none
    // Output data: (Wait for 2uS), Connection status
    //
    // This command is used to query the connection status of the current USB device in USB host mode. Typically,
    // this command is completed within 2uS. After completion, USB_INT_CONNECT,
    // USB_INT_DISCONNECT, or USB_INT_USB_READY is output. USB_INT_CONNECT indicates that the
    // USB device is just connected or has been connected but has not been initialized. USB_INT_DISCONNECT
    // indicates that the USB device has not been connected or has been disconnected. Status
    // USB_INT_USB_READY indicates that USB device has been connected and initialized (USB address has
    // been assigned). 0 indicates that the command is not completed and the status can be read later.
    TEST_CONNECT = 0x16,

    // 17H - Abort the retry of the current NAK.
    // Input data: none
    // Output data: none
    //
    // This command is used to abort the retry of the current NAK. When CH376 works in the USB host mode, by
    // default, it will keep retrying until it returns success or error when receiving the NAK status returned by the
    // USB device. This command can force CH376 to terminate a retry in order to perform a new operation. In
    // addition, the command SET_RETRY can be used to set whether NAK retry is disabled.
    ABORT_NAK = 0x17,

    // 1CH - Set the receiver for USB host endpoint
    // Input data: Working mode
    // Output data: (Wait for 3uS)
    //
    // This command is used to set either the USB host endpoint or the receiver of the endpoint 2 (OUT of device
    // mode endpoint 2 / IN of host endpoint). This command needs to input 1 data to specify a new working mode.
    // For example, if an IN transaction is executed, DATA0 is expected to be received and DATA1 is given up, the
    // synchronous trigger flag of the receiver at the host endpoint must be set to 0 through this command. The byte
    // in the corresponding working mode byte is 80H. Typically, this command is completed within 3uS.
    SET_ENDP6 = 0x1C,

    // 1DH - Set the transmitter for USB host endpoint
    // Input data: Working mode
    // Output data: (Wait for 3uS)
    //
    // This command is used to set either the USB host endpoint or the transmitter of the endpoint 2 (IN of device
    // mode endpoint 2 / OUT of host endpoint). This command needs to input 1 data to specify a new working
    // mode. For example, if SETUP or OUT transaction is executed and DATA0 is expected to be transmitted, the
    // synchronous trigger flag of the transmitter at the host endpoint must be set to 0 through this command. The
    // corresponding working mode byte is 80H. If DATA1 is expected to be transmitted, the working mode byte
    // will be C0H. Typically, this command is completed within 3uS.
    SET_ENDP7 = 0x1D,

    // 22H - Get the interrupt status and cancel the interrupt request
    // Input data: none
    // Output data: Interrupt status
    //
    // This command is used to get the interrupt status of CH376 and notify CH376 to cancel the interrupt request.
    // After CH376 requests an interrupt from MCU, MCU gets the interrupt status through the command, analyzes
    // the interrupt cause and processes.
    GET_STATUS = 0x22,

    // 25H - Clear internal disk and file buffers
    // Input data: none
    // Output data: none
    DIRTY_BUFFER = 0x25,

    // 27H - Read the data block from the endpoint buffer of the current USB interrupt or from the receive buffer of the host endpoint
    // Input data: none
    // Output data: Data length, Data stream (n)
    RD_USB_DATA0 = 0x27,

    // 2CH - Write the data block to the transmit buffer of the USB host endpoint
    // Input data: Data Length, Data stream (n)
    // Output data: none
    WR_HOST_DATA = 0x2C,

    // 2DH - Write the requested data block to the internal specified buffer
    // Input data: None, Data stream (n)
    // Output data: Data Length
    // To-do: check if this is correct, it seems to be wrong. Data length is maybe first input?
    WR_REQ_DATA = 0x2D,

    // 2EH - Write the data block to the specified offset address of the internal buffer
    // Input data: Offset address, Data Length, Data stream (n)
    // Output data: none
    WR_OFS_DATA = 0x2E,

    // 2FH - Set the filename of the file to be operated
    // Input data: Character string (n)
    // Output data: none
    SET_FILE_NAME = 0x2F,

    // 30H - Check whether the disk is connected
    // Input data: none
    // Output data: Generate interrupt
    DISK_CONNECT = 0x30,

    // 31H - Initialize the disk and test whether the disk is ready
    // Input data: none
    // Output data: Generate interrupt
    DISK_MOUNT = 0x31,

    // 32H - Open files or directories, enumerate files and directories
    // Input data: none
    // Output data: Generate interrupt
    FILE_OPEN = 0x32,

    // 33H - Continue enumerating files and directories
    // Input data: none
    // Output data: Generate interrupt
    FILE_ENUM_GO = 0x33,

    // 34H - New File
    // Input data: none
    // Output data: Generate interrupt
    FILE_CREATE = 0x34,

    // 35H - Delete File
    // Input data: none
    // Output data: Generate interrupt
    FILE_ERASE = 0x35,

    // 36H - Close the currently opened file or directory
    // Input data: Whether update is allowed
    // Output data: Generate interrupt
    FILE_CLOSE = 0x36,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DIR_INFO_READ = 0x37,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DIR_INF0_SAVE = 0x38,

    //  -
    // Input data:
    // Output data: Generate interrupt
    BYTE_LOCATE = 0x39,

    //  -
    // Input data:
    // Output data: Generate interrupt
    BYTE_READ = 0x3A,

    //  -
    // Input data:
    // Output data: Generate interrupt
    BYTE_RD_GO = 0x3B,

    //  -
    // Input data:
    // Output data: Generate interrupt
    BYTE_WRITE = 0x3C,

    //  -
    // Input data:
    // Output data: Generate interrupt
    BYTE_WR_GO = 0x3D,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_CAPACITY = 0x3E,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_QUERY = 0x3F,

    // 40H - Create a new directory and open it or open an existing directory
    // Input data: none
    // Output data: Generate interrupt
    DIR_CREATE = 0x40,

    //  -
    // Input data:
    // Output data: Generate interrupt
    CLR_STALL = 0x41,

    //  -
    // Input data:
    // Output data: Generate interrupt
    SET_ADDRESS = 0x45,

    //  -
    // Input data:
    // Output data: Generate interrupt
    GET_DESCR = 0x46,

    //  -
    // Input data:
    // Output data: Generate interrupt
    SET_CONFIG = 0x49,

    //  -
    // Input data:
    // Output data: Generate interrupt
    SEG_LOCATE = 0x4A,

    //  -
    // Input data:
    // Output data: Generate interrupt
    SEC_READ = 0x4B,

    //  -
    // Input data:
    // Output data: Generate interrupt
    SEC_WRITE = 0x4C,

    //  -
    // Input data:
    // Output data: Generate interrupt
    AUTO_SETUP = 0x4D,

    //  -
    // Input data:
    // Output data: Generate interrupt
    ISSUE_TKN_X = 0x4E,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_BOC_CMD = 0x50,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_INIT = 0x51,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_RESET = 0x52,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_SIZE = 0x53,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_READ = 0x54,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_RD_GO = 0x55,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_WRITE = 0x56,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_WR_GO = 0x57,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_INQUIRY = 0x58,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_READY = 0x59,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_R_SENSE = 0x5A,

    //  -
    // Input data:
    // Output data: Generate interrupt
    RD_DISK_SEC = 0x5B,

    //  -
    // Input data:
    // Output data: Generate interrupt
    WR_DISK_SEC = 0x5C,

    //  -
    // Input data:
    // Output data: Generate interrupt
    DISK_MAX_LUN = 0x5D,
} CH376_Command;

void DelayMilliseconds(uint32_t milliseconds);

uint8_t CH376_ReadCommand();
uint8_t CH376_ReadData();
void CH376_WriteCommand(CH376_Command cmd);
void CH376_WriteData(uint8_t param);
bool CH376_CheckIfExists();

uint8_t CH376_GetLastInt();
bool CH376_WaitInt(uint32_t retry_count);

// GET_IC_VER
uint8_t CH376_GetVersion();
// SET_BAUDRATE
uint8_t CH376_SetBaudRate(uint8_t coefficient, uint8_t divisor);
// ENTER_SLEEP
void CH376_EnterSleep();
// SET_USB_SPEED
void CH376_SetUSBSpeed(uint8_t speed);
// RESET_ALL
void CH376_ResetAll();
// CHECK_EXIST
uint8_t CH376_CheckExists(uint8_t data);
// GET_DEV_RATE
uint8_t CH376_GetDevRate();
// 0AH READ_VAR8
uint8_t CH376_ReadVar8(uint8_t address);
// 0BH SET_SD0_INT
void CH376_SetSD0Int(uint8_t enable);
// 0BH SET_RETRY
void CH376_SetRetry(uint8_t retry);

// 22H GET_STATUS
uint8_t CH376_GetStatus();

// SET_USB_MODE
bool CH376_SetUSBMode(CH376_USB_Mode mode);
uint8_t CH376_ReadUSBData(uint8_t *buffer);
void CH376_SetFileName(uint8_t *name, uint32_t length);
bool CH376_MountDisk();
bool CH376_FileOpen();
bool CH376_FileEnumGo();
bool CH376_FileClose();
bool CH376_CreateDirectory();

uint32_t CH376_DiskCapacity();
DiskInfo CH376_QueryDisk();
bool CH376_CreateDirectory();

#endif // _WCH376S_H_