/** @file
  ACPI RTD3 SSDT table for WCL N Lp5 T3 Rvp2

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "WclNRvp2",
    0x1000
    )
{
//
// Board configuration WCL N Lp5 T3 Rvp2 CONFIG1
// __________________________________________________________________________________________
// | RP Number | config | End Point                                | ClkSrc# | Lane Reversal |
// __________________________________________________________________________________________|
// |   RP01    |   x4   | SSD (M.2) / CEM Slot2                    |   3     |  No           |
// |   RP05    |   x1   | CEM Slot / WLAN (M.2)                    |   2     |  No           |
// |   RP06    |   x1   | Gbe / CEM Slot1                          |   5     |  No           |
// __________________________________________________________________________________________|
//
  ACPI_DEBUG_EXTERNAL_REFERENCE
  External (\_SB.PC00.RP01.PXSX, DeviceObj)
  External (\_SB.PC00.RP01.PXSX.PNVM, MethodObj)
  External (\_SB.PC00.RP01.PXSX.PAHC, MethodObj)

  Include ("PcieRvpRtd3Common.asl")

  //
  // M.2 Gen4 SSD Key-M (NIST) (x4 RP01)
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
  // PCIe Slot  ( RP05)
  // P1PG, 32, // [PcieSlot1PowerEnableGpio ] Pcie Slot 1 Power Enable Gpio pin
  // P1EP, 8,  // [PcieSlot1PowerEnableGpioPolarity ] Pcie Slot 1 Power Enable Gpio pin polarity
  // P1RG, 32, // [PcieSlot1RstGpio  ] Pcie Slot 1 Rest Gpio pin
  // P1RP, 8,  // [PcieSlot1RstGpioPolarity] Pcie Slot 1 Rest Gpio pin polarity
  // RW05      // Wake Pin
  //

  //
  // Pcie slot 1
  //
  If (LEqual (P1RN ,0x5)) {
    PCIE_RP_SCOPE_BEGIN (\_SB.PC00.RP05)
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
      PCIE_RP_L6F_WAKE(\_SB.PC00.RP05)

      ADBG ("AL6F End!")
    }
  }

  ADBG ("[Wcl N Rvp2 RTD3 SSDT][AcpiTableExit]")
} // End SSDT
