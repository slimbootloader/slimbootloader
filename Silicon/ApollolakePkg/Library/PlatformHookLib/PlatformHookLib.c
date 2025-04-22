/** @file
  The platform hook library.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <RegAccess.h>
#include <PlatformHookSupport.h>

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
  return 4;
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
  UINTN   PciUartMmBase;
  UINT16  Cmd16;
  UINT64  MmioBase;

  PciUartMmBase = MM_PCI_ADDRESS (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_LPSS_HSUART,
                    (PCI_FUNCTION_NUMBER_LPSS_HSUART0 + GetDebugPort()),
                    0
                    );
  Cmd16 = MmioRead16 (PciUartMmBase + R_LPSS_IO_STSCMD);
  if (Cmd16 == 0xFFFF) {
    //
    // Device might be hidden, assigned temp base address for it
    //
    return LPSS_UART_TEMP_BASE_ADDRESS + (2 * LPSS_UART_TMP_BAR_DELTA);
  } else {
    if (MmioRead8 (PciUartMmBase + R_LPSS_IO_STSCMD) & 0x02) {
      MmioBase  = LShiftU64 (MmioRead32 (PciUartMmBase + PCI_BASE_ADDRESSREG_OFFSET + 4), 32);
      MmioBase += (MmioRead32 (PciUartMmBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0);
      return MmioBase;
    } else {
      return 0;
    }
  }
}

/**
  Performs UART config space initialization

  @param[in]  Port             Specifies a UART port

  @retval RETURN_SUCCESS       The UART config space initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The UART config space initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
UartPortInitialize (
  IN  UINT8   Port
  )
{
  UINTN             PciUartMmBase;
  UINT32            PciBar;
  UINT32            Data32;

  //
  // Program the UART PCI resource
  //
  PciUartMmBase = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART0 + Port, 0);
  if (MmioRead8 (PciUartMmBase + R_LPSS_IO_STSCMD) & 0x02) {
    PciBar = MmioRead32 (PciUartMmBase + R_LPSS_IO_BAR) & 0xFFFFFFF0;
  } else {
    PciBar = LPSS_UART_TEMP_BASE_ADDRESS + (Port * LPSS_UART_TMP_BAR_DELTA);
    MmioWrite32 (PciUartMmBase + R_LPSS_IO_BAR,  PciBar);
    MmioWrite32 (PciUartMmBase + R_LPSS_IO_BAR + 4, 0);
    MmioWrite32 (PciUartMmBase + R_LPSS_IO_BAR1, PciBar + 0x1000);
    MmioWrite32 (PciUartMmBase + R_LPSS_IO_BAR1 + 4, 0);
    MmioWrite8  (PciUartMmBase + R_LPSS_IO_STSCMD, 0x06);
  }

  //
  // Bring UART out of reset
  //
  MmioWrite32 (PciBar + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_iDMA_RESET_REL | B_LPSS_IO_MEM_HC_RESET_REL);

  //
  // Set clock
  //
  Data32 = (V_FSP_LPSS_UART_PPR_CLK_N_DIV << 16) | (V_FSP_LPSS_UART_PPR_CLK_M_DIV << 1) | B_LPSS_IO_MEM_PCP_CLK_EN;
  MmioWrite32 (PciBar + R_LPSS_IO_MEM_PCP, Data32);
  MmioWrite32 (PciBar + R_LPSS_IO_MEM_PCP, Data32 | B_LPSS_IO_MEM_PCP_CLK_UPDATE);
  MmioRead32  (PciBar + R_LPSS_IO_MEM_PCP);

  return RETURN_SUCCESS;
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @param[in] none

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  IN  VOID
  )
{
  UINT8       Index;
  EFI_STATUS  Status;

  Index = GetDebugPort ();
  Status = UartGpioInitialize (Index);
  if (!EFI_ERROR(Status)) {
    Status = UartPortInitialize (Index);
  }
  return Status;
}
