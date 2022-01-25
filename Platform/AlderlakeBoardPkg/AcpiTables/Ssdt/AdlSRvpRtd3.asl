/** @file
  ACPI RTD3 SSDT table for ADL S Dddr4 Udimm  RVP

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <AcpiDebug.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "AdlS_Rvp",
    0x1000
    )
{

External(GBED)
Include("Rtd3Common.asl")

External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)
External(PFCP)
External(WBRS)
External(PBRS)
External(GPDI)
External(GPLI)
External(GPLP)
External(GPLR)
External(GPI1)
External(TPP1)
External(TPR1)
External(PPDI)
External(PPLI)
External(PPLP)
External(PPLR)
External(PPI1)
External(PPP1)
External(PPR1)
External(PSPE)
External(PPSP)
External(PSPR)
External(PPSR)
External(PSWP)
External(RPNB)
External(PSW2)
External(RPN2)
External(PSP2)
External(PS2P)
External(PSR2)
External(PSP3)
External(PS3P)
External(PSR3)
External(SR3P)
External(PSW3)
External(RPN3)
External(SR2P)
External(WLWK)
External(SATP)
External(STPP)
External(SSDP)
External(SDPP)
External(SSDR)
External(SDRP)
External(SD2P)
External(SDP1)
External(SD2R)
External(SDR1)
External(SD3P)
External(SDP3)
External(SD3R)
External(SDR3)
External(SD4P)
External(SDP4)
External(SD4R)
External(SDR4)
External(TPLS)
External(P1PE)
External(P1PP)
External(P1RE)
External(P1RP)
External(P1WP)
External(PRP1)
External(P2PE)
External(P2PP)
External(P2RE)
External(P2RP)
External(P2WP)
External(PRP2)
External(VMDE)
External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)

External(\_SB.PC00.XHCI.RHUB.HS14, DeviceObj)
External(\_SB.PC00.PEG0.PEGP, DeviceObj)
External(\_SB.PC00.RP13.PXSX, DeviceObj)
External(\_SB.PC00.RP01.PXSX, DeviceObj)
External(\_SB.PC00.RP25.PXSX, DeviceObj)

External(SPPR) // PchNvs SataPortPresence
External(\PSOC, MethodObj) // PS_ON control method
External(\PSOS, MethodObj) // PS_ON status method

//
// Platform power distribution description.
// Keep the power resources in the "\" scope
// to allow Windows to disable them if they are not
// referenced by any devices. Windows will not turn off
// unused power resources located in device scope.
//
Scope(\) {

  Name(GPOM, 0) // Global mask to track PS_ON consent from IPs.

  //
  // Global power removal veto tracker. Gathers information from all IPs about power
  // removal consent.
  // Arg0 - Interface index, 0 - SATA, 1 - PCIe
  // Arg1 - 0: Allow power removal, 1: veto power removal
  //
  Method(GPRV, 2, Serialized) {
    ShiftLeft(0x1, Arg0, Local0)
    If(Arg1) {
      Or(\GPOM, Local0, \GPOM)
    } Else {
      And(\GPOM, Not(Local0), \GPOM)
    }
    If(LEqual(\GPOM, 0)) {
      \PSOC(1)
    } Else {
      \PSOC(0)
    }
  }

  Name(PCPB, 0x0) // PCIe core power veto bitmask, default - allow for core power removal

  //
  // PCIe slots core power removal permission
  // aggregation.
  // Arg0 - PCIe index in the aggregation bitmask
  //           Indexing:
  //           RP21 - index 0
  //           RP09 - index 1
  //           RP05 - index 2
  //           PEG1 - index 3
  //           PEG2 - index 4
  // Arg1 - 0: agree to core power down, 1: veto core power down
  Method(PCPA, 2, Serialized) {
    ShiftLeft(0x1, Arg0, Local0)
    If(Arg1) {
      Or(\PCPB, Local0, \PCPB)
    } Else {
      And(\PCPB, Not(Local0), \PCPB)
    }
    If(LEqual(PCPB, 0)) {
      \GPRV(1, 0)
    } Else {
      \GPRV(1, 1)
    }
  }

  Name(SATM, 0) // SATA ports disable bitmask. Each bit represents SATA port.

  //
  // Update SATA bitmask that allow for PS_ON entry
  // Arg0 - SATA port index
  // Arg1 - 0: SATA disable, 1: SATA enable
  //
  Method(STMC, 2, Serialized) {
    //
    // This is just to clear the junk values on SATM variables since it might be modified
    // unexpectedly by RST driver via _DSM method.
    //
    And(\SATM, SPPR, Local0)
    Store (Local0, SATM)
    ShiftLeft(0x1, Arg0, Local0)
    If(Arg1) {
      Or(\SATM, Local0, \SATM)
    } Else {
      And(\SATM, Not(Local0), \SATM)
    }
    If(LEqual(\SATM, 0)) {
      \GPRV(0,0)
    } Else {
      \GPRV(0,1)
    }
  }

  //
  // Get the SATA port status in the mask
  // Arg0 - SATA port index
  // @return 0: SATA disabled, 1: SATA enabled
  //
  Method(STMS, 1, Serialized) {
    ShiftLeft(0x1, Arg0, Local0)
    And(Local0, \SATM, Local1)
    If(Local1) {
      Return(1)
    } Else {
      Return(0)
    }
  }

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

  //
  // PCIe power budgeting support.
  //
  External(\AUPL)        // Auxilary power limit
  Name(AURB, 0xFFFFFFFF) // Auxilary power remaining budget. Initialized on demand to \AUPL value by budgeting method.

  //
  // Reserve auxilary power budget.
  // Arg0 - 0: release, 1: reserve
  // Arg1 - power in mW
  //
  // @retval 0 - Request denied, remaining power not updated
  // @retval 1 - Request accepted, remaining power updated
  //
  Method(RAPC, 2, Serialized) {
    If(LEqual(\AURB, 0xFFFFFFFF)) {
      Store(\AUPL, \AURB)
    }
    If(Arg0) {
      If(LGreaterEqual(\AURB, Arg1)) {
        Subtract(\AURB, Arg1, \AURB)
        Return(1)
      } Else {
        Return(0)
      }
    } Else {
      Add(Arg1, \AURB, \AURB)
      Return(1)
    }
  } //End of RAPC
} // End of Scope(\)

// PCIe root ports - START
    ///
    /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR
    ///
    //PSPE, 32,     // [PcieSlot1PowerEnableGpio           ] Pcie Slot 1 Power Enable Gpio pin
    //PPSP, 8,      // [PcieSlot1PowerEnableGpioPolarity   ] Pcie Slot 1 Power Enable Gpio pin polarity
    //PSPR, 32,     // [PcieSlot1RstGpio                   ] Pcie Slot 1 Rest Gpio pin
    //PPSR, 8,      // [PcieSlot1RstGpioPolarity           ] Pcie Slot 1 Rest Gpio pin polarity
    //PSWP, 32,     // [PcieSlot1WakeGpio       ] Pcie Slot Wake Gpio pin
    Scope(\_SB.PC00.RP21) {
      Name(RSTG, Package() {0, 0})
      Store(PSPR, Index(RSTG, 0))
      Store(PPSR, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSPE, Index(PWRG, 0))
      Store(PPSP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(PSWP, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 2)
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
      Include("PcieAuxDSD.asl")
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }

    ///
    /// PCIE RTD3 - PCIE SLOT 2 - X4 CONNECTOR
    ///
    //PSP2, 32,     // [PcieSlot2PowerEnableGpio          ] Pcie Slot 2 Power Enable Gpio pin
    //PS2P, 8,      // [PcieSlot2PowerEnableGpioPolarity  ] Pcie Slot 2 Power Enable Gpio pin polarity
    //PSR2, 32,     // [PcieSlot2RstGpio                  ] Pcie Slot 2 Rest Gpio pin
    //SR2P, 8,      // [PcieSlot2RstGpioPolarity          ] Pcie Slot 2 Rest Gpio pin polarity
    //PSW2, 32,     // [PcieSlot2WakeGpio      ] Pcie Slot 2 Wake Gpio pin
    Scope(\_SB.PC00.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(PSR2, Index(RSTG, 0))
      Store(SR2P, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSP2, Index(PWRG, 0))
      Store(PS2P, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(PSW2, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 14)
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
      Include("PcieAuxDSD.asl")
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }

    ///
    /// PCIE RTD3 - PCIE SLOT 3 - X2 CONNECTOR
    ///
    //PSP3, 32,     // [PcieSlot3PowerEnableGpio          ] Pcie Slot 3 Power Enable Gpio pin
    //PS3P, 8,      // [PcieSlot3PowerEnableGpioPolarity  ] Pcie Slot 3 Power Enable Gpio pin polarity
    //PSR3, 32,     // [PcieSlot3RstGpio                  ] Pcie Slot 3 Rest Gpio pin
    //SR3P, 8,      // [PcieSlot3RstGpioPolarity          ] Pcie Slot 3 Rest Gpio pin polarity
    //PSW3, 32,     // [PcieSlot3WakeGpio       ] Pcie Slot Wake Gpio pin
    Scope(\_SB.PC00.RP05) {
      Name(RSTG, Package() {0, 0})
      Store(PSR3, Index(RSTG, 0))
      Store(SR3P, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSP3, Index(PWRG, 0))
      Store(PS3P, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(PSW3, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 15)
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
      Include("PcieAuxDSD.asl")
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }

    ///
    /// PCH M.2 SSD RTD3
    ///
    //SSDP, 32,     // [PchM2SsdPowerEnableGpio           ] Pch M.2 SSD Power Enable Gpio pin
    //SDPP, 8,      // [PchM2SsdPowerEnableGpioPolarity   ] Pch M.2 SSD Power Enable Gpio pin polarity
    //SSDR, 32,     // [PchM2SsdRstGpio                   ] Pch M.2 SSD Reset Gpio pin
    //SDRP, 8,      // [PchM2SsdRstGpioPolarity           ] Pch M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC00.RP13) {
      Name(RSTG, Package() {0, 0})
      Store(SSDR, Index(RSTG, 0))
      Store(SDRP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SSDP, Index(PWRG, 0))
      Store(SDPP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 12)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP13.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH M.2 SSD2 RTD3
    ///
    //SD3P, 32,     // [PchM2Ssd2PowerEnableGpio           ] Pch M.2 SSD2 Power Enable Gpio pin
    //SDP3, 8,      // [PchM2Ssd2PowerEnableGpioPolarity   ] Pch M.2 SSD2 Power Enable Gpio pin polarity
    //SD3R, 32,     // [PchM2Ssd2RstGpio                   ] Pch M.2 SSD2 Reset Gpio pin
    //SDR3, 8,      // [PchM2Ssd2RstGpioPolarity           ] Pch M.2 SSD2 Reset Gpio pin polarity
    Scope(\_SB.PC00.RP25) {
      Name(RSTG, Package() {0, 0})
      Store(SD3R, Index(RSTG, 0))
      Store(SDR3, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SD3P, Index(PWRG, 0))
      Store(SDP3, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 13)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP25.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PCH M.2 SSD3 RTD3
    ///
    //SD4P, 32,     // [PchM2Ssd3PowerEnableGpio           ] Pch M.2 SSD3 Power Enable Gpio pin
    //SDP4, 8,      // [PchM2Ssd3PowerEnableGpioPolarity   ] Pch M.2 SSD3 Power Enable Gpio pin polarity
    //SD4R, 32,     // [PchM2Ssd3RstGpio                   ] Pch M.2 SSD3 Reset Gpio pin
    //SDR4, 8,      // [PchM2Ssd3RstGpioPolarity           ] Pch M.2 SSD3 Reset Gpio pin polarity
    Scope(\_SB.PC00.RP01) {
      Name(RSTG, Package() {0, 0})
      Store(SD4R, Index(RSTG, 0))
      Store(SDR4, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SD4P, Index(PWRG, 0))
      Store(SDP4, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 10)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP01.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// CPU M.2 SSD RTD3
    ///
    //SD2P, 32,     // [M2Ssd2PowerEnableGpio             ] PCIe x4 M.2 SSD Power Enable Gpio pin
    //SDP1, 8,      // [M2Ssd2PowerEnableGpioPolarity     ] PCIe x4 M.2 SSD Power Enable Gpio pin polarity
    //SD2R, 32,     // [M2Ssd2RstGpio                     ] PCIe x4 M.2 SSD Reset Gpio pin
    //SDR1, 8,      // [M2Ssd2RstGpioPolarity             ] PCIe x4 M.2 SSD Reset Gpio pin polarity
    Scope(\_SB.PC00.PEG0) {
      Name(RSTG, Package() {0, 0})
      Store(SD2R, Index(RSTG, 0))
      Store(SDR1, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SD2P, Index(PWRG, 0))
      Store(SDP1, Index(PWRG, 1))
      Name(WAKG, 0)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 11)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.PEG0.PEGP) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }
    ///
    /// CPU PEG1
    ///
    //P1PE, 32,     // [PegSlot1PwrEnableGpioNo           ] PEG slot 1 Power Enable Gpio pin
    //P1PP, 8,      // [PegSlot1PwrEnableGpioPolarity     ] PEG slot 1 Power Enable Gpio pin polarity
    //P1RE, 32,     // [PegSlot1RstGpioNo                 ] PEG slot 1 Reset Gpio pin
    //P1RP, 8,      // [PegSlot1RstGpioPolarity           ] PEG slot 1 Reset Gpio pin polarity
    //P1WP, 32,     // [PegSlot1WakeGpioPin               ] PEG slot 1 Wake Gpio pin
    //PRP1, 8,      // [PegSlot1RootPort                  ] PEG slot 1 Root Port
    Scope(\_SB.PC00.PEG1) {
      Name(RSTG, Package() {0, 0})
      Store(P1RE, Index(RSTG, 0))
      Store(P1RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P1PE, Index(PWRG, 0))
      Store(P1PP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P1WP, WAKG)
      Name(WAKP, 0)
      Name(SCLK, 0)
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
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }
    ///
    /// CPU PEG2
    ///
    //P2PE, 32,     // [PegSlot2PwrEnableGpioNo           ] PEG slot 2 Power Enable Gpio pin
    //P2PP, 8,      // [PegSlot2PwrEnableGpioPolarity     ] PEG slot 2 Power Enable Gpio pin polarity
    //P2RE, 32,     // [PegSlot2RstGpioNo                 ] PEG slot 2 Reset Gpio pin
    //P2RP, 8,      // [PegSlot2RstGpioPolarity           ] PEG slot 2 Reset Gpio pin polarity
    //P2WP, 32,     // [PegSlot2WakeGpioPin               ] PEG slot 2 Wake Gpio pin
    //PRP2, 8,      // [PegSlot2RootPort                  ] PEG slot 2 Root Port

    //Check P2PP value here to distinquish if it's BoardIdAdlSAdpSDdr5UDimm1DCrb or
    //BoardIdAdlSAdpSDdr4UDimm2DCrb. Ddr5 board has No PEG2 Slot, so donot need handle RTD3
    if (LNotEqual(P2PP,0)) {
      Scope(\_SB.PC00.PEG2) {
      Name(RSTG, Package() {0, 0})
      Store(P2RE, Index(RSTG, 0))
      Store(P2RP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(P2PE, Index(PWRG, 0))
      Store(P2PP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P1WP, WAKG)
      Name(WAKP, 0)
      Name(SCLK, 1)
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
      Method(BCPR, 1, Serialized) {\PCPA(4, Arg0)}
      Include("PcieAuxPowerBudgeting.asl")
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }
  }
// PCIe root ports - END

//
// SATA - START
//
  Scope(\_SB.PC00.SAT0) {
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

    //Power Resource for Audio Codec
    Scope(\_SB.PC00)
    {
      PowerResource(PAUD, 0, 0) {
        /// Namespace variable used:
        Name(PSTA, One) /// PSTA: Physical Power Status of Codec 0 - OFF; 1-ON
        Name(ONTM, Zero) /// ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Name(_STA, One) /// _STA: PowerResource Logical Status 0 - OFF; 1-ON

        ///@defgroup pr_paud Power Resource for onboard Audio CODEC

        Method(_ON, 0){     /// _ON method \n
          Store(One, _STA)        ///- Set Logocal power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
                    ///@addtogroup pr_paud
        } // End _ON

        Method(_OFF, 0){    /// _OFF method \n
          Store(Zero, _STA)    ///- Set the current power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
        ///@addtogroup pr_paud
        } // End _OFF

        ///  PUAM - Power Resource User Absent Mode for onboard Audio CODEC
        ///  Arguments:
        ///
        ///  Uses:
        ///      _STA - Variable updated by Power Resource _ON/_OFF methods \n
        ///      \\UAMS - Variable updated by GUAM method to show User absent present \n
        ///      ONTM - Local variable to store ON time during Speculative ON \n
        /// ______________________________
        // |  Inputs      |   Outputs    |
        // ______________________________
        // | _STA | \UAMS | GPIO | ONTM |
        // ______________________________
        // |   1  |   0   | ON   |   !0 |
        // |   1  |   !0  | ON   |   !0 |
        // |   0  |   0   | ON   |   !0 |
        // |   0  |   !0  | OFF  |   0  |
        // ______________________________
                    /**
                    <table>
                    <tr> <th colspan="2"> Inputs <th colspan="2"> Output
                    <tr> <th>_STA <th> \\UAMS <th> GPIO <th>ONTM
                    <tr> <td>1 <td>0 <td>ON <td>!0
                    <tr> <td>1 <td>!0<td>ON <td>!0
                    <tr> <td>0 <td>0 <td>ON <td>!0
                    <tr> <td>0 <td>!0<td>OFF<td> 0
                    </table>
                    **/
        ///@addtogroup pr_paud_puam
        Method(PUAM, 0, Serialized)
        {
                // power rail = NOT there for ICL U
                // Note:- Audio Power enable need not be implemented by default and need rework if we need power control.
          If (LAnd(LEqual(^_STA, Zero), LNotEqual(\UAMS, Zero))) { ///New state = OFF Check if (_STA ==0 && \UAMS != 0) \n
          } Else { /// New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
            /// Turn power on \n
            If(LNotEqual(^PSTA, One)) { ///- Skip below if Power Resource is already in ON
              Store(One, ^PSTA)  ///- >> Set PSTA to 1
              Store(Timer(), ^ONTM) ///- >> Start the timer for this PR
            }
          }
        ///@defgroup pr_paud_puam Power Resource User Absent Mode for onboard Audio CODEC
        } //PUAM
      } //PAUD
    } //Scope(\_SB.PC00)

