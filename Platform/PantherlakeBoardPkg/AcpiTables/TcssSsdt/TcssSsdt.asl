/** @file
  This file contains the TCSS SSDT Table ASL code.
  It defines a SSDT table for TCSS

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Register/PchPcieRpRegs.h>
#include <Register/UsbHostRegs.h>
#include <Register/UsbDeviceRegs.h>
#include <Register/Usb4Regs.h>
#include <Register/ItbtPcieRegs.h>
#include <Register/TcPcieRpRegs.h>
#include <Register/IomRegs.h>
#include <Register/PmcRegs.h>
#include <TbtMailBoxCmdDefinition.h>
#include <PcieRpMacroDefinition.h>
#include <Include/AcpiDebug.h>
#include <Register/HostDmaRegs.h>
#include <Register/PcieSipRegs.h>

//
// MAILBOX_BIOS_CMD_TCSS_DEVEN_INTERFACE
// Command code 0x15
// Description: Gateway command for handling TCSS DEVEN clear/restore.
// Field PARAM1[15:8] of the _INTERFACE register is used in this command to select from a pre-defined set of subcommands:
//
#define MAILBOX_BIOS_CMD_TCSS_DEVEN_INTERFACE     0x00000015
// Sub-Command 0
#define TCSS_DEVEN_MAILBOX_SUBCMD_GET_STATUS      0
// Sub-Command 1
#define TCSS_DEVEN_MAILBOX_SUBCMD_TCSS_CHANGE_REQ 1

#define TCSS_IOM_ACK_TIMEOUT_IN_MS      100

#define TCSS_ITBT_PCIE0_RP0             0
#define TCSS_ITBT_PCIE0_RP1             1
#define TCSS_ITBT_PCIE0_RP2             2
#define TCSS_ITBT_PCIE0_RP3             3
#define TCSS_XHCI                       4
#define TCSS_DMA0                       6
#define TCSS_DMA1                       7
#define TCSS_ITBT_PCIE1_RP0             8
#define TCSS_ITBT_PCIE1_RP1             9
#define TCSS_ITBT_PCIE1_RP2             10
#define TCSS_ITBT_PCIE1_RP3             11

DefinitionBlock (
  "TcssSsdt.aml",
  "SSDT",
  2,
  "INTEL ",
  "TcssSsdt",
  0x1000
  )
{
External (\_SB.PC00, DeviceObj)
External (\GPRW, MethodObj)
External (\ECR1)
ACPI_DEBUG_EXTERNAL_REFERENCE
External (\_SB.PC00.GMHB, MethodObj)
External (PCIC, MethodObj)
External (PCID, MethodObj)
External (\_SB.PC00.GPCB, MethodObj)
External (\_SB.PC00.GFX0, DeviceObj)
External (\_SB.PC00.GFX0.GFPS, MethodObj)

External (THCE) // TCSS XHCI Device Enable
External (DME0) // TCSS DMA 0 Device Enable
External (DME1) // TCSS DMA 1 Device Enable
External (TRE0) // TCSS ItbtPcie PCIE RP 0 Device Enable
External (TRE1) // TCSS ItbtPcie PCIE RP 1 Device Enable
External (TRE2) // TCSS ItbtPcie PCIE RP 2 Device Enable
External (TRE3) // TCSS ItbtPcie PCIE RP 3 Device Enable
External (TPA0) // TCSS ItbtPcie PCIE RP 0 Address
External (TPA1) // TCSS ItbtPcie PCIE RP 1 Address
External (TPA2) // TCSS ItbtPcie PCIE RP 2 Address
External (TPA3) // TCSS ItbtPcie PCIE RP 3 Address
External (TRTD) // TCSS RTD3
External (IMRY) // IOM Ready
External (TIVS) // TCSS IOM VccSt
External (REGO) // MCHBAR Regbar Offset
External (IOMB) // IOM Reg Bar Base
External (RTBM) // USBC Retimer Mapping

External (LTE0) // Latency Tolerance Reporting Mechanism Enable 0
External (LTE1) // Latency Tolerance Reporting Mechanism Enable 1
External (LTE2) // Latency Tolerance Reporting Mechanism Enable 2
External (LTE3) // Latency Tolerance Reporting Mechanism Enable 3
External (PSL0) // PCIE LTR max snoop Latency 0
External (PSL1) // PCIE LTR max snoop Latency 1
External (PSL2) // PCIE LTR max snoop Latency 2
External (PSL3) // PCIE LTR max snoop Latency 3
External (PNS0) // PCIE LTR max no snoop Latency 0
External (PNS1) // PCIE LTR max no snoop Latency 1
External (PNS2) // PCIE LTR max no snoop Latency 2
External (PNS3) // PCIE LTR max no snoop Latency 3

External (\U4CM, IntObj)
External (\GP1E, IntObj)
External(\_SB.PC00.TDM0.SCME)
External(\_SB.PC00.TDM1.SCME)

Include ("ITbt.asl")

Scope (\_SB) {

  Name (C2PW, 0) // Set Default value 0 to Tcss CPU to PCH WAKE Value

  // C2PM (CPU to PCH Method)
  //
  // This object is Enable/Disable GPE_CPU_WAKE_EN.
  //
  // Arguments: (4)
  //   Arg0 - An Integer containing the device wake capability
  //   Arg1 - An Integer containing the target system state
  //   Arg2 - An Integer containing the target device state
  //   Arg3 - An Integer containing the request device type
  // Return Value:
  //   return 0
  //
  Method (C2PM, 4, NotSerialized) {

    Switch (ToInteger (Arg3)) {
      Case (TCSS_ITBT_PCIE0_RP0) {
        Store ("Tcss iTbt Pcie 0 RP 0", Local0)
      }
      Case (TCSS_ITBT_PCIE0_RP1) {
        Store ("Tcss iTbt Pcie 0 RP 1", Local0)
      }
      Case (TCSS_ITBT_PCIE0_RP2) {
        Store ("Tcss iTbt Pcie 0 RP 2", Local0)
      }
      Case (TCSS_ITBT_PCIE0_RP3) {
        Store ("Tcss iTbt Pcie 0 RP 3", Local0)
      }
      Case (TCSS_XHCI) {
        Store ("Tcss xHci", Local0)
      }
      Case (TCSS_DMA0) {
        Store ("Tcss Dma 0", Local0)
      }
      Case (TCSS_DMA1) {
        Store ("Tcss Dma 1", Local0)
      }
      Case (TCSS_ITBT_PCIE1_RP0) {
        Store ("Tcss iTbt Pcie 1 RP 0", Local0)
      }
      Case (TCSS_ITBT_PCIE1_RP1) {
        Store ("Tcss iTbt Pcie 1 RP 1", Local0)
      }
      Case (TCSS_ITBT_PCIE1_RP2) {
        Store ("Tcss iTbt Pcie 1 RP 2", Local0)
      }
      Case (TCSS_ITBT_PCIE1_RP3) {
        Store ("Tcss iTbt Pcie 1 RP 3", Local0)
      }
      Default {Return (0)}
    } // End of Switch

    Store (0, Local1)
    ShiftLeft (BIT0, ToInteger (Arg3), Local1)
    ADBG (Concatenate ("Local1 = ", ToHexString (Local1)))

    ADBG (Concatenate ("Before CPU to PCH Wake capability configuration Start from ", Local0))
    ADBG (Concatenate ( Local0, Concatenate (" Cpu To Pch Wake Value = ", ToHexString (C2PW))))

    /// This method is used to enable/disable wake from Tcss Device (WKEN)
    ADBG (Concatenate ("C2PM ", Local0))
    If (LAnd (Arg0, Arg1))
    { /// If entering Sx and enabling wake, need to enable WAKE capability
      If (LEqual (And (C2PW, Local1), 0)) {
        Or (C2PW, Local1,C2PW) // Set Corresponding Device En BIT in C2PW
      }
      ADBG (Concatenate (Local0, " Sx EnWak"))
    } Else { /// If Staying in S0 or Disabling Wake
      If (LOr (Arg0, Arg2)) /// Check if Exiting D0 and arming for wake
      {
        If (LEqual (And (C2PW, Local1), 0)) {
          Or (C2PW, Local1,C2PW) // Set Corresponding Device En BIT in C2PW
        }
        ADBG (Concatenate (Local0, " D3 En Wak"))
      } Else { /// Disable runtime PME, either because staying in D0 or disabling wake
        If (LNotEqual (And (C2PW, Local1), 0)) {
          And (C2PW, Not (Local1),C2PW) // Clear Corresponding Device En BIT in C2PW
        }
        ADBG (Concatenate (Local0, " DisWak"))
      }
    }

    ADBG (Concatenate ("After CPU to PCH Wake capability configuration End from ", Local0))
    ADBG (Concatenate ( Local0, Concatenate (" Cpu To Pch Wake Value = ", ToHexString (C2PW))))

    Store (0, Local0) //dummy operation to fix warning error
    Return (Local0)
  } // End of C2PM

  //
  // TCSS iTBT Wake notify
  //
  Method (TCWK, 1) {
    If (LOr (LEqual (Arg0,3), LEqual (Arg0,4))) {
      If (LNotEqual (\_SB.PC00.TRP0.VDID,0xFFFFFFFF))
      {
        Notify (\_SB.PC00.TRP0,0)
      }
      If (LNotEqual (\_SB.PC00.TRP1.VDID,0xFFFFFFFF))
      {
        Notify (\_SB.PC00.TRP1,0)
      }
      If (LNotEqual (\_SB.PC00.TRP2.VDID,0xFFFFFFFF))
      {
        Notify (\_SB.PC00.TRP2,0)
      }
      If (LNotEqual (\_SB.PC00.TRP3.VDID,0xFFFFFFFF))
      {
        Notify (\_SB.PC00.TRP3,0)
      }
    }
  } // End of TCWK
} // End of Scope (\_SB)

Scope (\_SB.PC00) {

  //
  // Operation region defined to access the IOM REGBAR
  // Get the MCHBAR in offset 0x48 in B0:D0:F0
  // REGBAR Base address is in offset REGO's value of MCHBAR.
  //
  OperationRegion (MBAR, SystemMemory, Add (\_SB.PC00.GMHB (), REGO), 0x1000)
  Field (MBAR, ByteAcc, NoLock, Preserve)
  {
    RBAR, 64       //RegBar, offset REGO's value in MCHBAR
  }

  OperationRegion (PBAR, SystemMemory, Add (\_SB.PC00.GMHB (), 0x5DA0), 0x08)
  Field (PBAR, DWordAcc, NoLock, Preserve)
  {
    PMBD, 32,       // pCode MailBox Data, offset 0x5DA0 in MCHBAR
    PMBC, 8,        // pCode MailBox Command, [7:0] of offset 0x5DA4 in MCHBAR
    PSCM, 8,        // pCode MailBox Sub-Command, [15:8] of offset 0x5DA4 in MCHBAR
    ,     15,       // Reserved
    PMBR, 1         // pCode MailBox RunBit,  [31:31] of offset 0x5DA4 in MCHBAR
  }

  //
  // Poll pCode MailBox Ready
  //
  // Return 0xFF - Timeout
  //        0x00 - Ready
  //
  Method (PMBY, 0)
  {
    Store (0, Local0)
    While (LAnd (PMBR, LLess (Local0, 1000))) {
      Increment (Local0)
      Stall (1)
    }
    If (LEqual (Local0, 1000)) {
      //
      // Timeout occurred
      //
      Return (0xFF)
    }
    Return (0)
  }

  //
  // From RegBar Base, IOM_TypeC_SW_configuration_1 is in offset 0xC10040, where 0x07(in A0) is the IOM port
  // id and 0x0040 is the register offset
  //
  OperationRegion (IOMR, SystemMemory, IOMB, 0x100)
  Field (IOMR, DWordAcc, NoLock, Preserve)
  {
    Offset (R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_1),
    ,     15,
    TD3C, 1,          // [15:15] Type C D3 cold bit
    TACK, 1,          // [16:16] IOM Acknowledge bit
    DPOF, 1,          // [17:17] Set 1 to indicate IOM, all the display is OFF, clear otherwise
    ,     2,
    Offset (R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_2),    // SW_CONFIG_2
    DPHD, 1,          // [0:0] DP_ALT and DP Tunneling HPD assertion.
    Offset (R_IOM_PCR_IOM_TYPEC_SW_CONFIGURATION_3),    // SW_CONFIG_3
    ,     12,
    INDP, 1,          // [12:12] The capability of monitoring the DP_ALT and DP tunneling.
    Offset (R_IOM_PCR_IOM_BIOS_MAILBOX_CMD),    // Pyhical addr is offset 0x70.
    IMCD, 32,         // R_SA_IOM_BIOS_MAIL_BOX_CMD
    IMDA, 32,         // R_SA_IOM_BIOS_MAIL_BOX_DATA
  }

  // Notify iGfx when HPD_ASSERT with DP_ALT/DP tunneling.
  Name (IGFG, 0) // Internal Monitor On/Off Flag. 1 - Off, 0 - On
  Method (IMNG, 1)
  {
    Switch (ToInteger (Arg0)) {
      Case (3) {
        // Clear IOM_TYPEC_SW_CONFIGURATION_2 BIT0 to 0 while BIOS detect BIT0 is 1
        If (LEqual (INDP, 1)) {
          Store (1, IGFG) // Set Internal Monitor off flag for for skipping notification to iGfx after PEP Fun#3 Display off notification.
        }
        If (LEqual (DPHD, 1)) {
          Store (0, DPHD) // Clear IOM HPD indication.
        }
        //Return
      }
      Case (4) {
        // Clear IOM_TYPEC_SW_CONFIGURATION_2 BIT0 to 0 and notify IGD for device scan
        If (LEqual (INDP, 1)) {
          Store (0, IGFG) // Clear the internal monitor off flag for enabling SL71 notification to iGfx when iGfx is in D3 hot.
        }
        If (LEqual (DPHD, 1)) {
          Store (0, DPHD) // Clear IOM HPD indication.
        }
        //Return
      }

      // _L71 using condition
      Case (113) { // HEX 0x71 == DEC 113
        Local0 = 0xF
        If (CondRefOf (\_SB.PC00.GFX0.GFPS)) {
          Store (\_SB.PC00.GFX0.GFPS (), Local0)
        }
        If (LEqual (IGFG, 0)) {
          If (LAnd (LEqual (INDP, 1), LEqual(Local0, 3))) { // Notify iGfx when the capability bit of monitoring DP_ALT & DP tunneling is set and iGfx is in D3 hot.
            // 0 = Re-enumeration
            // 0x80 = Display switch
            ADBG (" Notify _SB.PC00.GFX0 ")
            Notify (\_SB.PC00.GFX0, 0)
          }
        }
      } // Case 113 end
    } // switch end
  }

  //
  // IMMD -> Iom Mailbox data
  // Arg0 : Set value to data register
  //
  Method (IMMD,1)
  {
    Store (Arg0, IMDA)
    ADBG (Concatenate ("IMMD :: ", ToHexString (IMDA)))
    Return (0)
  }

  //
  // IMMC -> IOM MailBox Command
  //
  // Arg0 : Command Type
  // Arg1 : Timeout for waiting IOM's response, The unit is in ms.
  // Arg2 : Param1
  // Arg3 : Param2
  // Arg4 : Data Field
  //
  Method (IMMC,5)
  {

    Name (OPTS, Buffer (4) {0,0,0,0})
    CreateByteField (OPTS, 0x00, CMST) // Command Status field
                                       //         Success - 0
                                       //         Fail    - 1
    CreateByteField (OPTS, 0x01, RTB1) // Return Buffer 1

    //
    // Set Data register
    //
    IMMD (Arg4)

    //
    // Execute Command
    //
    Store (Arg3, Local1) // Set Param2 shift to [23:16]
    ShiftLeft (Local1, 8, Local1)
    Add (Local1, Arg2, Local1)
    ShiftLeft (Local1, 8, Local1)
    Add (Local1, Arg0, Local1)
    Store (Local1, IMCD) // Set command register

    ADBG (Concatenate ("Set IMMC Command ", ToHexString (IMCD)))
    Or (IMCD, BIT31, IMCD)
    ADBG (Concatenate ("Set IMMC Run Bit ", ToHexString (IMCD)))
    Store (Arg1, Local0) // Wait IOM response
    ADBG (Concatenate ("IMMC Delay ", ToHexString (Local0)))
    While (LAnd (LNotEqual (And (IMCD, BIT31), 0), LNotEqual (Local0, 0))) {
      Sleep (1)
      Decrement (Local0)
    }

    // Return Status
    Switch (ToInteger (Arg0)) {
      Case (V_IOM_BIOS_GEM_SB_TRAN_CMD)
      {
        ADBG ("GEM_SB_TRAN_CMD")
        Return (0)
      }
      Case (V_IOM_BIOS_MBX_GET_HPD_COUNT)
      {
        ADBG ("IOM_BIOS_MBX_GET_HPD_COUNT")
        if (LNotEqual (And (IMCD, BIT31), 0)) {
          ADBG ("Timeout!")
          ADBG (Concatenate ("IOM MB CMD Reg ", ToHexString (IMCD)))
          ADBG (Concatenate ("IOM MB DATA Reg ", ToHexString (IMDA)))
          Store (1, CMST)
          Return (OPTS)
        } else {
          if (LNotEqual (And (IMCD, 0xFF), 0)) {
            ADBG ("Completion Code Fail")
            ADBG (Concatenate ("IOM MB CMD Reg ", ToHexString (IMCD)))
            Store (1, CMST)
          } else {
            ADBG ("Passed")
            ADBG (Concatenate ("IOM MB CMD Reg :: ", ToHexString (IMCD)))
            ADBG (Concatenate ("IOM MB DATA Reg :: ", ToHexString (IMDA)))
            ADBG (Concatenate ("Arg2 is Port Number :: ", ToHexString (Arg2)))
            Store (0, CMST)
            Store (IMDA, RTB1)
          }
        }
        Return (OPTS)
      }
      Case (V_IOM_BIOS_MBX_DEC_HPD_COUNT)
      {
        ADBG ("BIOS_MBX_DEC_HPD_COUNT")
        Store (0, RTB1)
        if (LNotEqual (And (IMCD, BIT31), 0)) {
          ADBG ("Timeout!")
          Store (1, CMST)
        } else {
          if (LNotEqual (And (IMCD, 0xFF), 0)) {
            ADBG ("Completion Code Fail")
            ADBG (Concatenate ("IOM MB CMD Reg ", ToHexString (IMCD)))
            Store (1, CMST)
            Store (IMDA, RTB1)
          } else {
            ADBG ("Passed")
            Store (0, CMST)
          }
        }
        Return (OPTS)
      }

      Default {
        Store (1, CMST)
        Return (OPTS)  // Error invalid argument
      }
    }
  } // End of IMMC

  //
  // Below is a variable to store devices connect state for TBT PCIe RP before enter D3 cold
  //
  // Value 0 - no device connected before enter D3 cold
  //           no need to send CONNECT_TOPOLOGY in D3 cold exit
  //       1 - has device connected before enter D3 cold
  //           need to send CONNECT_TOPOLOGY in D3 cold exit.
  //
  Name (CTP0, 0) // Variable of device connecet status for TBT0 group
  Name (CTP1, 0) // Variable of device connecet status for TBT1 group

  //
  // TBT Group0 ON method
  //
  Method (TG0N, 0)
  {
    ADBG ("TG0N Start")

    ADBG (Concatenate ("DMA0 VDID -", ToHexString (\_SB.PC00.TDM0.VDID)))
    ADBG (Concatenate ("DMA0 PMST -", ToHexString (\_SB.PC00.TDM0.PMST)))
    ADBG (Concatenate ("DMA0 PMEE -", ToHexString (\_SB.PC00.TDM0.PMEE)))
    ADBG (Concatenate ("DMA0 PMES -", ToHexString (\_SB.PC00.TDM0.PMES)))
    ADBG (Concatenate ("DMA0 STAT -", ToHexString (\_SB.PC00.TDM0.STAT)))


    If (LNotEqual (\_SB.PC00.TDM0.VDID,0xFFFFFFFF)) {                      // DMA0 exists
      If (LEqual (\_SB.PC00.TDM0.STAT, 0)) {                               // DMA0 is in D3Cold early
        \_SB.PC00.TDM0.D3CX ()                                             // RTD3 Exit
        ADBG ("Let's bring TBT RPs out of D3Cold")
        If (LNotEqual (\_SB.PC00.TRP0.VDID, 0xFFFFFFFF)) {
          //\_SB.PC00.TRP0.D3CX ()                                         // RP0 D3 cold exit
        }
        If (LNotEqual (\_SB.PC00.TRP1.VDID, 0xFFFFFFFF)) {
          \_SB.PC00.TRP1.D3CX ()                                         // RP1 D3 cold exit
        }
        //
        // Need to send Connect-Topology command when TBT host controller back to D0 from D3
        //
        If (LEqual (\_SB.PC00.TDM0.ALCT, 1)) {
          If (LEqual (CTP0, 1)) {
            \_SB.PC00.TDM0.CNTP ()          // Send Connect-Topology command if there is device present on PCIe RP
            Store (1, \_SB.PC00.TDM0.WACT)  // Indicate to wait Connect-Topology cmd
            Store (0, CTP0)                 // Clear the connect states
          }
          Store (0, \_SB.PC00.TDM0.ALCT)    // Disallow to send Connect-Topology cmd
        }
      } Else {
        ADBG ("Drop TG0N due to it is already exit D3 cold")
      }
      Sleep (ITRE)
    }
    ADBG ("TG0N End")
  } // End of TG0N

  //
  // TBT Group0 OFF method
  //
  Method (TG0F, 0)
  {
    ADBG ("TG0F Start")

    ADBG (Concatenate ("DMA0 VDID -", ToHexString (\_SB.PC00.TDM0.VDID)))
    ADBG (Concatenate ("DMA0 PMST -", ToHexString (\_SB.PC00.TDM0.PMST)))
    ADBG (Concatenate ("DMA0 PMEE -", ToHexString (\_SB.PC00.TDM0.PMEE)))
    ADBG (Concatenate ("DMA0 PMES -", ToHexString (\_SB.PC00.TDM0.PMES)))
    ADBG (Concatenate ("DMA0 STAT -", ToHexString (\_SB.PC00.TDM0.STAT)))

    If (LNotEqual (\_SB.PC00.TDM0.VDID,0xFFFFFFFF)) {                       // DMA0 exists
      If (LEqual (\_SB.PC00.TDM0.STAT, 1)) {                                // DMA0 is in not D3Cold now
        \_SB.PC00.TDM0.D3CE ()                                             // Enable DMA RTD3
        ADBG ("Let's push TBT RPs to D3Cold together")
        If (LNotEqual (\_SB.PC00.TRP0.VDID, 0xFFFFFFFF)) {
          ADBG (Concatenate ("PC00.TRP0.PDSX -", ToHexString (\_SB.PC00.TRP0.PDSX)))
          If (LEqual (\_SB.PC00.TRP0.PDSX, 1)) {
            Store (1, CTP0)
          }
          \_SB.PC00.TRP0.D3CE () // Put RP0 to D3 cold
        }
        If (LNotEqual (\_SB.PC00.TRP1.VDID, 0xFFFFFFFF)) {
          ADBG (Concatenate ("PC00.TRP1.PDSX -", ToHexString (\_SB.PC00.TRP1.PDSX)))
          If (LEqual (\_SB.PC00.TRP1.PDSX, 1)) {
            Store (1, CTP0)
          }
          \_SB.PC00.TRP1.D3CE () // Put RP1 to D3 cold
        }
      }
    }
    ADBG ("TG0F End")
  } // End of TG0F

  //
  // TBT Group1 ON method
  //
  Method (TG1N, 0)
  {
    ADBG ("TG1N Start")

    ADBG (Concatenate ("DMA1 VDID -", ToHexString (\_SB.PC00.TDM1.VDID)))
    ADBG (Concatenate ("DMA1 PMST -", ToHexString (\_SB.PC00.TDM1.PMST)))
    ADBG (Concatenate ("DMA1 PMEE -", ToHexString (\_SB.PC00.TDM1.PMEE)))
    ADBG (Concatenate ("DMA1 PMES -", ToHexString (\_SB.PC00.TDM1.PMES)))
    ADBG (Concatenate ("DMA1 STAT -", ToHexString (\_SB.PC00.TDM1.STAT)))


    If (LNotEqual (\_SB.PC00.TDM1.VDID,0xFFFFFFFF)) {                      // DMA1 exists
      If (LEqual (\_SB.PC00.TDM1.STAT, 0)) {                               // DMA1 is in D3Cold early
        \_SB.PC00.TDM1.D3CX ()                                             // RTD3 Exit
        ADBG ("Let's bring TBT RPs out of D3Cold")
        If (LNotEqual (\_SB.PC00.TRP2.VDID, 0xFFFFFFFF)) {
          //\_SB.PC00.TRP2.D3CX ()                                         // RP2 D3 cold exit
        }
        If (LNotEqual (\_SB.PC00.TRP3.VDID, 0xFFFFFFFF)) {
          \_SB.PC00.TRP3.D3CX ()                                         // RP3 D3 cold exit
        }
        //
        // Need to send Connect-Topology command when TBT host controller back to D0 from D3
        //
        If (LEqual (\_SB.PC00.TDM1.ALCT, 1)) {
          If (LEqual (CTP1, 1)) {
            \_SB.PC00.TDM1.CNTP ()          // Send Connect-Topology command if there is device present on PCIe RP
            Store (1, \_SB.PC00.TDM1.WACT)  // Indicate to wait Connect-Topology cmd
            Store (0, CTP1)                 // Clear the connect states
          }
          Store (0, \_SB.PC00.TDM1.ALCT)    // Disallow to send Connect-Topology cmd
        }
      } Else {
        ADBG ("Drop TG1N due to it is already exit D3 cold")
      }
      Sleep (ITRE)
    }
    ADBG ("TG1N End")
  } // End of TG1N

  //
  // TBT Group1 OFF method
  //
  Method (TG1F, 0)
  {
    ADBG ("TG1F Start")

    ADBG (Concatenate ("DMA1 VDID -", ToHexString (\_SB.PC00.TDM1.VDID)))
    ADBG (Concatenate ("DMA1 PMST -", ToHexString (\_SB.PC00.TDM1.PMST)))
    ADBG (Concatenate ("DMA1 PMEE -", ToHexString (\_SB.PC00.TDM1.PMEE)))
    ADBG (Concatenate ("DMA1 PMES -", ToHexString (\_SB.PC00.TDM1.PMES)))
    ADBG (Concatenate ("DMA1 STAT -", ToHexString (\_SB.PC00.TDM1.STAT)))

    If (LNotEqual (\_SB.PC00.TDM1.VDID,0xFFFFFFFF)) {                      // DMA1 exists

      If (LEqual (\_SB.PC00.TDM1.STAT, 1)) {                               // DMA1 is in not D3Cold now
        \_SB.PC00.TDM1.D3CE ()                                           // Enable DMA RTD3
        ADBG ("Let's push TBT RPs to D3Cold together")
        If (LNotEqual (\_SB.PC00.TRP2.VDID, 0xFFFFFFFF)) {
          ADBG (Concatenate ("PC00.TRP2.PDSX -", ToHexString (\_SB.PC00.TRP2.PDSX)))
          If (LEqual (\_SB.PC00.TRP2.PDSX, 1)) {
            Store (1, CTP1)
          }
          \_SB.PC00.TRP2.D3CE () // Put RP2 to D3 cold
        }
        If (LNotEqual (\_SB.PC00.TRP3.VDID, 0xFFFFFFFF)) {
          ADBG (Concatenate ("PC00.TRP3.PDSX -", ToHexString (\_SB.PC00.TRP3.PDSX)))
          If (LEqual (\_SB.PC00.TRP3.PDSX, 1)) {
            Store (1, CTP1)
          }
          \_SB.PC00.TRP3.D3CE () // Put RP3 to D3 cold
        }
      }
    }
    ADBG ("TG1F End")
  } // End of TG1F

  If (ITRT) {

    Method (TBON, 1) {
      If (ITIM) {
        If (LEqual (Arg0, 0)) {
          ADBG("TBT0 Group ON Start")
          //TG0N()
          ADBG("TBT0 Group ON End")
        } Else {
          ADBG("TBT1 Group ON Start")
          //TG1N()
          ADBG("TBT1 Group ON End")
        }
      } Else {
        ADBG("TBT IMR is not valid")
      }
    } // End of TBON

    Method (TBOF, 1) {
      If (ITIM) {
        If (LEqual (Arg0, 0)) {
          ADBG("TBT0 Group OFF Start")
          ADBG(Concatenate("Skip D3C entry? ", ToHexString(\_SB.PC00.TDM0.SD3C)))
          If (LEqual(\_SB.PC00.TDM0.SD3C, 0)) { // Skip D3C entry
            TG0F()
          }
          ADBG("TBT0 Group OFF End")
        } Else {
          ADBG("TBT1 Group OFF Start")
          ADBG(Concatenate("Skip D3C entry? ", ToHexString(\_SB.PC00.TDM1.SD3C)))
          If (LEqual(\_SB.PC00.TDM1.SD3C, 0)) { // Skip D3C entry
            TG1F()
          }
          ADBG("TBT1 Group OFF End")
        }
      } Else {
        ADBG("TBT IMR is not valid")
      }
    } // End of TBOF

    // PowerResource (resourcename, systemlevel, resourceorder)
    // systemlevel - S5, the deepest system sleep level OSPM must maintain to keep this power resource
    // on following by the ordering. resourceorder - The resourceorder field in the Power Resource object
    // is a value per Power Resource that provides the system with the order in which Power Resources must
    // be enabled or disabled. Power Resource levels are enabled from low resourceorder values to high
    // values and are disabled from high resourceorder values to low values.
    PowerResource (TBT0,
                     0,
                     1)
    {
      Method (_STA, 0)
      {
        Return (\_SB.PC00.TDM0.STAT)
      }

      Method (_ON,0)
      {
        ADBG ("TBT0 _ON")
        If (LEqual (\_SB.PC00.TDM0.SCME, 0)) {
          TBON (0)
        }
      }

      Method (_OFF,0)
      {
        ADBG ("TBT0 _OFF")
        If (LEqual (\_SB.PC00.TDM0.SCME, 0)) {
          TBOF (0)
        }
      }
    } // End of PR (TBT0)

    PowerResource (TBT1,
                     0,
                     1)
    {
      Method (_STA, 0)
      {
        Return (\_SB.PC00.TDM1.STAT)
      }

      Method (_ON,0)
      {
        ADBG ("TBT1 _ON")
        If (LEqual (\_SB.PC00.TDM1.SCME, 0)) {
          TBON (1)
        }
      }

      Method (_OFF,0)
      {
        ADBG ("TBT1 _OFF")
        If (LEqual (\_SB.PC00.TDM1.SCME, 0)) {
          TBOF (1)
        }
      }
    } // End of PR (TBT1)
  } // End of If (ITRT)

  If (TRTD) {
    Method (TCON, 0) {
      ADBG ("TCSS ON")
      ADBG ("TCON")

      //Reset IOM D3 cold bit if it is in d3 cold now.
      If (LEqual (TD3C, 1)) // It was in D3 cold before.
      {
      //Reset IOM D3 cold bit
        ADBG (Concatenate ("TACK before D3 cold exit -", ToHexString (TACK)))
        Store (0, TD3C) //Request IOM for D3 cold exit sequence

        Store (0, Local0) //Time check counter variable

        // wait for ack, the maximum wait time for the ack is 100 msec. (Tune this accordingly after stable Si)
        While (LAnd (LNotEqual (TACK,0), LLess (Local0, TCSS_IOM_ACK_TIMEOUT_IN_MS)))  // Wait in this loop until TACK becomes 0 with timeout TCSS_IOM_ACK_TIMEOUT_IN_MS by default
        {
          Sleep (1) //Delay of 1ms
          Increment (Local0)
        }
        ADBG (Concatenate ("TACK after D3 cold exit -", ToHexString (TACK)))
        ADBG (Concatenate ("Time for wait TACK - ", ToHexString (Local0)))
        If (LEqual (Local0, TCSS_IOM_ACK_TIMEOUT_IN_MS)) // if TCSS_IOM_ACK_TIMEOUT_IN_MS timeout occurred
        {
          ADBG ("Error: Timeout occurred")
        }
        Else
        {
          ADBG ("D3Cold Exit")
        }
      } Else {
        //
        // Drop this method due to it is alredy exit D3 cold
        //
        ADBG ("Drop TCON due to it is already exit D3 cold")
        Return
      }
      ADBG ("TCSS ON End")
    } //End of TCON

    Method (TCOF, 0) {
      ADBG ("TCSS OFF Start")
      ADBG ("TCOF")

      ADBG (Concatenate ("DMA0 Skip D3C entry? ", ToHexString (\_SB.PC00.TDM0.SD3C)))
      ADBG (Concatenate ("DMA1 Skip D3C entry? ", ToHexString (\_SB.PC00.TDM1.SD3C)))
      ADBG (Concatenate ("XHCI Skip D3C entry? ", ToHexString (\_SB.PC00.TXHC.SD3C)))
      If (LOr (LNotEqual (\_SB.PC00.TXHC.SD3C, 0), LOr (LNotEqual (\_SB.PC00.TDM0.SD3C, 0), LNotEqual (\_SB.PC00.TDM1.SD3C, 0))))
      {
        Return                                                            // Skip D3C entry
      }

      ADBG ("D3Cold Entry")
      Store (1, TD3C) //Request IOM  for D3 cold entry sequence
      ADBG (Concatenate ("IOM D3Cold -", ToHexString (TD3C)))
      ADBG ("TCSS OFF End")
    } // End of TCOF

    PowerResource (D3C,
                    0,
                    0)  // Turn on second, turn off second to last
    {
      Name (STAT, 0x1)  // Variable to save power state 1 - TC Cold request cleared, 0 - TC Cold request sent

      Method (_STA, 0)
      {
        Return (STAT)
      }

      Method (_ON, 0)
      {
        \_SB.PC00.TCON ()
        Store (1, STAT)
      }

      Method (_OFF, 0)
      {
        \_SB.PC00.TCOF ()
        Store (0, STAT)
      }
    } // End of PR (D3C)
  } // End of if (TRTD)

  //
  // TCSS xHCI device
  //
  Device (TXHC) {

    Name (_ADR, 0x000D0000)  // _ADR: Address
    Name (_DDN, "TCSS XHCI controller")  // _DDN: DOS Device Name
    Name (_STR, Unicode ("TCSS XHCI controller"))  // _STR: Description String
    Name (DCPM, TCSS_XHCI)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (THCE, 1)) {
        Return (0x0F)
      }
      Return (0x0)
    }
    Include ("TcssXhci.asl")
  } // end "TCSS xHCI device"


  //
  // TCSS DMA 0 device
  //
  Device (TDM0) {

    Name (_ADR, 0x000D0002)  // _ADR: Address
    Name (_DDN, "TCSS DMA0 controller")  // _DDN: DOS Device Name
    Name (_STR, Unicode ("TCSS DMA0 controller"))  // _STR: Description String
    Name (DUID, 0) // TBT DMA number
    Name (DCPM, TCSS_DMA0)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (DME0, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }

    Method (BASE, 0) {
      Add (\_SB.PC00.GPCB (), 0x6A000, Local0)
      ADBG (Concatenate ("TDM0 MMIO Address", ToHexString (Local0)))
      Return (Local0)    // Return PCIE  Base address
    }
    Include ("TcssDma.asl")
    Include ("ITbtMailBox.asl")
  } // end "TCSS DMA 0 device"

  //
  // TCSS DMA 1 device
  //
  Device (TDM1) {

    Name (_ADR, 0x000D0003)  // _ADR: Address
    Name (_DDN, "TCSS DMA1 controller")  // _DDN: DOS Device Name
    Name (_STR, Unicode ("TCSS DMA1 controller"))  // _STR: Description String
    Name (DUID, 1) // TBT DMA number
    Name (DCPM, TCSS_DMA1)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (DME1, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }

    Method (BASE, 0) {
      Add (\_SB.PC00.GPCB (), 0x6B000, Local0)
      ADBG (Concatenate ("TDM1 MMIO Address", ToHexString (Local0)))
      Return (Local0)    // Return PCIE  Base address
    }
    Include ("TcssDma.asl")
    Include ("ITbtMailBox.asl")
  } // end "TCSS DMA 1 device"

  //
  // TCSS iTBT PCIE Root Port #00
  //
  Device (TRP0) {
    Method (_ADR, 0) {
      If (LNotEqual (TPA0,0)) {
        ADBG(Concatenate("TRP0 _ADR", ToHexString(TPA0)))
        Return (TPA0)
      } Else {
        Return (0x00070000)
      }
    }
    Name (TUID, 0) // TBT RP Number 0 for RP00
    Name (LTEN, 0)
    Name (LMSL, 0x88C8)
    Name (LNSL, 0x88C8)
    Name (DCPM, TCSS_ITBT_PCIE0_RP0)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (TRE0, 1)) {
        Return (0x0F)
      }
      Return (0x0)
    }
    Method (_INI)
    {
      Store (LTE0, LTEN)
      Store (PSL0, LMSL)
      Store (PNS0, LNSL)
    }
    Include ("TcssItbtPcieRp.asl")
  } // end "TCSS iTBT PCIE Root Port #00"

  //
  // TCSS iTBT PCIE Root Port #01
  //
  Device (TRP1) {
    Method (_ADR, 0) {
      If (LNotEqual (TPA1,0)) {
        ADBG(Concatenate("TRP1 _ADR", ToHexString(TPA1)))
        Return (TPA1)
      } Else {
        Return (0x00070001)
      }
    }
    Name (TUID, 1) // TBT RP Number 1 for RP01
    Name (LTEN, 0)
    Name (LMSL, 0x88C8)
    Name (LNSL, 0x88C8)
    Name (DCPM, TCSS_ITBT_PCIE0_RP1)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (TRE1, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }
    Method (_INI)
    {
      Store (LTE1, LTEN)
      Store (PSL1, LMSL)
      Store (PNS1, LNSL)
    }
    Include ("TcssItbtPcieRp.asl")
  } // end "TCSS iTBT PCIE Root Port #01"

  //
  // TCSS iTBT PCIE Root Port #02
  //
  Device (TRP2) {
    Method (_ADR, 0) {
      If (LNotEqual (TPA2,0)) {
        ADBG(Concatenate("TRP2 _ADR", ToHexString(TPA2)))
        Return (TPA2)
      } Else {
        Return (0x00070002)
      }
    }
    Name (TUID, 2) // TBT RP Number 2 for RP02
    Name (LTEN, 0)
    Name (LMSL, 0x88C8)
    Name (LNSL, 0x88C8)
    Name (DCPM, TCSS_ITBT_PCIE0_RP2)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (TRE2, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }
    Method (_INI)
    {
      Store (LTE2, LTEN)
      Store (PSL2, LMSL)
      Store (PNS2, LNSL)
    }
    Include ("TcssItbtPcieRp.asl")
  } // end "TCSS iTBT PCIE Root Port #02"

  //
  // TCSS iTBT PCIE Root Port #03
  //
  Device (TRP3) {
    Method (_ADR, 0) {
      If (LNotEqual (TPA3,0)) {
        ADBG(Concatenate("TRP3 _ADR", ToHexString(TPA3)))
        Return (TPA3)
      } Else {
        Return (0x00070003)
      }
    }
    Name (TUID, 3) // TBT RP Number 3 for RP03
    Name (LTEN, 0)
    Name (LMSL, 0x88C8)
    Name (LNSL, 0x88C8)
    Name (DCPM, TCSS_ITBT_PCIE0_RP3)

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (TRE3, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }
    Method (_INI)
    {
      Store (LTE3, LTEN)
      Store (PSL3, LMSL)
      Store (PNS3, LNSL)
    }
    Include ("TcssItbtPcieRp.asl")
  } // end "TCSS iTBT PCIE Root Port #03"

  Include ("TcssItbtPcieRpFru.asl")
  Include ("TcssXhciFru.asl")
  Include ("TcssDmaFru.asl")
} // End of Scope (\_SB.PC00)
} // End of DefinitionBlock
