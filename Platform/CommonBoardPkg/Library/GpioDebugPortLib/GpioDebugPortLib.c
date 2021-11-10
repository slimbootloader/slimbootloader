/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimeStampLib.h>

/**
  Set GPIO TX pin to required level.

**/
VOID
SetGpioTxPin (
  IN UINT32  Level
)
{
  UINT32  Data32;

  Data32 = MmioRead32 (PcdGet32 (PcdGpioDebugPortMmioBase));
  if (Level == 0) {
    Data32 &= (UINT32)(~(PcdGet32 (PcdGpioDebugPortMmioMask)));
  } else {
    Data32 |= PcdGet32 (PcdGpioDebugPortMmioMask);
  }
  MmioWrite32 (PcdGet32 (PcdGpioDebugPortMmioBase), Data32);
}

/**
  Send one byte through GPIO TX pin using bit bang.

**/
VOID
GpioDebugPortWriteByte (
  IN UINT8  Value
)
{
  UINT8   Idx;
  UINT32  Word;
  UINT32  Baud;
  UINT64  Freq;
  UINT64  Ts0;
  UINT64  Ts1;

  Baud = PcdGet32 (PcdGpioDebugPortBaudRate);
  Freq = GetTimeStampAccurateFrequency ();

  // Prepare 10 bits, 1 start bit, 8 data bits, 1 stop bits
  Word = (Value << 1) | BIT9;
  Ts0  = ReadTimeStamp ();
  for (Idx = 0; Idx < 10; Idx++) {
    SetGpioTxPin (Word & (1 << Idx));
    // Wait for 1 bit calculated by current baud rate
    Ts1 = Ts0 + DivU64x32 (MultU64x32(Freq, Idx + 1), Baud);
    while (ReadTimeStamp () < Ts1) {
      CpuPause ();
    }
  }

}


/**
  Write data from buffer to the console.

  Writes NumberOfBytes data bytes from Buffer to the virtual console.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to write to the console.

  @retval >0               The number of bytes written to the console.
**/
UINTN
EFIAPI
GpioDebugPortWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  )
{
  if (PcdGet32 (PcdGpioDebugPortMmioBase) != 0) {
    while (NumberOfBytes > 0) {
      GpioDebugPortWriteByte (*Buffer);
      Buffer++;
      NumberOfBytes--;
    }
  } else {
    NumberOfBytes = 0;
  }
  return NumberOfBytes;
}
