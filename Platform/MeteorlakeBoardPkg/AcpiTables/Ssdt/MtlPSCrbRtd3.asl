/** @file
  ACPI RTD3 SSDT table for MTL PS CRB

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "MtlPSCrb",
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
//Pcie Slot #1 Pins
//
External(P1PG)
External(P1EP)
External(P1RG)
External(P1RP)
External(P1WG)

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
External(\_SB.PC00.RP10.PXSX,DeviceObj)
External(\_SB.PC00.RP05.PXSX.WIST,MethodObj)
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)
External(\_SB.PC00.UA02.BTH0, DeviceObj)


External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)

// Board configuration
// Board configuration MTL-PS Default topology DDR5 CRB

//
// _______________________________________________________________________________________
// | RP Number | config | End Point                                | ClkSrc# |  Lane Reversal |
// _______________________________________________________________________________________
// |   RP01    |   x4   | M.2 SSD Gen4  #1 / SATA  # 2             |   0     |   No           |
// |   RP05/Gbe|   x1   | GBE LAN                                  |   2     |   No           |
// |   RP06    |   x1   | NC                                       |   3     |   No           |
// |   RP07    |   x1   | M.2 WWAN                                 |   1     |   No           |
// |   RP08    |   x1   | M.2 WLAN                                 |   5     |   No           |
// |   RP09    |   x4   | x4 PCIe Gen4 DT Slot #1                  |   4     |   No           |
// |   RP10    |   x4   | M.2 SSD Gen4 Slot #2                     |   8     |   No           |
// |   RP11    |   x4   | NC                                       |   7     |   No           |
// |   RP12    |   x8   | NC                                       |   6     |   No           |
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


    //P1PG, 32,     // [PcieSlot1PowerEnableGpio           ] Pcie Slot 1 Power Enable Gpio pin
    //P1EP, 8,      // [PcieSlot1PowerEnableGpioPolarity   ] Pcie Slot 1 Power Enable Gpio pin polarity
    //P1RG, 32,     // [PcieSlot1RstGpio                   ] Pcie Slot 1 Rest Gpio pin
    //P1RP, 8,      // [PcieSlot1RstGpioPolarity           ] Pcie Slot 1 Rest Gpio pin polarity
    //P1WG, 32,     // [PcieSlot1WakeGpio                  ] Pcie Slot 1 Wake Gpio pin
    ///
    /// PCIE RTD3 - x4 PCIe Gen4 DT Slot #1
    ///
    Scope(\_SB.PC00.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(P1RG, Index(RSTG, 0))
      Store(P1RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P1PG, Index(PWRG, 0))
      Store(P1EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P1WG, WAKG)
      Name(SCLK, 4)
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
    /// PCIe RP10 RTD3 - M.2 SSD Gen4 Slot #2
    ///
    Scope(\_SB.PC00.RP10) {
      Name(RSTG, Package() {0, 0})
      Store(S2RG, Index(RSTG, 0))
      Store(S2RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S2PG, Index(PWRG, 0))
      Store(S2PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 8)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP10.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

// PCIe root ports - END

// USB - START
  Include ("Rtd3Xdci.asl")
// USB - END

If (LNotEqual(GBES,0)) {
  Scope(\_SB.PC00.GLAN)
  {
    Method (_PS3, 0, NotSerialized)
    {
    //   ADBG("GBE CSD3")
      \_SB.CSD3(MODPHY_SPD_GATING_GBE)
    }
    Method (_PS0, 0, NotSerialized)
    {
      If(LNot(GBED)){  // If GBE_FDIS_PMC == 0
        // ADBG("GBE CSD0")
        \_SB.CSD0(MODPHY_SPD_GATING_GBE)
      }
    }
  } // Scope(\_SB.PC00.GLAN)
}

//
// Human Interface Devices Start
//
  Include ("Rtd3MPRvpLpss.asl")
Include ("Rtd3Hdas.asl")

//GPE Event handling - Start
  Scope(\_GPE) {

    Method(_L0C) { // GPP_A_12 pin
    //   ADBG(" _L0C WLAN Event")
      \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
      Notify(\_SB.PC00.RP08, 0x02)      // device wake
    }

    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
    //   ADBG("AL6F Start")

      //Wwan wake event
      If (\_SB.ISME(WWKP))
      {
        // ADBG("AL6F WWAN")
        \_SB.SHPO(WWKP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(WWAN_PCIE_ROOT_PORT, 0x02)     // device wake
        \_SB.CAGS(WWKP)    // WWAN_WAKE_N
      }

      //x4 PCIe Gen4 DT Slot #1 wake event
      If (\_SB.ISME(P1WG))
      {
        // ADBG("AL6F X4 PCIe slot 1")
        \_SB.SHPO(P1WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP09, 0x02)     // device wake
        \_SB.CAGS(P1WG)    // Clear GPE event status
      }

    //   ADBG("AL6F End")
    }
  } //Scope(\_GPE)
//GPE Event handling - End

} // End SSDT
