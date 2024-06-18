/** @file
  ACPI RTD3 SSDT table for MTL-S UDIMM 1DPC ERB RVP

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiDebug.h>
#include <Register/PmcRegs.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "MtlS_Rvp",
    0x1000
    )
{
External(S0ID)


//
// SOC x16 slot  Pins
//
External(P4WG)
External(P4PG)
External(P4EP)
External(P4RG)
External(P4RP)

//
// IOE M.2 x4 SSD slot 1 Pins
//
External(S3PG)
External(S3PP)
External(S3RG)
External(S3RP)

//
//IOE M.2 x4 SSD slot 2 pins
//
External(S4PG)
External(S4PP)
External(S4RG)
External(S4RP)

//
// PCH M.2 SSD 1 pins
//
External(S1PG)
External(S1PP)
External(S1RG)
External(S1RP)

//
// PCH M.2 SSD 2 pins
//
External(S2PG)
External(S2PP)
External(S2RG)
External(S2RP)

//
// PCH PCIE x2 SLOT #2
//
External(P2PG)
External(P2EP)
External(P2RG)
External(P2RP)
External(P2WG)
External(P2RN)

//
// PCH PCIE x4 SLOT #3
//
External(P3PG)
External(P3EP)
External(P3RG)
External(P3RP)
External(P3WG)
External(P3RN)

//
// PCH PCIE x4 SLOT #1
//
External(P1PG)
External(P1EP)
External(P1RG)
External(P1RP)
External(P1WG)
External(P1RN)

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
//Vmd
//
External(VMDE)

External(TPDT)
External(TPLT)
External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.PGPI.SHPO, MethodObj)

Include ("Rtd3Common.asl")

External(\_SB.PC02.HDAS, DeviceObj)
External(\_SB.PC02.HDAS.VDID)

// CPU x4 Slot
External(\_SB.PC00.RP01,DeviceObj)
External(\_SB.PC00.RP01.PXSX,DeviceObj)

// IOE M.2 SSD Slots
External(\_SB.PC00.RP17,DeviceObj)
External(\_SB.PC00.RP17.PXSX,DeviceObj)
External(\_SB.PC00.RP21,DeviceObj)
External(\_SB.PC00.RP21.PXSX,DeviceObj)

// PCH M.2 SSD Slots
External(\_SB.PC02.RP01,DeviceObj)
External(\_SB.PC02.RP01.PXSX,DeviceObj)
External(\_SB.PC02.RP13,DeviceObj)
External(\_SB.PC02.RP13.PXSX,DeviceObj)

// PCH x2 Slot
External(\_SB.PC02.RP05,DeviceObj)
External(\_SB.PC02.RP05.PXSX,DeviceObj)

// PCH x4 Slots
External(\_SB.PC02.RP09,DeviceObj)
External(\_SB.PC02.RP09.PXSX,DeviceObj)
External(\_SB.PC02.RP21,DeviceObj)
External(\_SB.PC02.RP21.PXSX,DeviceObj)

External(\_SB.PC02.RP08.PXSX.WIST,MethodObj)

External(SPPR) // PchNvs SataPortPresence
External(\PSOC, MethodObj) // PS_ON control method
External(\PSOS, MethodObj) // PS_ON status method
External(\RPSC, MethodObj) // PS_ON D3cold mask method

External (\_SB.PRR2.S200)

// Board configuration
// Board configuration MTL S Default topology UDIMM 1DPC RVP
//
// _________________________________________________________________________________________________________
// | RP Number | config | End Point                                          | ClkSrc# |  Lane Reversal |
// _______________________________________________________________________________________
// |   RP01    |   x4   | M.2 KEY M SSD Gen4 #2                              |    4    |    No          |
// |   RP05    |   x2   | x2 PCIe Slot #2                                    |    5    |    No          |
// |   RP07/Gbe|   x1   | GBE LAN                                            |    6    |    No          |
// |   RP08    |   x1   | M.2 KEY E WLAN                                     |    7    |    No          |
// |   RP09    |   x4   | x4 PCIe Slot #3                                    |    8    |    No          |
// |   RP13    |   x4   | M.2 KEY M SSD Gen4 #1/SATA0 (P0-P3)SATA (4-7)      |    9    |    No          |
// |   RP21    |   x4   | x4 PCIe DT Slot - 1                                |    10   |    Yes         |

// __________________________________________________________________________________________________________
// | RP Number | config | End Point                        | ClkSrc#                      |  Lane Reversal |
// __________________________________________________________________________________________________________
// |   RP01    |   x16  | PCIe x16 slot 4                  | CLKOUT_SOC_GEN5_1 (CLKSRC1)  |    No          |
// |   RP17    |   x4   | CPU M.2 x4 SSD slot 2 (Gen4)     | CLKOUT_IOE_GEN4_1 (CLKSRC5)  |    No          |
// |   RP21    |   x4   | CPU M.2 x4 SSD slot 1 (Gen5)     | CLKOUT_IOE_GEN5_0 (CLKSRC4)  |    No          |

//
// Platform power distribution description.
// Keep the power resources in the "\" scope
// to allow Windows to disable them if they are not
// referenced by any devices. Windows will not turn off
// unused power resources located in device scope.
//
Scope(\) {
  Include ("PsonVeto.asl")
  //
  // Note: SATA PRT0~3 on ADL-S is conencted to M.2 slot
  // and will be powered up from the auxilary power supply during
  // PS_ON so it does not depend on ATX power resource.
  //
  PowerResource(SPR4, 0, 0) {
    Method(_STA) {Return(\STMS(4))}
    Method(_ON) {\STMC(4, 1)}
    Method(_OFF) {\STMC(4, 0)}
  }
  PowerResource(SPR5, 0, 0) {
    Method(_STA) {Return(\STMS(5))}
    Method(_ON) {\STMC(5, 1)}
    Method(_OFF) {\STMC(5, 0)}
  }
  PowerResource(SPR6, 0, 0) {
    Method(_STA) {Return(\STMS(6))}
    Method(_ON) {\STMC(6, 1)}
    Method(_OFF) {\STMC(6, 0)}
  }
  PowerResource(SPR7, 0, 0) {
    Method(_STA) {Return(\STMS(7))}
    Method(_ON) {\STMC(7, 1)}
    Method(_OFF) {\STMC(7, 0)}
  }
} // End of Scope(\)

// PCIe root ports - START

    ///
    /// CPU PCIE RP01 - SOC PCIe Slot 1 - X4 Connector
    ///
    //P4WG, 32,     // [PcieSlot4WakeGpio       ] Pcie Slot 4 Wake Gpio pin
    //P4RN, 8,      // [PcieSlot4RpNumber       ] Pcie Slot 4 Root Port Number
    //P4PG, 32,     // [PcieSlot4PowerEnableGpio           ] Pcie Slot 4 Power Enable Gpio pin
    //P4EP, 8,      // [PcieSlot4PowerEnableGpioPolarity   ] Pcie Slot 4 Power Enable Gpio pin polarity
    //P4RG, 32,     // [PcieSlot4RstGpio                   ] Pcie Slot 4 Rest Gpio pin
    //P4RP, 8,      // [PcieSlot4RstGpioPolarity           ] Pcie Slot 4 Rest Gpio pin polarity
    Scope(\_SB.PC00.RP01) {
      Name(RSTG, Package() {0, 0})
      Store(P4RG, Index(RSTG, 0))
      Store(P4RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P4PG, Index(PWRG, 0))
      Store(P4EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P4WG, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 1) // CLKOUT_SOC_GEN5_1
      //
      // CEM slot power limit
      // note: even though PCIe CEM form factor
      // defines maximum auxilary power as 1238mW
      // our PCIe CEM slots can safely handle 3300mW.
      //
      Name(PSPL, 3300)
      //
      // Block core power removal.
      // Arg0 - 0: Allow for core power removal, 1: block core power removal
      //
      Method(BCPR, 1, Serialized) {\PCPA(0, Arg0)}
      Method(CEMP, 1, Serialized) {\RPSC(0, Arg0)}
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP01.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCIE RP17 RTD3 - IOE M.2 SSD Slot 2 (Gen4)
    ///
    //S4PG, 32,     // [M2Ssd4PowerEnableGpio           ] Pcie slot 4 M.2 SSD Power Enable Gpio pin
    //S4PP, 8,      // [M2Ssd4PowerEnableGpioPolarity   ] Pcie slot 4 M.2 SSD Power Enable Gpio pin polarity
    //S4RG, 32,     // [M2Ssd4RstGpio                   ] Pcie slot 4 M.2 SSD Reset Gpio pin
    //S4RP, 8,      // [M2Ssd4RstGpioPolarity           ] Pcie slot 4 M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC00.RP17) {
      Name(RSTG, Package() {0, 0})
      Store(S4RG, Index(RSTG, 0))
      Store(S4RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S4PG, Index(PWRG, 0))
      Store(S4PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 5) // CLKOUT_IOE_GEN4_1
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP17.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCIE RP21 RTD3 - IOE M.2 SSD Slot 1 (Gen5)
    ///
    //S3PG, 32,     // [M2Ssd3PowerEnableGpio           ] Pcie slot 3 M.2 SSD Power Enable Gpio pin
    //S3PP, 8,      // [M2Ssd3PowerEnableGpioPolarity   ] Pcie slot 3 M.2 SSD Power Enable Gpio pin polarity
    //S3RG, 32,     // [M2Ssd3RstGpio                   ] Pcie slot 3 M.2 SSD Reset Gpio pin
    //S3RP, 8,      // [M2Ssd3RstGpioPolarity           ] Pcie slot 3 M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC00.RP21) {
      Name(RSTG, Package() {0, 0})
      Store(S3RG, Index(RSTG, 0))
      Store(S3RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S3PG, Index(PWRG, 0))
      Store(S3PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 4) // CLKOUT_IOE_GEN5_0
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP21.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH RP01 RTD3 - PCH M.2 SSD Gen4 #2
    ///
    //S2PG, 32,     // [M2Ssd2PowerEnableGpio           ] Pcie slot 1 M.2 SSD Power Enable Gpio pin
    //S2PP, 8,      // [M2Ssd2PowerEnableGpioPolarity   ] Pcie slot 1 M.2 SSD Power Enable Gpio pin polarity
    //S2RG, 32,     // [M2Ssd2RstGpio                   ] Pcie slot 1 M.2 SSD Reset Gpio pin
    //S2RP, 8,      // [M2Ssd2RstGpioPolarity           ] Pcie slot 1 M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC02.RP01) {
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
      Scope(\_SB.PC02.RP01.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH RP05 RTD3 - PCH PCIe Slot 2 - X2 Connector
    ///
    //P2WG, 32,     // [PcieSlot2WakeGpio                  ] Pcie Slot 2 Wake Gpio pin
    //P2RN, 8,      // [PcieSlot2RpNumber                  ] Pcie Slot 2 Root Port Number
    //P2PG, 32,     // [PcieSlot2PowerEnableGpio           ] Pcie Slot 2 Power Enable Gpio pin
    //P2EP, 8,      // [PcieSlot2PowerEnableGpioPolarity   ] Pcie Slot 2 Power Enable Gpio pin polarity
    //P2RG, 32,     // [PcieSlot2RstGpio                   ] Pcie Slot 2 Rest Gpio pin
    //P2RP, 8,      // [PcieSlot2RstGpioPolarity           ] Pcie Slot 2 Rest Gpio pin polarity
    Scope(\_SB.PC02.RP05) {
      Name(RSTG, Package() {0, 0})
      Store(P2RG, Index(RSTG, 0))
      Store(P2RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P2PG, Index(PWRG, 0))
      Store(P2EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P2WG, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 5)
      //
      // CEM slot power limit
      // note: even though PCIe CEM form factor
      // defines maximum auxilary power as 1238mW
      // our PCIe CEM slots can safely handle 3300mW.
      //
      Name(PSPL, 3300)
      //
      // Block core power removal.
      // Arg0 - 0: Allow for core power removal, 1: block core power removal
      //
      Method(BCPR, 1, Serialized) {\PCPA(1, Arg0)}
      Method(CEMP, 1, Serialized) {\RPSC(1, Arg0)}
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC02.RP05.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH RP09 RTD3 - PCH PCIe Slot 3 - X4 Connector
    ///
    //P3WG, 32,     // [PcieSlot3WakeGpio                  ] Pcie Slot 3 Wake Gpio pin
    //P3RN, 8,      // [PcieSlot3RpNumber                  ] Pcie Slot 3 Root Port Number
    //P3PG, 32,     // [PcieSlot3PowerEnableGpio           ] Pcie Slot 3 Power Enable Gpio pin
    //P3EP, 8,      // [PcieSlot3PowerEnableGpioPolarity   ] Pcie Slot 3 Power Enable Gpio pin polarity
    //P3RG, 32,     // [PcieSlot3RstGpio                   ] Pcie Slot 3 Rest Gpio pin
    //P3RP, 8,      // [PcieSlot3RstGpioPolarity           ] Pcie Slot 3 Rest Gpio pin polarity
    Scope(\_SB.PC02.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(P3RG, Index(RSTG, 0))
      Store(P3RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P3PG, Index(PWRG, 0))
      Store(P3EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P3WG, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 8)
      //
      // CEM slot power limit
      // note: even though PCIe CEM form factor
      // defines maximum auxilary power as 1238mW
      // our PCIe CEM slots can safely handle 3300mW.
      //
      Name(PSPL, 3300)
      //
      // Block core power removal.
      // Arg0 - 0: Allow for core power removal, 1: block core power removal
      //
      Method(BCPR, 1, Serialized) {\PCPA(2, Arg0)}
      Method(CEMP, 1, Serialized) {\RPSC(2, Arg0)}
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC02.RP09.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH RP13 RTD3 - PCH M.2 SSD Gen4 #1
    ///
    //S1PG, 32,     // [M2Ssd1PowerEnableGpio           ] Pcie slot 1 M.2 SSD Power Enable Gpio pin
    //S1PP, 8,      // [M2Ssd1PowerEnableGpioPolarity   ] Pcie slot 1 M.2 SSD Power Enable Gpio pin polarity
    //S1RG, 32,     // [M2Ssd1RstGpio                   ] Pcie slot 1 M.2 SSD Reset Gpio pin
    //S1RP, 8,      // [M2Ssd1RstGpioPolarity           ] Pcie slot 1 M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC02.RP13) {
      Name(RSTG, Package() {0, 0})
      Store(S1RG, Index(RSTG, 0))
      Store(S1RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(S1PG, Index(PWRG, 0))
      Store(S1PP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 9)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC02.RP13.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH RP21 RTD3 - PCH PCIe Slot 1 - X4 Connector
    ///
    //P1WG, 32,     // [PcieSlot1WakeGpio                  ] Pcie Slot 1 Wake Gpio pin
    //P1RN, 8,      // [PcieSlot1RpNumber                  ] Pcie Slot 1 Root Port Number
    //P1PG, 32,     // [PcieSlot1PowerEnableGpio           ] Pcie Slot 1 Power Enable Gpio pin
    //P1EP, 8,      // [PcieSlot1PowerEnableGpioPolarity   ] Pcie Slot 1 Power Enable Gpio pin polarity
    //P1RG, 32,     // [PcieSlot1RstGpio                   ] Pcie Slot 1 Rest Gpio pin
    //P1RP, 8,      // [PcieSlot1RstGpioPolarity           ] Pcie Slot 1 Rest Gpio pin polarity
    Scope(\_SB.PC02.RP21) {
      Name(RSTG, Package() {0, 0})
      Store(P1RG, Index(RSTG, 0))
      Store(P1RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P1PG, Index(PWRG, 0))
      Store(P1EP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P1WG, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 10)
      //
      // CEM slot power limit
      // note: even though PCIe CEM form factor
      // defines maximum auxilary power as 1238mW
      // our PCIe CEM slots can safely handle 3300mW.
      //
      Name(PSPL, 3300)
      //
      // Block core power removal.
      // Arg0 - 0: Allow for core power removal, 1: block core power removal
      //
      Method(BCPR, 1, Serialized) {\PCPA(3, Arg0)}
      Method(CEMP, 1, Serialized) {\RPSC(3, Arg0)}
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC02.RP21.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

// PCIe root ports - END

//
// SATA - START
//
  Scope(\_SB.PC02.SAT0) {
    Scope(PRT4) {
      Name(_S0W, 4)
      Method(SPON) {
        \STMC(4, 1)
      }
      Method(SPOF) {
        \STMC(4, 0)
      }
      //
      // Check with PRES method if SATA port is present
      // and if OS will be able to enumerate it. We can't return
      // PowerResources for devices that won't be enumerated since
      // Windows won't turn off power resources for such devices.
      //
      External(PRES, MethodObj)
      If (PRES()) {
        Method(_PR0) {Return(Package() {SPR4})}
        Method(_PR3) {Return(Package() {SPR4})}
      }
   }

   Scope(PRT5) {
     Name(_S0W, 4)
     Method(SPON) {
       \STMC(5, 1)
     }
     Method(SPOF) {
       \STMC(5, 0)
     }
     External(PRES, MethodObj)
     If (PRES()) {
       Method(_PR0) {Return(Package() {SPR5})}
       Method(_PR3) {Return(Package() {SPR5})}
     }
    }

    Scope(PRT6) {
      Name(_S0W, 4)
      Method(SPON) {
        \STMC(6, 1)
      }
      Method(SPOF) {
        \STMC(6, 0)
      }
      External(PRES, MethodObj)
      If (PRES()) {
        Method(_PR0) {Return(Package() {SPR6})}
        Method(_PR3) {Return(Package() {SPR6})}
      }
    }

    Scope(PRT7) {
     Name(_S0W, 4)
     Method(SPON) {
       \STMC(7, 1)
     }
     Method(SPOF) {
       \STMC(7, 0)
     }
     External(PRES, MethodObj)
     If (PRES()) {
       Method(_PR0) {Return(Package() {SPR7})}
       Method(_PR3) {Return(Package() {SPR7})}
     }
    }
  }

//
// SATA - END
//

// USB - START
  Include ("Rtd3Xdci.asl")
// USB - END



If (LNotEqual(\_SB.PC02.GBES,0)) {
  Scope(\_SB.PC02.GLAN)
  {
    Method (_PS3, 0, NotSerialized)
    {
      ADBG("GBE CSD3")
      \_SB.CSD3(MODPHY_SPD_GATING_GBE)
    }

    Method (_PS0, 0, NotSerialized)
    {
      If(LNot(GBED)) {  // If GBE_FDIS_PMC == 0
        ADBG("GBE CSD0")
        \_SB.CSD0(MODPHY_SPD_GATING_GBE)
      }
    }
  } // Scope(\_SB.PC02.GLAN)
}

//
// Human Interface Devices Start
//

//GPE Event handling - Start
  Scope(\_GPE) {
    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      ADBG("AL6F Start")
      // Add methods to handle 2-tier RTD3 GPE events generated by SOC GPIO Wake pins
      ADBG("AL6F End")
    }

    //
    // Alternate _L6B(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6B) {
      ADBG("AL6B Start")

      // SOC PCIe Slot 4 - X4 PCIe Connector (SLOT4) wake event
      If (\_SB.PGPI.IGPI(P4WG))
      {
        ADBG("AL6B SOC PCIe X4 SLOT4")
        \_SB.PGPI.SHPO(P4WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP01, 0x02)   // device wake
        \_SB.PGPI.CGPI(P4WG)
      }

      // WLAN wake event
      If (\_SB.PGPI.IGPI(WLWK))
      {
        ADBG("AL6B WLAN")
        \_SB.PGPI.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC02.RP08, 0x02)      // device wake
        \_SB.PGPI.CGPI(WLWK)    // WIFI_WAKE_N
      }

      // PCH RP05 - X2 PCIe Connector (SLOT2) wake event
      If (\_SB.PGPI.IGPI(P2WG))
      {
        ADBG("AL6B PCH PCIe X2 SLOT2")
        \_SB.PGPI.SHPO(P2WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC02.RP05, 0x02)   // device wake
        \_SB.PGPI.CGPI(P2WG)
      }

      // PCH RP09 - X4 PCIe Connector (SLOT3) wake event
      If (\_SB.PGPI.IGPI(P3WG))
      {
        ADBG("AL6B PCH PCIe X4 SLOT3")
        \_SB.PGPI.SHPO(P3WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC02.RP09, 0x02)   // device wake
        \_SB.PGPI.CGPI(P3WG)
      }

      // PCH RP21 - X4 PCIe Connector (SLOT1) wake event
      If (\_SB.PGPI.IGPI(P1WG))
      {
        ADBG("AL6B PCH PCIe X4 SLOT3")
        \_SB.PGPI.SHPO(P1WG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC02.RP21, 0x02)   // device wake
        \_SB.PGPI.CGPI(P1WG)
      }

      // all GPIOs on MTL-S RVP platform are tier-2 on PCH so only tier-2 event is required
      // to be cleared
      Store (B_ACPI_IO_GPE0_STS_127_96_GPIO_TIER2_SCI_STS, \_SB.PRR2.S200)

      ADBG("AL6B End")
    }
  } //Scope(\_GPE)
//GPE Event handling - End
} // End SSDT
