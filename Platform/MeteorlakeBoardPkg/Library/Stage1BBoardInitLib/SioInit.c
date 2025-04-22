/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/IoLib.h>
#include <Include/PlatformBoardId.h>
#include <Include/Library/PciSegmentLib.h>
#include <Include/Library/PchPciBdfLib.h>
#include <Include/Library/DebugLib.h>
#include "uart16550.h"

#define SIO_RUNTIME_REG_BASE_ADDRESS      0x0680
#define SIO_CONFIG_PORT                   0x4E
#define SIO_DATA_PORT                     0x4F
#define SIO_HWMON_BASE_ADDRESS            0x0290

typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_TABLE;


GLOBAL_REMOVE_IF_UNREFERENCED EFI_SIO_TABLE mSioIt8659TableSerialPort[] = {
  {0x007, 0x01}, // Com1 Logical Device Number select
  {0x061, 0xF8}, // Serial Port 1 Base Address LSB Register
  {0x060, 0x03}, // Serial Port 1 Base Address MSB Register
  {0x0F0, 0x01}, // Change to Shared IRQ
  {0x030, 0x01}, // Serial Port 1 activate
};


/**
  Initialize 16550Uart.

**/
VOID
Init16550Uart(
    VOID
)
{
  UINT32  Divisor;
  UINT8   Data;

  //
  // Set Line Control Register (LCR)
  //
  IoWrite8(GLOBALCOMPORT + LINE_CONTROL, (DLAB_BIT << 7) | (STOP_BIT << 2) | DATA_BITS);

  //
  // Set Baud rate to 115k in Divisor Latch
  //
  Divisor = (115200 / BAUD_RATE_DIVISOR);
  IoWrite8(GLOBALCOMPORT + DIVISOR_LATCH_LOW, (((UINT8)Divisor) & 0xFF));
  IoWrite8(GLOBALCOMPORT + DIVISOR_LATCH_HIGH, ((UINT8)(Divisor >> 8)) & 0xFF);

  //
  // Clear DLAB bit in LCR
  //
  Data = IoRead8(GLOBALCOMPORT + LINE_CONTROL);
  DEBUG((DEBUG_INFO, "Uart1 - LCR %x\n", Data));
  Data &= ~(DLAB_BIT << 7);
  IoWrite8(GLOBALCOMPORT + LINE_CONTROL, Data);

  return;
}

/**
  Initialize Sio.

**/

VOID
MtlEclessSioInit(
    VOID
)
{
  //
  // Program and Enable Default Super IO Configuration Port Addresses and range
  //
  UINT64      LpcBaseAddr;
  UINT16      LpcIoDecodeRanges;
  UINT8       Index;

  DEBUG((DEBUG_INFO, "MtlEclessSioInit - Entry\n"));
  // Enable LPC IO decode for LPC/eSPI Bridge communication
  // Enable 4e/4f Decode over eSPI CS1#.
  DEBUG((DEBUG_INFO, "MtlEclessSioInit - Enable 4E 4F\n"));
  LpcBaseAddr = LpcPciCfgBase();
  LpcIoDecodeRanges = PciSegmentRead16(LpcBaseAddr + (0x82));
  LpcIoDecodeRanges |= ((0x00002000) | (0x00000001) | (0x00000002));
  DEBUG((DEBUG_INFO, "MtlEclessSioInit - LpcIoDecodeRanges: %x\n", LpcIoDecodeRanges));
  PciSegmentWrite16((UINTN)(LpcBaseAddr + (0x82)), (UINT16)LpcIoDecodeRanges);

  //Enter Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x87);
  IoWrite8(SIO_CONFIG_PORT, 0x01);
  IoWrite8(SIO_CONFIG_PORT, 0x55);
  IoWrite8(SIO_CONFIG_PORT, 0xAA);
  IoWrite8(SIO_CONFIG_PORT, 0x20);

  if ((IoRead8(SIO_DATA_PORT) & 0xFF) == 0x86)
  {
    IoWrite8(SIO_CONFIG_PORT, 0x21);
    if ((IoRead8(SIO_DATA_PORT) & 0xFF) != 0x59)
    {
      DEBUG((DEBUG_INFO, "MtlEclessSioInit - Return CHIPID2:Not ITE8659\n"));
      return;
    }
  }
  else
  {
    DEBUG((DEBUG_INFO, "MtlEclessSioInit - Return CHIPID1:Not ITE8659\n"));
    return;
  }

  for (Index = 0; Index < sizeof(mSioIt8659TableSerialPort) / sizeof(EFI_SIO_TABLE); Index++) {
    DEBUG((DEBUG_INFO, "MtlEclessSioInit - Reg:%x Val:%x\n", mSioIt8659TableSerialPort[Index].Register, mSioIt8659TableSerialPort[Index].Value));
    IoWrite8(SIO_CONFIG_PORT, mSioIt8659TableSerialPort[Index].Register);
    IoWrite8(SIO_DATA_PORT, mSioIt8659TableSerialPort[Index].Value);
  }

  LpcIoDecodeRanges = PciSegmentRead16(LpcBaseAddr + (0x80));
  LpcIoDecodeRanges = LpcIoDecodeRanges & 0xff00;
  LpcIoDecodeRanges |= ((7 << 4) | 0);
  PciSegmentWrite16((UINTN)(LpcBaseAddr + (0x80)), (UINT16)LpcIoDecodeRanges);
  LpcIoDecodeRanges = PciSegmentRead16(LpcBaseAddr + (0x80));
  DEBUG((DEBUG_INFO, "MtlEclessSioInit - R_LPC_CFG_IOD LpcIoDecodeRanges: %x\n", LpcIoDecodeRanges));

  Init16550Uart();
  DEBUG((DEBUG_INFO, "MtlEclessSioInit - UARTINIT Done\n"));

  //Exit Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x02);
  IoWrite8(SIO_DATA_PORT, 0x02);

  return;
}
