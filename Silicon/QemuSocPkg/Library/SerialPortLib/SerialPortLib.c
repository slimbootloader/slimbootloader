/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci.h>
#include <RegAccess.h>

//---------------------------------------------
// UART Register Offsets
//---------------------------------------------
#define STRIDE_SIZE             0x01
#define BAUD_LOW_OFFSET         (STRIDE_SIZE * 0x00)
#define BAUD_HIGH_OFFSET        (STRIDE_SIZE * 0x01)
#define IER_OFFSET              (STRIDE_SIZE * 0x01)
#define LCR_SHADOW_OFFSET       (STRIDE_SIZE * 0x01)
#define FCR_SHADOW_OFFSET       (STRIDE_SIZE * 0x02)
#define IR_CONTROL_OFFSET       (STRIDE_SIZE * 0x02)
#define FCR_OFFSET              (STRIDE_SIZE * 0x02)
#define EIR_OFFSET              (STRIDE_SIZE * 0x02)
#define BSR_OFFSET              (STRIDE_SIZE * 0x03)
#define LCR_OFFSET              (STRIDE_SIZE * 0x03)
#define MCR_OFFSET              (STRIDE_SIZE * 0x04)
#define LSR_OFFSET              (STRIDE_SIZE * 0x05)
#define MSR_OFFSET              (STRIDE_SIZE * 0x06)
#define SCR_OFFSET              (STRIDE_SIZE * 0x07)

//---------------------------------------------
// UART Register Bit Defines
//---------------------------------------------
#define LSR_TXRDY               0x20
#define LSR_RXDA                0x01
#define DLAB                    0x01
#define UART_MAGIC              0x55

#define GetPciUartBase          GetSerialPortBase

UINTN   gBps      = 115200;
UINT8   gData     = 8;
UINT8   gStop     = 1;
UINT8   gParity   = 0;
UINT8   gBreakSet = 0;

UINT8
GetSerialPortStrideSize (
  VOID
)
{
  return 1;
}

UINT32
GetPciUartBase (
  VOID
  )
{
  return 0x3F8;
}

UINT8
SerialPortReadRegister (
  IN UINTN    Offset
  )
{
  UINT8              Data;
  UINT32             Base;
  
  Base = GetPciUartBase();
  Data = IoRead8 (Base + Offset);
  
  return Data;
}

UINT8
SerialPortWriteRegister (
  IN UINTN    Offset,
  IN UINT8    Value
  )
{
  UINT32 Base;

  Base = GetPciUartBase();
  return IoWrite8(Base + Offset, Value);  
}

RETURN_STATUS
EFIAPI
UARTInitialize (
  VOID
  )
{
  UINTN  Divisor;
  UINT8  OutputData;
  // UINT8  Data;

  if (SerialPortReadRegister (SCR_OFFSET) != UART_MAGIC) {
    //
    // Calculate divisor for baud generator
    //
    Divisor = 115200 / gBps;

    //
    // Set communications format
    //
    OutputData = (UINT8) (0x83);
    SerialPortWriteRegister (LCR_OFFSET, OutputData);

    //
    // Configure baud rate
    //
    SerialPortWriteRegister (BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
    SerialPortWriteRegister (BAUD_LOW_OFFSET,  (UINT8) (Divisor & 0xff));

    //
    // Switch back to bank 0
    //
    OutputData = (UINT8) (0x03);
    SerialPortWriteRegister (LCR_OFFSET, OutputData);

    SerialPortWriteRegister (IR_CONTROL_OFFSET, 0xc1);

    SerialPortWriteRegister (SCR_OFFSET, UART_MAGIC);
  }

  return RETURN_SUCCESS;
}

/**
  Initialize the serial device hardware.
  
  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.
  
  @retval EFI_SUCCESS        The serial device was initialized.
  @retval EFI_DEVICE_ERROR   The serial device could not be initialized.
**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{  
  return UARTInitialize();
}

/**
  Write data to serial device.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes writed to serial device.
**/
UINTN
EFIAPI
UARTDbgOut (
  IN UINT8    *Buffer,
  IN UINTN    NumberOfBytes
  )
{
  UINTN  Result;
  UINT8  Data;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  while (NumberOfBytes--) {
    //
    // Wait for the serail port to be ready.
    //
    do {
      Data = SerialPortReadRegister (LSR_OFFSET);
    } while ((Data & LSR_TXRDY) == 0);
    SerialPortWriteRegister (0, *Buffer++);
  }

  return Result;
}

/**
  Write data from buffer to serial device. 

  Writes NumberOfBytes data bytes from Buffer to the serial device.  
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT(). 

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.  
                           If this value is less than NumberOfBytes, then the read operation failed.
**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8    *Buffer,
  IN UINTN    NumberOfBytes
  )
{
  return UARTDbgOut (Buffer, NumberOfBytes);
}

UINTN
EFIAPI
UARTDbgIn (
  OUT UINT8    *Buffer,
  IN  UINTN    NumberOfBytes
  )
{
  UINTN Result;
  UINT8 Data;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  while (NumberOfBytes--) {
    //
    // Wait for the serial port to be ready.
    //
    do {
      Data = SerialPortReadRegister (LSR_OFFSET);
    } while ((Data & LSR_RXDA) == 0);

    *Buffer++ = SerialPortReadRegister (0);
  }

  return Result;
}

/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.  
                           If this value is less than NumberOfBytes, then the read operation failed.
**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8 *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  return UARTDbgIn (Buffer, NumberOfBytes);
}

BOOLEAN
EFIAPI
UARTDbgPoll (
  VOID
  )
{
  //
  // Read the serial port status
  //
  if ((SerialPortReadRegister (LSR_OFFSET) & LSR_RXDA) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.
**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
  BOOLEAN Status;

  Status = FALSE;
  Status |= UARTDbgPoll ();

  return Status;
}

