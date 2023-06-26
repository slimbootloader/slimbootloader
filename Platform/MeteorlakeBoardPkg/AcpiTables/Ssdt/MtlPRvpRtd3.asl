/** @file
  ACPI RTD3 SSDT table for MTL P RVP

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "MtlP_Rvp",
    0x1000
    )
{

External(S0ID)

//
//WWAN Pins
//
External(WRTO)
External(WBRS)
External(PBRS)
External(PRST)
External(WPRP)
External(WFCP)
External(PFCP)
External(WWKP)

//
//Pcie Slot #3 Pins
//
External(P3PG)
External(P3EP)
External(P3RG)
External(P3RP)
External(P3WG)

//
//Pcie Slot #4 Pins
//
External(P4PG)
External(P4EP)
External(P4RG)
External(P4RP)
External(P4WG)

//
// SSD1 Pins
//
External(S1PG)
External(S1PP)
External(S1RG)
External(S1RP)

//
// SSD2 Pins
//
External(S2PG)
External(S2PP)
External(S2RG)
External(S2RP)

//
// SSD3 Pins
//
External(S3PG)
External(S3PP)
External(S3RG)
External(S3RP)

//
// SSD4 Pins
//
External(S4PG)
External(S4PP)
External(S4RG)
External(S4RP)


External(XDCE)

External(GBED)

//
// Touch panel Pins
//
External(GPDI)
External(PPDI)
External(TPLS)

External(T0PE)
External(T0PR)
External(T0IP)
External(T0EP)
External(T0RP)
External(T0GI)

External(T1PE)
External(T1PR)
External(T1IP)
External(T1EP)
External(T1RP)
External(T1GI)

//
//WLAN wake Pins
//
External(WLWK)

//
//Vmd Pins
//
External(VMDE)

External(TPDT)
External(TPLT)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)

Include ("Rtd3Common.asl")

#define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP07
External(WWAN_PCIE_ROOT_PORT.PXSX, DeviceObj)
External(WWAN_PCIE_ROOT_PORT.LASX)
External(\_SB.PC00.RP01.PXSX,DeviceObj)
External(\_SB.PC00.RP09.PXSX,DeviceObj)
External(\_SB.PC00.RP10.PXSX,DeviceObj)
External(\_SB.PC00.RP11.PXSX,DeviceObj)
External(\_SB.PC00.RP05.PXSX.WIST,MethodObj)
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)
External(\_SB.PC00.UA02.BTH0, DeviceObj)


External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)

// Board configuration
// Board configuration MTL P Default topology DDR5 RVP

//
// _______________________________________________________________________________________
// | RP Number | config | End Point                                | ClkSrc# |  Lane Reversal |
// _______________________________________________________________________________________
// |   RP01    |   x4   | M.2 SSD Gen4  #1 / SATA Direct connect 1 |   0     |   No           |
// |   RP05/Gbe|   x1   | GBE LAN                                  |   2     |   No           |
// |   RP06    |   x1   | x1 PCIe DT Slot - #3                     |   3     |   No           |
// |   RP07    |   x1   | M.2 WWAN                                 |   1     |   No           |
// |   RP08    |   x1   | M.2 WLAN                                 |   5     |   No           |
// |   RP09    |   x4   | M.2 SSD Gen4 #2                          |   4     |   No           |
// |   RP10    |   x4   | M.2 SSD Gen4 #3 / x4 PCIe DT Slot - 2    |   8     |   No           |
// |   RP11    |   x4   | M.2 SSD Gen4 #4                          |   7     |   No           |
// |   RP12    |   x8   | x8 PCIe Gen5 DT Slot                     |   6     |   No           |
// _______________________________________________________________________________________
//

// PCIe root ports - START

    //S1PG, 32,     // [M2Ssd1PowerEnableGpio           ] Pcie slot 1 M.2 SSD Power Enable Gpio pin
    //S1PP, 8,      // [M2Ssd1PowerEnableGpioPolarity   ] Pcie slot 1 M.2 SSD Power Enable Gpio pin polarity
    //S1RG, 32,     // [M2Ssd1RstGpio                   ] Pcie slot 1 M.2 SSD Reset Gpio pin
    //S1RP, 8,      // [M2Ssd1RstGpioPolarity           ] Pcie slot 1 M.2 SSD Reset Gpio pin polarity
    ///
    /// PCIe RP01 RTD3 - M.2 SSD Gen4 - # 1 (Hybrid SSD x2+x2 support)
    ///
    Scope(\_SB.PC00.RP01) {
      Name(RSTG, Package() {0, 0})
      Store(S1RG, Index(RSTG, 0))
      Store(S1RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S1PG, Index(PWRG, 0))
      Store(S1PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP01.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }


    //P3PG, 32,     // [PcieSlot3PowerEnableGpio           ] Pcie Slot 3 Power Enable Gpio pin
    //P3EP, 8,      // [PcieSlot3PowerEnableGpioPolarity   ] Pcie Slot 3 Power Enable Gpio pin polarity
    //P3RG, 32,     // [PcieSlot3RstGpio                   ] Pcie Slot 3 Rest Gpio pin
    //P3RP, 8,      // [PcieSlot3RstGpioPolarity           ] Pcie Slot 3 Rest Gpio pin polarity
    //P3WG, 32,     // [PcieSlot3WakeGpio                  ] Pcie Slot 3 Wake Gpio pin
    ///
    /// PCIE RTD3 - x1 PCIe DT Slot - #3
    ///
    Scope(\_SB.PC00.RP06) {
      Name(RSTG, Package() {0, 0})
      Store(P3RG, Index(RSTG, 0))
      Store(P3RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P3PG, Index(PWRG, 0))
      Store(P3EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P3WG, WAKG)
      Name(SCLK, 3)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }

    //WFCP, 32,     // [WwanFullCardPowerOffGpio        ] WWAN Full card power off gpio pin
    //PFCP, 8,      // [WwanFullCardPowerOffGpioPolarity] WWAN Full card power off gpio pin polarity
    //WBRS, 32,     // [WwanBbrstGpio                   ] WWAN BBRST Gpio pin
    //PBRS, 8,      // [WwanBbrstGpioPolarity           ] WWAN BBRST Gpio pin polarity
    //WWKP, 32,     // [WwanWakeGpio                    ] WWAN Wake Gpio pin
    //PRST, 32,     // [WwanPerstGpio                   ] WWAN PERST Gpio pin
    //WPRP, 8,      // [WwanPerstGpioPolarity           ] WWAN PERST Gpio polarity
    //
    // PCIE RTD3 RP 07 - PCIe M.2 CONNECTOR WWAN
    //
    If (LNotEqual(WRTO,0)) {
      Scope(WWAN_PCIE_ROOT_PORT) {
        Name(BRST, Package() {0, 0})
        Store(WBRS, Index(BRST, 0))
        Store(PBRS, Index(BRST, 1))
        Name(RSTG, Package() {0, 0})
        Store(PRST, Index(RSTG, 0))
        Store(WPRP, Index(RSTG, 1))
        Name(PWRG, Package() {0, 0})
        Store(WFCP, Index(PWRG, 0))
        Store(PFCP, Index(PWRG, 1))
        Name(WAKG, 0)
        Store(WWKP, WAKG)
        Name(SCLK, 1)
        Include("Rtd3PcieWwan.asl")
      }
    }

    //S2PG, 32,     // [M2Ssd2PowerEnableGpio           ] Pcie slot 2 M.2 SSD Power Enable Gpio pin
    //S2PP, 8,      // [M2Ssd2PowerEnableGpioPolarity   ] Pcie slot 2 M.2 SSD Power Enable Gpio pin polarity
    //S2RG, 32,     // [M2Ssd2RstGpio                   ] Pcie slot 2 M.2 SSD Reset Gpio pin
    //S2RP, 8,      // [M2Ssd2RstGpioPolarity           ] Pcie slot 2 M.2 SSD Reset Gpio pin polarity
    ///
    /// PCIe RP09 RTD3 - M.2 SSD Gen4 - # 2
    ///
    Scope(\_SB.PC00.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(S2RG, Index(RSTG, 0))
      Store(S2RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S2PG, Index(PWRG, 0))
      Store(S2PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 4)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP09.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    //S3PG, 32,     // [M2Ssd3PowerEnableGpio           ] Pcie slot 3 M.2 SSD Power Enable Gpio pin
    //S3PP, 8,      // [M2Ssd3PowerEnableGpioPolarity   ] Pcie slot 3 M.2 SSD Power Enable Gpio pin polarity
    //S3RG, 32,     // [M2Ssd3RstGpio                   ] Pcie slot 3 M.2 SSD Reset Gpio pin
    //S3RP, 8,      // [M2Ssd3RstGpioPolarity           ] Pcie slot 3 M.2 SSD Reset Gpio pin polarity
    ///
    /// PCIe RP10 RTD3 - M.2 SSD Gen4 - #3
    ///
    Scope(\_SB.PC00.RP10) {
      Name(RSTG, Package() {0, 0})
      Store(S3RG, Index(RSTG, 0))
      Store(S3RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S3PG, Index(PWRG, 0))
      Store(S3PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 8)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP10.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    //S4PG, 32,     // [M2Ssd4PowerEnableGpio           ] Pcie slot 4 M.2 SSD Power Enable Gpio pin
    //S4PP, 8,      // [M2Ssd4PowerEnableGpioPolarity   ] Pcie slot 4 M.2 SSD Power Enable Gpio pin polarity
    //S4RG, 32,     // [M2Ssd4RstGpio                   ] Pcie slot 4 M.2 SSD Reset Gpio pin
    //S4RP, 8,      // [M2Ssd4RstGpioPolarity           ] Pcie slot 4 M.2 SSD Reset Gpio pin polarity
    ///
    /// PCIe RP11 RTD3 - M.2 SSD Gen4 - #4
    ///
    Scope(\_SB.PC00.RP11) {
      Name(RSTG, Package() {0, 0})
      Store(S4RG, Index(RSTG, 0))
      Store(S4RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S4PG, Index(PWRG, 0))
      Store(S4PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 7)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP11.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    //P4PG, 32,     // [PcieSlot4PowerEnableGpio           ] Pcie Slot 4 Power Enable Gpio pin
    //P4EP, 8,      // [PcieSlot4PowerEnableGpioPolarity   ] Pcie Slot 4 Power Enable Gpio pin polarity
    //P4RG, 32,     // [PcieSlot4RstGpio                   ] Pcie Slot 4 Rest Gpio pin
    //P4RP, 8,      // [PcieSlot4RstGpioPolarity           ] Pcie Slot 4 Rest Gpio pin polarity
    //P4WG, 32,     // [PcieSlot4WakeGpio                  ] Pcie Slot 4 Wake Gpio pin
    ///
    /// PCIE RTD3 - PCIE GEN5 DT Slot x8 - #4
    ///
    Scope(\_SB.PC00.RP12) {
      Name(RSTG, Package() {0, 0})
      Store(P4RG, Index(RSTG, 0))
      Store(P4RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P4PG, Index(PWRG, 0))
      Store(P4EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P4WG, WAKG)
      Name(SCLK, 6)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      If(LNotEqual (DGBA, 0)) {
        Include ("PcieRpDiscreteGraphicsDeviceRtd3Hook.asl")
      }
    }

// USB - END

If (LNotEqual(GBES,0)) {
  Scope(\_SB.PC00.GLAN)
  {
    Method (_PS3, 0, NotSerialized)
    {
      \_SB.CSD3(MODPHY_SPD_GATING_GBE)
    }
    Method (_PS0, 0, NotSerialized)
    {
      If(LNot(GBED)){  // If GBE_FDIS_PMC == 0
        \_SB.CSD0(MODPHY_SPD_GATING_GBE)
      }
    }
  } // Scope(\_SB.PC00.GLAN)
}

//
// Human Interface Devices Start
//


Include ("Rtd3Hdas.asl")

//GPE Event handling - Start
  Scope(\_GPE) {

    Method(_L0C) { // GPP_A_12 pin
      \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
      Notify(\_SB.PC00.RP08, 0x02)      // device wake
    }

    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      //Wwan wake event
      If (\_SB.ISME(WWKP))
      {
        \_SB.SHPO(WWKP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(WWAN_PCIE_ROOT_PORT, 0x02)     // device wake
        \_SB.CAGS(WWKP)    // WWAN_WAKE_N
      }

      //x1 PCIe Connector wake event
      If (\_SB.ISME(P3WG))
      {
        \_SB.SHPO(P3WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP06, 0x02)     // device wake
        \_SB.CAGS(P3WG)    // Clear GPE event status
      }
      //x8 PCIe Connector wake event
      If (\_SB.ISME(P4WG))
      {
        \_SB.SHPO(P4WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP12, 0x02)     // device wake
        \_SB.CAGS(P4WG)    // Clear GPE event status
      }
    }
  }
} // End SSDT
