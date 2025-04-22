/** @file
  Pch access library for GSPI controller.

  This library is provided as an example for FuSa proof of concept
  communication, so only SPI write is implemented.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PchGspiLibPrivate.h"

static CONST UINTN mPchGspiCtrls[] =
{
  PCI_SEGMENT_LIB_ADDRESS(
    DEFAULT_PCI_SEGMENT_NUMBER_PCH,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI0,
    PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI0,
    0
  ),
  PCI_SEGMENT_LIB_ADDRESS(
    DEFAULT_PCI_SEGMENT_NUMBER_PCH,
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI1,
    PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI1,
    0
  ),
  PCI_SEGMENT_LIB_ADDRESS(
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

  @param Controler
  @return Spi Controller Base Address
 */
UINT64
EFIAPI
GetGspiBaseAddress (
  UINT8 Controller
)
{
  UINT64 Bar0;
  Bar0 = PciRead32(mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET);
  // check for 64bit register
  if ((Bar0 & (BIT3 | BIT2)) == BIT3) {
    Bar0 += LShiftU64(PciRead32(mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET + 4),32);
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
PchGspiInit(
  UINT8 Controller
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN         Address;
  UINT32        Data32;
  UINT16        Cmd;
  UINT8         CapId, CapOffset;
  EFI_PCI_PMCSR Pmcsr;

  if (Controller > sizeof(mPchGspiCtrls)/sizeof(UINT64)) {
    return EFI_INVALID_PARAMETER;
  }

  // Make sure PCI device is visible.
  if (PciRead32(mPchGspiCtrls[Controller] + PCI_VENDOR_ID_OFFSET) == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }

  Address = (UINTN)GetGspiBaseAddress(Controller);
  DEBUG((DEBUG_INFO, "GSPI controller %d Address 0x%X\n", Controller, Address));
  // is BAR programmed already
  if (Address == 0) {
    Address = mPchGspiBase[Controller];
    PciWrite32(mPchGspiCtrls[Controller] + PCI_BASE_ADDRESSREG_OFFSET, (UINT32)Address);
  }
  // Enable memory space decoding
  Cmd = PciRead16(mPchGspiCtrls[Controller] + PCI_COMMAND_OFFSET);
  if (!(Cmd & BIT1)) {
    Cmd |= BIT1;
    PciWrite16(mPchGspiCtrls[Controller] + PCI_COMMAND_OFFSET, Cmd);
  }

  Address = (UINTN)GetGspiBaseAddress(Controller);
  DEBUG((DEBUG_INFO, "GSPI controller %d Address 0x%X\n", Controller, Address));
  // Make sure device is in D0 power state.
  // Find PM Capabilities
  CapOffset = PciRead8(mPchGspiCtrls[Controller]+PCI_CAPBILITY_POINTER_OFFSET);
  do {
    CapId = PciRead8(mPchGspiCtrls[Controller]+CapOffset);
      if (CapId == EFI_PCI_CAPABILITY_ID_PMI) {
        Pmcsr.Data = PciRead16(mPchGspiCtrls[Controller]+CapOffset+4);
        if (Pmcsr.Bits.PowerState != PCI_POWER_STATE_D0) {
          Pmcsr.Bits.PowerState  = PCI_POWER_STATE_D0;
          PciWrite16(mPchGspiCtrls[Controller]+CapOffset+4, Pmcsr.Data);
        }
        break;
      }
    CapOffset = PciRead8(CapOffset+1);
  } while (CapOffset);

  // DMA reset
  MmioWrite32(Address + PCH_GSPI_RESETS, 0);
  MmioWrite32(Address + PCH_GSPI_RESETS, 7);

  // Mask interrupts, select on-chip clock, set data size to 8 bits
  // set T/R mode, disable DMA, set clock phase, SCR = 196 -> 500khz
  MmioWrite32(Address + PCH_GSPI_SSCR0, (196 << 8) | 7);
  MmioWrite32(Address + PCH_GSPI_SSCR1, 0);

  // Clear WMRF
  MmioAnd32(Address + PCH_GSPI_SIRF, (UINT32)~PCH_GSPI_SIRF_WMRF_MASK);

  // Set low and high water mark transmit FIFO to 2 entry
  MmioAndThenOr32(Address + PCH_GSPI_SIRF, (UINT32)~PCH_GSPI_SIRF_LWMTF_MASK, 1<<PCH_GSPI_SIRF_LWMTF_SHIFT);
  MmioAndThenOr32(Address + PCH_GSPI_SIRF, (UINT32)~PCH_GSPI_SIRF_HWMTF_MASK, 1);

  // Disable SPI operation
  MmioAnd32(Address + PCH_GSPI_SSCR0, (UINT32)~PCH_GSPI_SSCR0_SSE);

  // Configure SPI Speed: M=N=1. CLK_EN=1, CLK_UPDATE=0
  MmioWrite32(Address + PCH_GSPI_CLOCKS, BIT16 | BIT1 | BIT0);
  // Update Clock
  MmioOr32(Address + PCH_GSPI_CLOCKS, BIT31);
  // set clock divider to 1. disable clock output for now. Set SPI mode.
  MmioWrite32(Address + PCH_GSPI_SSCR1, BIT31 | BIT16 | BIT4 | BIT3 | BIT1);

  // set HW CS mode, CS0, CS polarity low
  MmioAndThenOr32(Address + PCH_GSPI_CS_CTL, (UINT32)~(BIT12 | BIT13 | BIT9 | BIT8 | BIT0), BIT1);

  // Set SPI transfer mode to Transmit/Receive mode
  MmioAnd32(Address + PCH_GSPI_SSCR1, (UINT32)~BIT23);

  Data32 = MmioRead32(Address + PCH_GSPI_SSSR);
  DEBUG((DEBUG_INFO, "GSPI %d Status Register 0x%X\n", Controller, Data32));
  return Status;

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
PchGspiWrite(
  UINT8 Controller,
  UINT8 ChipSelect,
  UINT8 *Data,
  UINTN DataSize
)
{
  UINTN   Addr = (UINTN)GetGspiBaseAddress(Controller);
  UINT32  Data32;
  UINTN   DataIndex;
  UINT32  Sitfl;
  UINT32  Sssr;
  UINT32  RemainingSize;
  UINT8   *CurrentData;


  if ((Controller > sizeof(mPchGspiCtrls)/sizeof(UINT64)) || ChipSelect > 1) {
    return EFI_INVALID_PARAMETER;
  }
  if (Data == NULL || DataSize == 0) {
    return EFI_SUCCESS;
  }

  RemainingSize = DataSize;
  CurrentData = Data;

  // Send block size at a time, otherwise GSPI controller stops sending.
  while (RemainingSize != 0) {
    // Enable SPI operation
    MmioOr32(Addr + PCH_GSPI_SSCR0, PCH_GSPI_SSCR0_SSE);

    Data32 = MmioRead32(Addr + PCH_GSPI_CS_CTL);
    Data32 = (Data32 & ~(BIT8 | BIT9))  | (ChipSelect << 8);
    MmioWrite32(Addr + PCH_GSPI_CS_CTL, Data32);

    for (DataIndex = 0;
          DataIndex < (RemainingSize < PCH_GSPI_BLOCK_SIZE ? RemainingSize : PCH_GSPI_BLOCK_SIZE);
          DataIndex++)
    {
      // wait for Tx FIFO empty and controller not busy
      do {
        Sitfl = (MmioRead32(Addr + PCH_GSPI_SITF) & PCH_GSPI_SIRF_SITFL_MASK) >> PCH_GSPI_SIRF_SITFL_SHIFT;
        DEBUG((DEBUG_VERBOSE, "SITFL: 0x%X\n", Sitfl));
        Sssr = MmioRead32(Addr + PCH_GSPI_SSSR);
        DEBUG((DEBUG_VERBOSE, "SSSR: 0x%X\n", Sssr));
      } while ((Sitfl != 0) || (Sssr & PCH_GSPI_SSSR_BSY));
      DEBUG((DEBUG_VERBOSE, "\tChannel %d TX FIFO ready\n", Controller));
      MmioWrite32(Addr + PCH_GSPI_SSDR, CurrentData[DataIndex]);
      DEBUG((DEBUG_VERBOSE, "\tWrote GSPI Data byte index 0x%X to FIFO\n", DataIndex));
    }

    CurrentData += PCH_GSPI_BLOCK_SIZE;
    RemainingSize -= (RemainingSize < PCH_GSPI_BLOCK_SIZE ? RemainingSize : PCH_GSPI_BLOCK_SIZE);

    // Disable SPI operation
    MmioAnd32(Addr + PCH_GSPI_SSCR0, (UINT32)~PCH_GSPI_SSCR0_SSE);
  }
  return EFI_SUCCESS;
}
