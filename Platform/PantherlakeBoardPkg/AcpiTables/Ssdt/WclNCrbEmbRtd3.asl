/** @file
  ACPI RTD3 SSDT table for WCL N DDR5 CRB

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "WclNCrb",
    0x1000
    )
{
//
// Board configuration WCL N DDR5 CRB CONFIG
// __________________________________________________________________________________________
// | RP Number | config | End Point                                | ClkSrc# | Lane Reversal |
// __________________________________________________________________________________________|
// |   RP01    |   x2   | SSD (M.2)                                |   3     |  No           |
// |   RP03    |   x1   | x1 PCIe Slot                             |   2     |  No           |
// |   RP04    |   x1   | WLAN (M.2)                               |   0     |  No           |
// |   RP05    |   x1   | WWAN (M.2)                               |   4     |  No           |
// |   RP06    |   x1   | Gbe                                      |   5     |  No           |
// __________________________________________________________________________________________|
//
  Store ("[Wcl N Crb RTD3 SSDT][AcpiTableEntry]", Debug)
  Store (Timer, Debug)
  ACPI_DEBUG_EXTERNAL_REFERENCE
  External (\_SB.PC00.RP01.PXSX, DeviceObj)
  External (\_SB.PC00.RP01.PXSX.PNVM, MethodObj)
  External (\_SB.PC00.RP01.PXSX.PAHC, MethodObj)
  ADBG ("[Wcl N Crb RTD3 SSDT][AcpiTableEntry]")

  Include ("PcieRvpRtd3Common.asl")

  //
  // M.2 Gen4 SSD Key-M (NIST) (x2 RP01)
  // SSDP, 32, // [PchM2SsdPowerEnableGpio            ] Pch M.2 SSD Power Enable Gpio pin
  // SDPP, 8,  // [PchM2SsdPowerEnableGpioPolarity    ] Pch M.2 SSD Power Enable Gpio pin polarity
  // SSDR, 32, // [PchM2SsdRstGpio                    ] Pch M.2 SSD Reset Gpio pin
  // SDRP, 8,  // [PchM2SsdRstGpioPolarity            ] Pch M.2 SSD Reset Gpio pin polarity
  //

  PCIE_RP_SCOPE_BEGIN (\_SB.PC00.RP01)
  PCIE_RP_SCOPE_BODY (SSDP,SDPP,SSDR,SDRP,RW01,3)
  Include ("PcieRpGenericPcieDeviceRtd3.asl")
  PCIE_SSD_EP_SCOPE_BEGIN (\_SB.PC00.RP01)
  Include ("PcieRpSsdStorageRtd3Hook.asl")
  PCIE_SSD_EP_SCOPE_END
  PCIE_RP_SCOPE_END

  //
  // PCIe Slot  ( RP03)
  // P1PG, 32, // [PcieSlot1PowerEnableGpio ] Pcie Slot 1 Power Enable Gpio pin
  // P1EP, 8,  // [PcieSlot1PowerEnableGpioPolarity ] Pcie Slot 1 Power Enable Gpio pin polarity
  // P1RG, 32, // [PcieSlot1RstGpio  ] Pcie Slot 1 Rest Gpio pin
  // P1RP, 8,  // [PcieSlot1RstGpioPolarity] Pcie Slot 1 Rest Gpio pin polarity
  // RW05      // Wake Pin
  //

  //
  // Pcie x1 Slot
  //
  If (LEqual (P1RN ,0x3)) {
    PCIE_RP_SCOPE_BEGIN (\_SB.PC00.RP03)
    PCIE_RP_SCOPE_BODY (P1PG,P1EP,P1RG,P1RP,RW05, 2)
    Include ("PcieRpGenericPcieDeviceRtd3.asl")
    PCIE_RP_SCOPE_END
  }


  Scope (\_GPE) {
    //
    // AL6F to handle the 2-tier GPIO GPE event
    //
    Method (AL6F) {
      ADBG ("AL6F Start!")
      //
      // PCIe Slot 1 (x1) Wake
      //
      PCIE_RP_L6F_WAKE(\_SB.PC00.RP03)

      ADBG ("AL6F End!")
    }

  }

  ADBG ("[Wcl N Crb RTD3 SSDT][AcpiTableExit]")
  Store ("[Wcl N Crb RTD3 SSDT][AcpiTableExit]", Debug)
  Store (Timer, Debug)
} // End SSDT
