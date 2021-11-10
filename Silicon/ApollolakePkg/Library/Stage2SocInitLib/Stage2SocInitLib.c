/** @file
  The platform hook library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <RegAccess.h>
#include <GpioDefines.h>
#include <GlobalNvsArea.h>

/**
  Get size of global nvs area.

  @param[in] none

  @retval    UINT32     Size of global nvs area.

**/
UINT32
EFIAPI
GetAcpiGnvsSize (
  VOID
)
{
  return sizeof(GLOBAL_NVS_AREA);
}

/**
 Update GPIO address and length to global NVS data.

 @param [in] GnvsIn Pointer to Global NVS data.
 **/
VOID
EFIAPI
SocUpdateAcpiGnvs (
  IN VOID  *GnvsIn
  )
{
  GLOBAL_NVS_AREA      *Gnvs;
  PLATFORM_NVS_AREA    *Pnvs;
  CPU_NVS_AREA         *Cnvs;
  UINT32                Bar;
  UINT32                Port;



  Gnvs = (GLOBAL_NVS_AREA *)GnvsIn;
  Pnvs = &Gnvs->PlatformNvs;
  Cnvs = &Gnvs->CpuNvs;

  Pnvs->BxtStepping      = 0x51;

  Pnvs->IPCBar0Address   = PciRead32(PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC, R_P2SB_BASE)) & ~0x0F;;
  Pnvs->IPCBar0Length    = 0x2000;
  DEBUG((DEBUG_INFO, "  IPCBar0Address Address = 0x%08X, Len = 0x%08X\n", Pnvs->IPCBar0Address, Pnvs->IPCBar0Length));

  Pnvs->SSRAMBar0Address = PciRead32(PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC_SSRAM, R_P2SB_BASE)) & ~0x0F;;
  Pnvs->SSRAMBar0Length  = 0x2000;
  DEBUG((DEBUG_INFO, "  SSRAMBar0Address Address = 0x%08X, Len = 0x%08X\n", Pnvs->SSRAMBar0Address, Pnvs->SSRAMBar0Length));

  Bar = PciRead32(PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, R_P2SB_BASE));
  if (Bar == 0xFFFFFFFF) {
    Pnvs->P2SBBaseAddress = SC_PCR_BASE_ADDRESS;
  } else {
    Pnvs->P2SBBaseAddress = Bar & B_PCH_P2SB_SBREG_RBA;
  }
  DEBUG((DEBUG_INFO, "  P2SBBaseAddress Old = 0x%08X, New = 0x%08X\n", Bar, Pnvs->P2SBBaseAddress));

  Bar = PciRead32 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_SC, 0, 0, CUNIT_MCHBAR_REG)) & 0xFFFF7000;
  Pnvs->IPCBIOSMailBoxData      = (UINT32)Bar + MCHBAR_RANGE_PUNIT + PUNIT_MAILBOX_DATA;
  Pnvs->IPCBIOSMailBoxInterface = (UINT32)Bar + MCHBAR_RANGE_PUNIT + PUNIT_MAILBOX_INTERFACE;
  DEBUG((DEBUG_INFO, "  BIOS MAIL-BOX Data= 0x%08X \n", Pnvs->IPCBIOSMailBoxData));
  DEBUG((DEBUG_INFO, "  BIOS MAIL-BOX Interface= 0x%08X \n", Pnvs->IPCBIOSMailBoxInterface));

  Bar = Pnvs->P2SBBaseAddress;
  Pnvs->GPIO0Addr = P2SB_MMIO_ADDR (Bar, GPIO_MMIO_OFFSET_N, 0);
  Pnvs->GPIO0Len  = GPIO_NORTH_COMMUNITY_LENGTH;
  DEBUG((DEBUG_INFO, "  GPIO0Addr Address = 0x%08X, Len = 0x%08X\n", Pnvs->GPIO0Addr, Pnvs->GPIO0Len));

  Pnvs->GPIO1Addr = P2SB_MMIO_ADDR (Bar, GPIO_MMIO_OFFSET_NW, 0);
  Pnvs->GPIO1Len  = GPIO_NORTHWEST_COMMUNITY_LENGTH;
  DEBUG((DEBUG_INFO, "  GPIO1Addr Address = 0x%08X, Len = 0x%08X\n", Pnvs->GPIO1Addr, Pnvs->GPIO1Len));

  Pnvs->GPIO2Addr = P2SB_MMIO_ADDR (Bar, GPIO_MMIO_OFFSET_W, 0);
  Pnvs->GPIO2Len  = GPIO_WEST_COMMUNITY_LENGTH;
  DEBUG((DEBUG_INFO, "  GPIO2Addr Address = 0x%08X, Len = 0x%08X\n", Pnvs->GPIO2Addr, Pnvs->GPIO2Len));

  Pnvs->GPIO3Addr = P2SB_MMIO_ADDR (Bar, GPIO_MMIO_OFFSET_SW, 0);
  Pnvs->GPIO3Len  = GPIO_SOUTHWEST_COMMUNITY_LENGTH;
  DEBUG((DEBUG_INFO, "  GPIO3Addr Address = 0x%08X, Len = 0x%08X\n", Pnvs->GPIO3Addr, Pnvs->GPIO3Len));

  for (Port = 0; Port < PCIE_MAX_ROOT_PORTS; Port++) {
    Pnvs->RpAddress[Port] = (SC_PCIE_ROOT_PORT_BUS (Port) << 16);
    Pnvs->RpAddress[Port] |= SC_PCIE_ROOT_PORT_FUNC (Port);
    DEBUG((DEBUG_INFO, "  RpAddress Port[%d] Address = 0x%08X\n", Port, Pnvs->RpAddress[Port]));
  }

  Cnvs->PpmFlags = PPM_EIST | PPM_C1 | PPM_C1E | PPM_TURBO;
  Cnvs->LogicalProcessorCount = Pnvs->LogicalProcessorCount;
  Cnvs->MonitorMwaitEnable    = 0;
}
