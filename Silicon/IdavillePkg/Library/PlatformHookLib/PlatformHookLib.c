/** @file
  The platform hook library.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <PchReservedResources.h>
#include <Register/PchRegs.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsHsuart.h>
#include <IndustryStandard/Pci.h>

#define SERIAL_REGISTER_BASE      0x3F8

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINTN   BaseAddr;

  // LPC I/O Configuration
  // Remove COM0 and COM1 from LPC as we are using HSUART for debug messages
  //
  PchLpcIoDecodeRangesSet (
    (V_LPC_CFG_IOD_LPT_378  << N_LPC_CFG_IOD_LPT)
  );

  PchLpcIoEnableDecodingSet (
    B_LPC_CFG_IOE_ME2  |
    B_LPC_CFG_IOE_SE   |
    B_LPC_CFG_IOE_ME1  |
    B_LPC_CFG_IOE_KE   |
    B_LPC_CFG_IOE_HGE  |
    B_LPC_CFG_IOE_LGE  |
    B_LPC_CFG_IOE_FDE  |
    B_LPC_CFG_IOE_PPE
  );

  BaseAddr = MM_PCI_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_HSUART,
    PCI_FUNCTION_NUMBER_PCH_HSUART0,
    0);

  MmioAnd16 (BaseAddr + PCI_COMMAND_OFFSET, (UINT16)~(EFI_PCI_COMMAND_IO_SPACE |
    EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));
  MmioWrite32 (BaseAddr + PCI_BASE_ADDRESSREG_OFFSET, SERIAL_REGISTER_BASE);
  MmioWrite16 (BaseAddr + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_BUS_MASTER));

  return RETURN_SUCCESS;
}

/**
  Get serial port stride register size.

  @retval  The serial port register stride size.

**/
UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
)
{
  return 1;
}

/**
  Get serial port register base address.

  @retval  The serial port register base address.

**/
UINT64
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  UINTN   BaseAddr;
  UINT64  Bar;

  BaseAddr = MM_PCI_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_HSUART,
      PCI_FUNCTION_NUMBER_PCH_HSUART0,
      0);

  Bar = (UINT64)(MmioRead32 (BaseAddr + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF8);
  return Bar;
}
