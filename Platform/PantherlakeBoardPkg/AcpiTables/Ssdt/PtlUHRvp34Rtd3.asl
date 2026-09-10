/** @file
  Intel RVP RTD3 code

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Include/AcpiDebug.h>

DefinitionBlock (
  "Rtd3.aml",
  "SSDT",
  2,
  "Rtd3",
  "PtlRvp34",
  0x1000
  )
{
  Store ("[PTL UH LP5 RVP3 and RVP4 RTD3 SSDT][AcpiTableEntry]", Debug)
  Store (Timer, Debug)
  ACPI_DEBUG_EXTERNAL_REFERENCE
  ADBG ("[PTL UH LP5 RVP1 RTD3 SSDT][AcpiTableEntry]")

  External (\_SB.PC00.RP03.PXSX.PNVM, MethodObj)
  External (\_SB.PC00.RP03.PXSX.PAHC, MethodObj)
  Name (\_SB.PC00.RP03.WAKG, 0)


  Include ("PcieRvpRtd3Common.asl")
  //
  // PCIe Root Port Start
  //
  //
  // PCIe Slot  ( RP01)
  // P1PG, 32, // [PcieSlot1PowerEnableGpio ] Pcie Slot 1 Power Enable Gpio pin
  // P1EP, 8,  // [PcieSlot1PowerEnableGpioPolarity ] Pcie Slot 1 Power Enable Gpio pin polarity
  // P1RG, 32, // [PcieSlot1RstGpio  ] Pcie Slot 1 Rest Gpio pin
  // P1RP, 8,  // [PcieSlot1RstGpioPolarity] Pcie Slot 1 Rest Gpio pin polarity
  // RW03      // Wake Pin
  //


  If (LEqual (P1RN ,0x1)) {
    PCIE_RP_SCOPE_BEGIN(\_SB.PC00.RP01)
    PCIE_RP_SCOPE_BODY(P1PG,P1EP,P1RG,P1RP,RW01,2)
    Include ("PcieRpGenericPcieDeviceRtd3.asl")
    PCIE_RP_SCOPE_END
  }
  //
  // Gen4 M.2 SSD (x4 RP05)
  // SSDP, 32, // [PchM2SsdPowerEnableGpio            ] Pch M.2 SSD Power Enable Gpio pin
  // SDPP, 8,  // [PchM2SsdPowerEnableGpioPolarity    ] Pch M.2 SSD Power Enable Gpio pin polarity
  // SSDR, 32, // [PchM2SsdRstGpio                    ] Pch M.2 SSD Reset Gpio pin
  // SDRP, 8,  // [PchM2SsdRstGpioPolarity            ] Pch M.2 SSD Reset Gpio pin polarity
  //

  PCIE_RP_SCOPE_BEGIN(\_SB.PC00.RP05)
  PCIE_RP_SCOPE_BODY(SSDP,SDPP,SSDR,SDRP,RW05,6)
  Include ("PcieRpGenericPcieDeviceRtd3.asl")
  PCIE_SSD_EP_SCOPE_BEGIN(\_SB.PC00.RP05)
  Include ("PcieRpSsdStorageRtd3Hook.asl")
  PCIE_SSD_EP_SCOPE_END
  PCIE_RP_SCOPE_END

  //
  // Gen5 M.2 SSD (RP09)
  // SD2P, 32, // [M2Ssd2PowerEnableGpio          ] Pch M.2 SSD Power Enable Gpio pin
  // SDP1, 8,  // [M2Ssd2PowerEnableGpioPolarity  ] Pch M.2 SSD Power Enable Gpio pin polarity
  // SD2R, 32, // [M2Ssd2RstGpio                  ] Pch M.2 SSD Reset Gpio pin
  // SDR1, 8,  // [M2Ssd2RstGpioPolarity          ] Pch M.2 SSD Reset Gpio pin polarity
  //

  PCIE_RP_SCOPE_BEGIN(\_SB.PC00.RP09)
  PCIE_RP_SCOPE_BODY(SD2P,SDP1,SD2R,SDR1,RW09,1)
  Include ("PcieRpGenericPcieDeviceRtd3.asl")
  PCIE_SSD_EP_SCOPE_BEGIN(\_SB.PC00.RP09)
  Include ("PcieRpSsdStorageRtd3Hook.asl")
  PCIE_SSD_EP_SCOPE_END
  PCIE_RP_SCOPE_END


  // PCIe Slot 2 x8 slot
  // P2PG, 32, // [PcieSlot2PowerEnableGpio           ] Pcie Slot 2 Power Enable Gpio pin
  // P2EP, 8,  // [PcieSlot2PowerEnableGpioPolarity   ] Pcie Slot 2 Power Enable Gpio pin polarity
  // P2RG, 32, // [PcieSlot2RstGpio                   ] Pcie Slot 2 Rest Gpio pin
  // P2RP, 8,  // [PcieSlot2RstGpioPolarity           ] Pcie Slot 2 Rest Gpio pin polarity

  If (LEqual(P2RN ,0xB)) {
    PCIE_RP_SCOPE_BEGIN(\_SB.PC00.RP11)
    PCIE_RP_SCOPE_BODY(P2PG,P2EP,P2RG,P2RP,RW11,0)
    Include ("PcieRpGenericPcieDeviceRtd3.asl")
    PCIE_RP_SCOPE_END
  }

   // P3RN, 8,      // [PcieSlot3RpNumber       ] Pcie Slot 3 Root Port Number
   // P3PG, 32,     // [PcieSlot3PowerEnableGpio           ] Pcie Slot 3 Power Enable Gpio pin
   // P3EP, 8,      // [PcieSlot3PowerEnableGpioPolarity   ] Pcie Slot 3 Power Enable Gpio pin polarity
   // P3RG, 32,     // [PcieSlot3RstGpio                   ] Pcie Slot 3 Rest Gpio pin
   // P3RP, 8,      // [PcieSlot3RstGpioPolarity           ] Pcie Slot 3 Rest Gpio pin polarity

   // PCIe Slot 3 x4 slot
   If (LEqual(P3RN ,0xC)) {
    PCIE_RP_SCOPE_BEGIN(\_SB.PC00.RP12)
    PCIE_RP_SCOPE_BODY(P3PG,P3EP,P3RG,P3RP,RW12,7)
    Include ("PcieRpGenericPcieDeviceRtd3.asl")
    PCIE_RP_SCOPE_END
   }

  //
  // PCIe Root Port End
  //

  //
  // GPE handler for 2-tier GPIO Start
  //
  Scope (\_GPE) {
    //
    // x8 slot wake
    //
    If (CondRefOf (\_SB.PC00.RP11.WAKG)) {
      If (LNotEqual(\_SB.PC00.RP11.WAKG, 0)) {
        Method (_L38) {
          ADBG("_L38 RP11 SLOT")
          \_SB.SHPO(\_SB.PC00.RP11.WAKG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
          Notify(\_SB.PC00.RP11, 0x02)   // Device wake
        }
      }
    }

    //
    //  X4 slot wake
    //
    If (CondRefOf (\_SB.PC00.RP12.WAKG)) {
      If (LNotEqual(\_SB.PC00.RP12.WAKG, 0)) {
        Method (_L39) {
          ADBG("_L39 RP12 SLOT")
          \_SB.SHPO(\_SB.PC00.RP12.WAKG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
          Notify(\_SB.PC00.RP12, 0x02)   // Device wake
        }
      }
    }

    //
    // AL6F to handle the 2-tier GPIO GPE event
    //
    Method (AL6F) {
      ADBG ("AL6F Start!")
      // Root port 01
      PCIE_RP_L6F_WAKE(\_SB.PC00.RP01)
      ADBG ("AL6F End!")
    }
  }
  // GPE handler for 2-tier GPIO End

  ADBG ("[PTL UH LP5 RVP3 and RVP4 RTD3 SSDT][AcpiTableExit]")
  Store ("[PTL UH LP5 RVP3 and RVP4 RTD3 SSDT][AcpiTableExit]", Debug)
  Store (Timer, Debug)
} // End SSDT