//
// Check HDAS (HD-Audio) controller present
//
    If(LNotEqual(\_SB.PC00.HDAS.VDID, 0xFFFFFFFF)) {
      Scope(\_SB.PC00.HDAS) {
        Method(PS0X,0,Serialized)     /// Platform D0 Method for HD-A Controller
        {
          If(LEqual(\_SB.PC00.PAUD.ONTM, Zero)){    ///- Check if ONTM=0
            Return()
          }

          ///
          ///- Make sure "D0 delay" (AUDD) delay is elapsed before returning _PS0
          ///- Local0: Elapse time since the _ON method
          ///- VRRD: VR Rampup Delay
          ///- AUDD: Time required for device to be ready after power on
          ///- Local1 = AUDD + VRRD: Need to incorporate VRRD since the _ON method no longer has VR Rampup Delay
          ///- So only need sleep for (Local1 - Local0), the amount of time remaining since the _ON method
          ///
          Divide(Subtract(Timer(), \_SB.PC00.PAUD.ONTM), 10000, , Local0) ///- Store Elapsed time in ms, ignore remainder
          Add(AUDD, VRRD, Local1) ///- Incorporate VR Rampup Delay
          If(LLess(Local0, Local1)) { ///- Do not sleep if already past the delay requirement audio
            ///- Delay for device init
            Sleep(Subtract(Local1, Local0)) ///- Sleep (AUDD + VRRD - time elapsed)
          }
        }

        ///Associate _PR0 with \ref pr_paud
        Name(_PR0, Package(){\_SB.PC00.PAUD})
      ///@defgroup hdef_scope       Intel High Definition Audio Scope
      }
    }// If(LNotEqual(\_SB.PC00.HDAS.VDID, 0xFFFFFFFF))

//GPE Event handling - Start
  Scope(\_GPE) {
    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      // X4 PCIe Connector (SLOT1) wake event
      If (\_SB.ISME(PSWP))
      {
        \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP21, 0x02)   // device wake
        \_SB.CAGS(PSWP)
      }

      //X4 PCIe Connector (SLOT2) wake event
      If (\_SB.ISME(PSW2))
      {
        \_SB.SHPO(PSW2, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP09, 0x02)     // device wake
        \_SB.CAGS(PSW2)    // Clear GPE event status
      }

      //X2 PCIe Connector (SLOT3) wake event
      If (\_SB.ISME(PSW3))
      {
        \_SB.SHPO(PSW3, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP05, 0x02)     // device wake
        \_SB.CAGS(PSW3)    // Clear GPE event status
      }

      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP08, 0x02)      // device wake
        \_SB.CAGS(WLWK)    // WIFI_WAKE_N
      }
    }
  } //Scope(\_GPE)
//GPE Event handling - End
} // End SSDT
