/**@file
  IPU IMG Clock Output Control
  Set of ACPI methods to enable, disable and select frequency of IMG Clock Output
  In Specs please refer to IMGCLKOUT_x where x is Clock Output index

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PchLimits.h>
#include <Register/PchRegsClk.h>

#define R_ICLK_PCR_CAMERA1                              0x8000
#define B_ICLK_PCR_FREQUENCY                            BIT0
#define B_ICLK_PCR_REQUEST                              BIT1

#define PID_ICLK                                0xAD

Scope(\_SB)
{
  Device (ICLK) {
    Name(_HID, EISAID("PNP0C02"))
    Name(_UID, "ISCLK")

    OperationRegion (CKOR, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ICLK_PCR_CAMERA1)), 0x40)
    Field(CKOR,AnyAcc,Lock,Preserve) {
      CLK0, 8,
      Offset(0x0C),
      CLK1, 8,
      Offset(0x18),
      CLK2, 8,
      Offset(0x24),
      CLK3, 8,
      Offset(0x30),
      CLK4, 8,
      Offset(0x3C),
      CLK5, 8,
    }

    //
    // Number Of Clocks
    //
    Method(NCLK, 0x0, NotSerialized)
    {
      If(LEqual(PCHS, PCHP)) {
        Return (6) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2, IMGCLKOUT_3, IMGCLKOUT_4, IMGCLKOUT_5
      } Else {
        Return (0) // ADL S does not have IMGCLKOUT GPIO pin
      }
    }

    //
    // Clock Control
    //
    Method (CLKC, 0x2, NotSerialized)
    {
      //
      // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
      // Arg1 - Desired state (0:Disable, 1:Enable)
      //
      If(LAnd (LLess(Arg0, NCLK()), LEqual(PCHS, PCHP)))
      {
        Switch(ToInteger(Arg0)) {
          Case (0) {
            Store(CLK0, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK0)
          }
          Case (1) {
            Store(CLK1, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK1)
          }
          Case (2) {
            Store(CLK2, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK2)
          }
          Case (3) {
            Store(CLK3, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK3)
          }
          Case (4) {
            Store(CLK4, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK4)
          }
          Case (5) {
            Store(CLK5, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK5)
          }
        }
      }
    }

    //
    // Clock Frequency
    //
    Method (CLKF, 0x2, NotSerialized)
    {
      //
      // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
      // Arg1 - Clock frequency (0:24MHz, 1:19.2MHz)
      //
      If(LAnd (LLess(Arg0, NCLK()), LEqual(PCHS, PCHP)))
      {
        Switch(ToInteger(Arg0)) {
          Case (0) {
            Store(CLK0, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK0)
          }
          Case (1) {
            Store(CLK1, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK1)
          }
          Case (2) {
            Store(CLK2, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK2)
          }
          Case (3) {
            Store(CLK3, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK3)
          }
          Case (4) {
            Store(CLK4, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK4)
          }
          Case (5) {
            Store(CLK5, Local0)
            Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK5)
          }
        }
      }
    }

    Method (_STA)
    {
      // Device is present, enabled and decoding it's resources
      // and should not be shown in UI
      Return (0x3)
    }
  }
}

//
// PCIe clock runtime control methods.
//
Scope (\) {
  //
  // Modify clock using IPC message
  // Arg0: Clock number
  // Arg1: Enable(1)/Disable(0) Clock
  //
  Method (MCUI, 2, Serialized) {
    Name(PRPI, 0) // PCIe root port index
    ShiftLeft(0x1, Arg0, Local0)
    ShiftLeft(Arg1, Arg0, Local1)
    Store(CTRP(Arg0), PRPI)
    If(LLess (PRPI, PCH_MAX_PCIE_ROOT_PORTS)) {
      ShiftLeft(0x1, PRPI, Local2)
      ShiftLeft(Arg1, PRPI, Local3)
    } Else {
      //
      // Todo: CPU Code needs to be updated. As Clock usage value for CPU is greater than
      // or equal to 0.
      //
      ShiftLeft(0x1, PRPI, Local2)
      ShiftLeft(Arg1, PRPI, Local3)
      //
      // Feature HAS - IPC1 0xAC command update for RTD3 flows v0p60.docx says
      // Note that bits 23:0 apply to the PCH clocks and bits 31:24 apply to the CPU clocks.
      // So left shift by 24 bits for CPU PCIe
      //
      ShiftLeft(Local2, 24, Local2)
      ShiftLeft(Local3, 24, Local3)
    }
    //
    // PMC IPC command to disale PCIe source clock
    // WBUF0 - PCIe source clock mask. Each set bit in this mask tells
    //         PMC FW to act on this clock according to WBUF1 settings
    // WBUF1 - PCIe source clock enable/disable. Each bit represents the
    //         target state of the source clock
    // WBUF2 - PCIe root port CLKREQ mapping mask. Each set bit in this mask
    //         tells PMC FW to act on this root port accordingly to WBUF3 settings
    // WBUF3 - PCIe root port CLKREQ mapping state mask. Each bit represents the
    //         target state of the PCIe root port CLKREQ mapping.
    //
    ADBG("Calling IPCS method with command V_PMC_PWRM_IPC_SRC_CLK_PORT_MAPPING_CMD")
    ADBG(Concatenate("Local0 =", Local0))
    ADBG(Concatenate("Local1 =", Local1))
    ADBG(Concatenate("Local2 =", Local2))
    ADBG(Concatenate("Local3 =", Local3))
    IPCS(V_PMC_PWRM_IPC_SRC_CLK_PORT_MAPPING_CMD, 0, 16, Local0, Local1, Local2, Local3)
  }

  //
  // Configure PCIe ClkReq Override
  // Arg0: Clock number
  // Arg1: Enable(1)/Disable(0) Clock
  //
  Method (SPCO, 2, Serialized) {
    ADBG("Calling SPCO method to configure PCIe ClkReq Override")
    Name(CLKU, 0) // CLK REQ Usage
    If (LNotEqual(IPCC,0)) {
      TMVA (10)
      MCUI(Arg0, Arg1)
    } Else {
      Store(CTRP(Arg0), CLKU)
      If (LEqual(Arg1,1)) {
        //
        // Unmask the isCLK DBUFF_SRC
        //
        If(LGreater(Arg0, 6)) {
          ADD(Arg0,1, Local1)
          Subtract(Local1, 7, Local1)
          PCRO(PID_ICLK, R_ICLK_PCR_BIOS_BUFFEN_H,(ShiftLeft(1, Local1)))
        } Else {
          ADD(Arg0,2, Local1)
          PCRO(PID_ICLK, R_ICLK_PCR_BIOS_BUFFEN,(ShiftLeft(1, Local1)))
        }
      } Else {
        //
        // Disable the Clock
        // Mask the isCLK DBUFF_SRC
        //
        If(LGreater(Arg0, 6)) {
          ADD(Arg0, 1, Local1)
          Subtract(Local1, 7, Local1)
          PCRA(PID_ICLK, R_ICLK_PCR_BIOS_BUFFEN_H, Not(ShiftLeft(1, Local1)))
        } Else {
          ADD(Arg0, 2, Local1)
          PCRA(PID_ICLK, R_ICLK_PCR_BIOS_BUFFEN, Not(ShiftLeft(1, Local1)))
        }
      }
    }
  }

  //
  // Configure PCIe ClkReq and IPC1 Command Timeout Override
  // Arg0: Clock number
  // Arg1: Enable(1)/Disable(0) Clock
  // Arg2: Timeout Value Override (<=10ms: Keep default setting, >10ms: Using customize timeout value)
  //
  Method (SPCX, 3, Serialized) {
    ADBG ("Calling SPCX method to configure PCIe ClkReq Override")
    If (LNotEqual(IPCC,0)) {
      ADBG ("Calling TMVA method to override IPC1 Command Timeout Value")
      TMVA (Arg2) // Configuring/Overriding IPC1 Command Timeout Value
      MCUI (Arg0, Arg1)
    } Else {
      SPCO (Arg0, Arg1)
    }
  }

  //
  // WA to Mask/UnMask Hybrid Partner CLKREQ using IPC message
  // Arg0: Hybrid device partner port number
  // Arg1: UnMask(1)/Mask(0) Clock
  //
  Method (HBCM, 2, Serialized) {
    ADBG("mask/unmask Hybrid Partner CLKREQ")
    Name(HPRI, 0) // Hybrid Partner root port index
    Store(Arg0, HPRI)
    If(LLess (HPRI, PCH_MAX_PCIE_ROOT_PORTS)) {
      ShiftLeft(0x1, HPRI, Local0)
      ShiftLeft(Arg1, HPRI, Local1)
    }
    Else {
      Store(0, Local0)
      Store(0, Local1)
    }

    ADBG("Calling IPCS method for Hybrid Partner with command V_PMC_PWRM_IPC_SRC_CLK_PORT_MAPPING_CMD")
    ADBG(Concatenate("Port number of Hybrid Partner =", HPRI))
    ADBG(Concatenate("Local0 of Hybrid Partner =", Local0))
    ADBG(Concatenate("Local1 of Hybrid Partner =", Local1))
    IPCS(V_PMC_PWRM_IPC_SRC_CLK_PORT_MAPPING_CMD, 0, 16, 0, 0, Local0, Local1)
  }

  //
  // Clock index to root port number method
  // Arg0: Clock index
  //
  Method(CTRP, 1) {
    Switch(ToInteger(Arg0)) {
      Case(0) {Return(CR00)}
      Case(1) {Return(CR01)}
      Case(2) {Return(CR02)}
      Case(3) {Return(CR03)}
      Case(4) {Return(CR04)}
      Case(5) {Return(CR05)}
      Case(6) {Return(CR06)}
      Case(7) {Return(CR07)}
      Case(8) {Return(CR08)}
      Case(9) {Return(CR09)}
      Case(10) {Return(CR10)}
      Case(11) {Return(CR11)}
      Case(12) {Return(CR12)}
      Case(13) {Return(CR13)}
      Case(14) {Return(CR14)}
      Case(15) {Return(CR15)}
      Case(16) {Return(CR16)}
      Case(17) {Return(CR17)}
      Default {Return(0xFF)}
    }
  }

  //
  // Clock Source index to ClkReq number method
  // Arg0: Clock index
  //
  Method(CTRQ, 1) {
    Switch(ToInteger(Arg0)) {
      Case(0) {Return(CQ00)}
      Case(1) {Return(CQ01)}
      Case(2) {Return(CQ02)}
      Case(3) {Return(CQ03)}
      Case(4) {Return(CQ04)}
      Case(5) {Return(CQ05)}
      Case(6) {Return(CQ06)}
      Case(7) {Return(CQ07)}
      Case(8) {Return(CQ08)}
      Case(9) {Return(CQ09)}
      Case(10) {Return(CQ10)}
      Case(11) {Return(CQ11)}
      Case(12) {Return(CQ12)}
      Case(13) {Return(CQ13)}
      Case(14) {Return(CQ14)}
      Case(15) {Return(CQ15)}
      Case(16) {Return(CQ16)}
      Case(17) {Return(CQ17)}
      Default {Return(0xFF)}
    }
  }
}
