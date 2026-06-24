/** @file
  Pch access library for GSPI controller.

  This library is provided as an example for FuSa proof of concept
  communication, so only SPI write is implemented.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PchGspiLibPrivate.h"

#define PCH_GSPI_READY_TIMEOUT_US  1000000

static CONST UINTN mPchGspiCtrls[] =
{
  PCI_SEGMENT_LIB_ADDRESS (
    DEFAULT_PCI_SEGMENT_NUMBER_PCH,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI0,
    PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI0,
    0
  ),
  PCI_SEGMENT_LIB_ADDRESS (
    DEFAULT_PCI_SEGMENT_NUMBER_PCH,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI1,
    PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI1,
    0
  ),
  PCI_SEGMENT_LIB_ADDRESS (
    DEFAULT_PCI_SEGMENT_NUMBER_PCH,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI2,
    PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI2,
    0
  )
};

static CONST UINTN mPchGspiBase[] =
{
  PCH_SERIAL_IO_BASE_ADDRESS + 0xC0000,
  PCH_SERIAL_IO_BASE_ADDRESS + 0xE0000,
  PCH_SERIAL_IO_BASE_ADDRESS + 0x110000
};

/**
  Get base address of SPI controller instance

  @param Controller
  @return Spi Controller Base Address
 */
UINT64
EFIAPI
GetGspiBaseAddress (
  UINT8 Controller
)
{
  UINT64 Bar0;

  if (Controller >= ARRAY_SIZE (mPchGspiCtrls)) {
    ASSERT (FALSE);
    return 0;
  }

  Bar0 = PciSegmentRead32 (mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET);
  // Check for 64-bit memory BAR (bit0 == 0 and bits[2:1] == 10b)
  if (((Bar0 & BIT0) == 0) && ((Bar0 & (BIT2 | BIT1)) == BIT2)) {
    Bar0 += LShiftU64 (PciSegmentRead32 (mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET + 4), 32);
  }
  Bar0 &= ~0xF;
  return Bar0;
}

