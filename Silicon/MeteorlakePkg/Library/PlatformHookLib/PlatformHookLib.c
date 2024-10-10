/** @file
  The platform hook library.

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <PlatformBase.h>
#include <PchReservedResources.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci.h>
#include <Register/SerialIoUartRegs.h>
#include <Register/SaRegsHostBridge.h>
#include <Register/PchRegsLpc.h>

#define MM_PCI_OFFSET(Bus, Device, Function) \
    ( (UINTN)(Bus << 20) +    \
      (UINTN)(Device << 15) + \
      (UINTN)(Function << 12) )

CONST UINT32
mUartMmPciOffset[] = {
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART0, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART0),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART1, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART1),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART3, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART3),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART4, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART4),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART5, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART5),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART6, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART6),
};


UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
  )
{
  if (GetDebugPort () >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    // External UART, assume 0x3F8 I/O port
    return 1;
  } else {
    // SOC UART, MMIO only
    return 4;
  }
}


UINT64
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  UINT16  Cmd16;
  UINTN   PciAddress;
  UINT8   DebugPort;
  UINT64  MmioBase;

  DebugPort = GetDebugPort ();
  if (DebugPort >=  PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    if (DebugPort == 0xFE) {
      return 0x2F8;
    } else {
      return 0x3F8;
    }
  }
  Cmd16 = 0;
  PciAddress = mUartMmPciOffset[DebugPort] + (UINTN)PcdGet64(PcdPciExpressBaseAddress);

  Cmd16 = MmioRead16 (PciAddress + PCI_VENDOR_ID_OFFSET);
  if (Cmd16 == 0xFFFF) {
    return LPSS_UART_TEMP_BASE_ADDRESS(DebugPort);
  } else {
    if (MmioRead32 (PciAddress + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) {
      MmioBase  = LShiftU64 (MmioRead32 (PciAddress + PCI_BASE_ADDRESSREG_OFFSET + 4), 32);
      MmioBase += (MmioRead32 (PciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0);
      return MmioBase;
    } else {
      return 0;
    }
  }
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not initialize the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
LegacySerialPortInitialize (
  VOID
  )
{
  UINTN   EspiBaseAddr;
  UINT16  IoDecodeRanges;
  UINT16  IoDecodeEnable;
  UINT32  MchBar;

  // Decode range value, Bit 6:4: ComB range, Bit 2:0: ComA range
  EspiBaseAddr    = PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_ESPI, PCI_FUNCTION_NUMBER_PCH_ESPI, 0);
  IoDecodeRanges  = PciRead16 (EspiBaseAddr + R_LPC_CFG_IOD) & 0xFF84;
  IoDecodeRanges |= (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB);
  IoDecodeRanges |= (V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA);

  // Set PCH LPC/eSPI IO decode ranges in IOC
  MchBar  = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, MCHBAR_HOSTBRIDGE_CFG_REG));
  MchBar &= 0xFFFE0000;
  MmioWrite32 (MchBar + R_IOC_CFG_LPCIOD, IoDecodeRanges);

  // Program same decode value in LPC/eSPI PCI
  PciWrite16 (EspiBaseAddr + R_LPC_CFG_IOD, IoDecodeRanges);

  IoDecodeEnable = PciRead16 (EspiBaseAddr + R_LPC_CFG_IOE);
  IoDecodeEnable |= B_LPC_CFG_IOE_SE;
  IoDecodeEnable |= B_LPC_CFG_IOE_CBE;
  IoDecodeEnable |= B_LPC_CFG_IOE_CAE;
  IoDecodeEnable |= B_LPC_CFG_IOE_ME1;

  MmioWrite32 (MchBar + R_IOC_CFG_LPCIOE, IoDecodeEnable);
  PciWrite16 (EspiBaseAddr + R_LPC_CFG_IOE, IoDecodeEnable);

  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINTN   PciAddress;
  UINT32  BarAddress;
  UINT8   DebugPort;

  DebugPort = GetDebugPort ();
  if (DebugPort >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    LegacySerialPortInitialize ();
  } else {
    BarAddress = LPSS_UART_TEMP_BASE_ADDRESS(DebugPort);
    PciAddress = mUartMmPciOffset[DebugPort] + (UINTN)PcdGet64(PcdPciExpressBaseAddress);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR0_LOW,  BarAddress);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR0_HIGH, 0x0);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR1_LOW,  BarAddress + 0x1000);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR1_HIGH, 0x0);
    MmioWrite32 (PciAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE);
    MmioOr32    (PciAddress + R_SERIAL_IO_CFG_D0I3MAXDEVPG, BIT18 | BIT17 | BIT16);

    // get controller out of reset
    MmioOr32 (BarAddress + R_SERIAL_IO_MEM_PPR_RESETS,
      B_SERIAL_IO_MEM_PPR_RESETS_FUNC | B_SERIAL_IO_MEM_PPR_RESETS_APB | B_SERIAL_IO_MEM_PPR_RESETS_IDMA);

    // Program clock dividers for UARTs
    MmioWrite32 (BarAddress + R_SERIAL_IO_MEM_PPR_CLK,
        (B_SERIAL_IO_MEM_PPR_CLK_UPDATE | (V_SERIAL_IO_MEM_PPR_CLK_N_DIV << 16) |
         (V_SERIAL_IO_MEM_PPR_CLK_M_DIV << 1) | B_SERIAL_IO_MEM_PPR_CLK_EN )
        );
  }
  return RETURN_SUCCESS;
}