/**
  Initialize selected GSPI controller instance with default settings.

  @param Controller
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
PchGspiInit (
  UINT8 Controller
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN         Address;
  UINT32        Data32;
  UINT16        Cmd;
  UINT8         CapId, CapOffset;
  EFI_PCI_PMCSR Pmcsr;

  if (Controller >= ARRAY_SIZE (mPchGspiCtrls)) {
    return EFI_INVALID_PARAMETER;
  }

  // Make sure PCI device is visible.
  if (PciSegmentRead32 (mPchGspiCtrls[Controller] + PCI_VENDOR_ID_OFFSET) == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }

  Address = (UINTN)GetGspiBaseAddress (Controller);
  DEBUG ((DEBUG_INFO, "GSPI controller %d Address %p\n", Controller, (VOID *)Address));
  // is BAR programmed already
  if (Address == 0) {
    Address = mPchGspiBase[Controller];
    PciSegmentWrite32 (mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET, (UINT32)Address);
  }
  // Enable memory space decoding
  Cmd = PciSegmentRead16 (mPchGspiCtrls[Controller] + PCI_COMMAND_OFFSET);
  if (!(Cmd & BIT1)) {
    Cmd |= BIT1;
    PciSegmentWrite16 (mPchGspiCtrls[Controller] + PCI_COMMAND_OFFSET, Cmd);
  }

  Address = (UINTN)GetGspiBaseAddress (Controller);
  DEBUG ((DEBUG_INFO, "GSPI controller %d Address %p\n", Controller, (VOID *)Address));
  // Make sure device is in D0 power state.
  // Find PM Capabilities
  CapOffset = PciSegmentRead8 (mPchGspiCtrls[Controller] + PCI_CAPBILITY_POINTER_OFFSET);
  while (CapOffset != 0) {
    CapId = PciSegmentRead8 (mPchGspiCtrls[Controller] + CapOffset);
    if (CapId == EFI_PCI_CAPABILITY_ID_PMI) {
      Pmcsr.Data = PciSegmentRead16 (mPchGspiCtrls[Controller] + CapOffset + 4);
      if (Pmcsr.Bits.PowerState != PCI_POWER_STATE_D0) {
        Pmcsr.Bits.PowerState = PCI_POWER_STATE_D0;
        PciSegmentWrite16 (mPchGspiCtrls[Controller] + CapOffset + 4, Pmcsr.Data);
      }
      break;
    }
    CapOffset = PciSegmentRead8 (mPchGspiCtrls[Controller] + CapOffset + 1);
  };

  // DMA reset
  MmioWrite32 (Address + PCH_GSPI_RESETS, 0);
  MmioWrite32 (Address + PCH_GSPI_RESETS, 7);

  // Mask interrupts, select on-chip clock, set data size to 8 bits
  // set T/R mode, disable DMA, set clock phase, SCR = 196 -> 500khz
  MmioWrite32 (Address + PCH_GSPI_SSCR0, (196 << 8) | 7);
  MmioWrite32 (Address + PCH_GSPI_SSCR1, 0);

  // Clear WMRF
  MmioAnd32 (Address + PCH_GSPI_SIRF, (UINT32)~PCH_GSPI_SIRF_WMRF_MASK);

  // Set low and high water mark transmit FIFO to 2 entry
  MmioAndThenOr32 (Address + PCH_GSPI_SITF, (UINT32)~PCH_GSPI_SITF_LWMTF_MASK, 1 << PCH_GSPI_SITF_LWMTF_SHIFT);
  MmioAndThenOr32 (Address + PCH_GSPI_SITF, (UINT32)~PCH_GSPI_SITF_HWMTF_MASK, 1);

  // Disable SPI operation
  MmioAnd32 (Address + PCH_GSPI_SSCR0, (UINT32)~PCH_GSPI_SSCR0_SSE);

  // Configure SPI Speed: M=N=1. CLK_EN=1, CLK_UPDATE=0
  MmioWrite32 (Address + PCH_GSPI_CLOCKS, BIT16 | BIT1 | BIT0);
  // Update Clock
  MmioOr32 (Address + PCH_GSPI_CLOCKS, BIT31);
  // set clock divider to 1. disable clock output for now. Set SPI mode.
  MmioWrite32 (Address + PCH_GSPI_SSCR1, BIT31 | BIT16 | BIT4 | BIT3 | BIT1);

  // set HW CS mode, CS0, CS polarity low
  MmioAndThenOr32 (Address + PCH_GSPI_CS_CTL, (UINT32)~(BIT12 | BIT13 | BIT9 | BIT8 | BIT0), BIT1);

  // Set SPI transfer mode to Transmit/Receive mode
  MmioAnd32 (Address + PCH_GSPI_SSCR1, (UINT32)~BIT23);

  Data32 = MmioRead32 (Address + PCH_GSPI_SSSR);
  DEBUG ((DEBUG_INFO, "GSPI %d Status Register 0x%X\n", Controller, Data32));
  return Status;

}

/**
  This function is used to wait for GSPI controller to be ready for next operation.
  It checks the SITFL and SSSR registers to ensure that the Tx FIFO is empty and the controller is not busy.
  If the controller is busy, it will loop until it becomes ready.

  @param BaseAddress - GSPI controller base address

 */
EFI_STATUS
WaitForGspiReady (
  UINTN   BaseAddress
)
{
  UINT32  Sitfl;
  UINT32  Sssr;
  UINT64  StartTimeNs;
  UINT64  CurrentTimeNs;

  StartTimeNs = GetTimeInNanoSecond (GetPerformanceCounter ());

  while (TRUE) {
    Sitfl = (MmioRead32 (BaseAddress + PCH_GSPI_SITF) & PCH_GSPI_SITF_SITFL_MASK) >> PCH_GSPI_SITF_SITFL_SHIFT;
    DEBUG ((DEBUG_VERBOSE, "SITFL: 0x%X", Sitfl));
    Sssr = MmioRead32 (BaseAddress + PCH_GSPI_SSSR);
    DEBUG ((DEBUG_VERBOSE, "\tSSSR: 0x%04X\n", Sssr));

    if ((Sitfl == 0) && ((Sssr & PCH_GSPI_SSSR_BSY) == 0)) {
      return EFI_SUCCESS;
    }

    CurrentTimeNs = GetTimeInNanoSecond (GetPerformanceCounter ());
    if ((CurrentTimeNs - StartTimeNs) > (PCH_GSPI_READY_TIMEOUT_US * 1000ULL)) {
      DEBUG ((DEBUG_ERROR, "GSPI wait ready timeout: SITFL=0x%X SSSR=0x%X\n", Sitfl, Sssr));
      return EFI_TIMEOUT;
    }

    MicroSecondDelay (1);
  }
}

/**
  Wait until GSPI TX FIFO is not full.

  @param BaseAddress - GSPI controller base address

  @retval EFI_SUCCESS     TX FIFO can accept data.
  @retval EFI_TIMEOUT     Timed out waiting for TX FIFO space.
**/
EFI_STATUS
WaitForTxNotFull (
  UINTN   BaseAddress
)
{
  UINT32  Sssr;
  UINT64  StartTimeNs;
  UINT64  CurrentTimeNs;

  StartTimeNs = GetTimeInNanoSecond (GetPerformanceCounter ());

  while (TRUE) {
    Sssr = MmioRead32 (BaseAddress + PCH_GSPI_SSSR);
    if ((Sssr & PCH_GSPI_SSSR_TNF) != 0) {
      return EFI_SUCCESS;
    }

    CurrentTimeNs = GetTimeInNanoSecond (GetPerformanceCounter ());
    if ((CurrentTimeNs - StartTimeNs) > (PCH_GSPI_READY_TIMEOUT_US * 1000ULL)) {
      DEBUG ((DEBUG_ERROR, "GSPI wait TX not full timeout: SSSR=0x%X\n", Sssr));
      return EFI_TIMEOUT;
    }

    MicroSecondDelay (1);
  }
}

/**
  Write Buffer to SPI controller indicated. Configure HW
  chip select setting first.

  @param Controller
  @param ChipSelect
  @param Data
  @param DataSize
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
PchGspiWrite (
  UINT8 Controller,
  UINT8 ChipSelect,
  UINT8 *Data,
  UINTN DataSize
)
{
  EFI_STATUS Status;
  UINTN   Addr;
  UINT32  Data32;
  UINTN   DataIndex;
  UINTN   RemainingSize;
  UINT8   *CurrentData;


  if ((Controller >= ARRAY_SIZE (mPchGspiCtrls)) || ChipSelect > 1) {
    return EFI_INVALID_PARAMETER;
  }
  Addr = (UINTN)GetGspiBaseAddress (Controller);

  if (DataSize == 0) {
    return EFI_SUCCESS;
  }

  if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RemainingSize = DataSize;
  CurrentData = Data;

  // Send block size at a time, otherwise GSPI controller stops sending.
  while (RemainingSize != 0) {
    // Enable SPI operation
    MmioOr32 (Addr + PCH_GSPI_SSCR0, PCH_GSPI_SSCR0_SSE);

    Data32 = MmioRead32 (Addr + PCH_GSPI_CS_CTL);
    Data32 = (Data32 & ~(BIT8 | BIT9)) | (ChipSelect << 8);
    MmioWrite32 (Addr + PCH_GSPI_CS_CTL, Data32);

    for (DataIndex = 0;
         DataIndex < (RemainingSize < PCH_GSPI_BLOCK_SIZE ? RemainingSize : PCH_GSPI_BLOCK_SIZE);
         DataIndex++) {
      Status = WaitForTxNotFull (Addr);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      DEBUG ((DEBUG_VERBOSE, "Channel %d TX FIFO ready", Controller));
      MmioWrite32 (Addr + PCH_GSPI_SSDR, CurrentData[DataIndex]);
      DEBUG ((DEBUG_VERBOSE, "\tWrote GSPI Data byte index 0x%X data %02x to FIFO\n", DataIndex, CurrentData[DataIndex]));
    }

    CurrentData += (UINTN)(RemainingSize < PCH_GSPI_BLOCK_SIZE ? RemainingSize : PCH_GSPI_BLOCK_SIZE);
    RemainingSize -= (RemainingSize < PCH_GSPI_BLOCK_SIZE ? RemainingSize : PCH_GSPI_BLOCK_SIZE);

    Status = WaitForGspiReady (Addr);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    // Disable SPI operation
    MmioAnd32 (Addr + PCH_GSPI_SSCR0, (UINT32)~PCH_GSPI_SSCR0_SSE);
  }
  return EFI_SUCCESS;
}
