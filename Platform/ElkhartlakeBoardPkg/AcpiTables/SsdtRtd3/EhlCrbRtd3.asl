/** @file
  ACPI RTD3 SSDT table for EHL U DDR4 CRB

 Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "AcpiRf",
    "Ehl_Crb",
    0x1000
    )
{
External(RTD3, IntObj)
External(S0ID)
External(OSYS)
External(HGMD)
External(\ECON, IntObj)

External(GBED)
External(SDS9)

Include ("Rtd3Common.asl")

External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)
External(PEPC)
External(WRTO)
External(PRST)
External(WPRP)
External(WFCP)
External(PFCP)
External(WWKP)
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
External(PSWP)
External(PS1C)
External(PS2C)
External(RPNB)
External(PSPE)
External(PPSP)
External(PSPR)
External(PPSR)
External(PSW2)
External(RPN2)
External(PSP2)
External(PS2P)
External(PSR2)
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
External(TPLS)
External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
#define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP05
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.PC00.RP07.PXSX.WIST, MethodObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)
External(\_SB.PC00.UA00.BTH0, DeviceObj)
External(\_SB.PC00.I2C0, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C0.TPD0, DeviceObj) // Touch pad
External(\_SB.PC00.I2C0.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C2, DeviceObj) //I2C2 Controller
External(\_SB.PC00.I2C2.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C0.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C1.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C2.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C0.TPD0._STA, MethodObj)
External(WWAN_PCIE_ROOT_PORT.PXSX, DeviceObj) // For WWAN
External(\_SB.PC00.RP01.PXSX, DeviceObj)
External(WWAN_PCIE_ROOT_PORT.LASX)

// PCIe root ports - START
  ///
  /// PCIE RTD3 - PCIe M.2 CONNECTOR WWAN
  ///
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
      Name(SCLK, 2)
      Include("Rtd3PcieWwan.asl")
      Scope(PXSX) {
        Method(_S0W, 0) {
          //
          // WRTO - WWAN RTD3 Option
          //   3: D3/L2
          //   1: D0/L1.2
          //   0: Disabled
          //
          If(LEqual(WRTO, 3)) {
            Return(4)
          } Else {
            Return(3)
          }
        }

        // For WWAN endpoint device
        Method(_PR0) {
          Return(Package(){WWAN_PCIE_ROOT_PORT.PXP})
        }

        Method(_PR3) {
          Return(Package(){WWAN_PCIE_ROOT_PORT.PXP})
        }

        //----------------------------------------------------------------------------------------
        //  Intel Proprietary WWAN Enabling DSM Method
        //
        //  Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
        //  Arguments:
        //    Arg0: UUID Unique function identifier - bad01b75-22a8-4f48-8792-bdde9467747d
        //    Arg1: Integer Revision Level - 0
        //    Arg2: Integer Function Index -
        //        Function Index - 0 (Returns the supported _DSM methods)
        //        Function Index - 1 (Reserved)
        //        Function Index - 2 (Reserved)
        //        Function Index - 3 (Conveys the BIOS ACPI Device Power Mode setup option selection to the WWAN driver)
        //    Arg3: Package Parameters - None
        //-----------------------------------------------------------------------------------------
        // DSM UUID for WWAN Enabling. Do Not change.
        Method(_DSM, 0x4, Serialized) {
          // DSM UUID for WWAN. Do Not change.
          If(LEqual(Arg0, ToUUID("bad01b75-22a8-4f48-8792-bdde9467747d")))
          {
            //
            // Function 0: Query of supported functions.
            //
            If(LEqual(Arg2, Zero))
            {
              Return(Buffer(One) { 0x9 })
            }

            //
            // Function 1: Reserved.
            //
            If(LEqual(Arg2, 1))
            {
              //Reserved
            }

            //
            // Function 2: Reserved
            //
            If(LEqual(Arg2, 2))
            {
              //Reserved
            }

            //
            // Function 3: Conveys the BIOS ACPI Device Power Mode setup option selection to the WWAN driver
            //
            If(LEqual(Arg2, 3))
            {
              //
              // _DSM Power
              // WRTO - WWAN RTD3 Option
              //   3: D3/L2
              //   1: D0/L1.2
              //   0: Disabled
              //
              Return(WRTO)
            }
          }
          Return(Buffer(One) { 0x00 }) // Guid mismatch
        }
      }
    }
  }
  ///
  /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR
  ///
    Scope(\_SB.PC00.RP01) {
      Name(RSTG, Package() {0, 0})
      Store(PSPR, Index(RSTG, 0))
      Store(PPSR, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSPE, Index(PWRG, 0))
      Store(PPSP, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(PSWP, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      Store(PS1C, SCLK)
      Include("Rtd3Pcie.asl")
      Scope(\_SB.PC00.RP01.PXSX) {
        Include("Rtd3PcieSsdStorage.asl")
      }
    }

  ///
  /// PCIE RTD3 - PCIE SLOT 2 - X1 CONNECTOR
  ///
    Scope(\_SB.PC00.RP07) {
      Name(RSTG, Package() {0, 0})
      Store(PSR2, Index(RSTG, 0))
      Store(SR2P, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSP2, Index(PWRG, 0))
      Store(PS2P, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(PSW2, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 1)
      Store(PS2C, SCLK)
      Include("Rtd3Pcie.asl")
    }
//

// PCIe root ports - END

//
// SATA - START
//
  Scope(\_SB.PC00.SAT0) {
    Scope(PRT1) {
      Name(PWRG, Package() {0, 0})
      Store(SATP, Index(PWRG, 0))
      Store(STPP, Index(PWRG, 1))
    }

    Include("Rtd3Sata.asl")
  }


//
// SATA - END
//
//
// USB - START
//
//

// Note:- The USB3/2 Type-A ports doesnt require D3 cold. Since these are user connectable
// ports and needs device insertion detection to enumerate the device with screen off.




// USB3 controller should attain Link down state which is supported as part of spec for USB3.

Include ("Rtd3UsbCommon.asl")  // Do not remove this file.







Include ("Rtd3Cnvi.asl")
//
// Report BT device if CNVd is present
  // EHL M.2 Hybrid-E routing
  //   USB:  HS10
  //   PCIe: RP07
//
  If (LOr (LAnd (LEqual (\_SB.PC00.CNIP (), 1), LEqual (CBTC, 1)), LEqual (\_SB.PC00.RP07.PXSX.WIST (), 1))) {
    Scope(\_SB.PC00.XHCI.RHUB.HS10) {
      If (\_SB.PC00.RP07.PXSX.WIST ()){
    //
    // Power resource object for CNVd BT
    //
    PowerResource (DBTP, 0, 0)
    {
      Method(_STA, 0)
      {
        If(LEqual (\_SB.GBTR (), 1)) {
          Return (0x1)
        } Else {
          Return (0x0)
        }
      }
      Method(_ON, 0, Serialized) { // Turn on BT Core
        \_SB.BTRK (0x01)
      }
      Method(_OFF, 0, Serialized) { // Turn off BT Core
        \_SB.BTRK (0x00)
      }
    }
    }
    Name(_S0W, 2) // S0 Device Wake State
      Method(_PR0) {
        If (\_SB.PC00.CNIP ()) {
          Return (Package(){BTPR})
        }
        If (\_SB.PC00.RP07.PXSX.WIST ()){
          Return (Package(){DBTP})
        }
        Return(Package(){}) // - Return NULL Power Package if BT is not present.
      }
      Method(_PR3) {
        If (\_SB.PC00.CNIP ()) {
          Return (Package(){BTPR})
        }
        If (\_SB.PC00.RP07.PXSX.WIST ()){
          Return (Package(){DBTP})
        }
        Return(Package(){}) // - Return NULL Power Package if BT is not present.
      }
    } // Scope(\_SB.PC00.XHCI.RHUB.HS10)
}

//
// Report BT power resource if CNVd BT is UART.
//
If (LEqual (SDS9, 2)) { // Only required for CNVd BT over UART selection
  Scope (\_SB.PC00.UA00.BTH0) {
    Method(_PS0, 0, Serialized)
    {
      \_SB.BTRK (0x01) // Turn on BT
    }
    Method(_PS3, 0, Serialized)
    {
      \_SB.BTRK (0x00) // Turn off BT
    }
  }
}

External(XDCE)
//
// XDCI - start
//
  If(LEqual(XDCE,1)) {
    Scope(\_SB)
    {
      //
      // Dummy power resource for USB D3 cold support
      //
      PowerResource(USBC, 0, 0)
      {
        Method(_STA) { Return (0xF) }
        Method(_ON) {}
        Method(_OFF) {}
      }
    }

    Scope(\_SB.PC00.XDCI)
    {
      OperationRegion (GENR, SystemMemory, Add(And(XDCB, 0xFFFFFFFFFFFFFF00), 0x10F81C), 0x4)  //AON MMIO - 10F81C: APBFC_U3PMU_CFG5
      Field (GENR, WordAcc, NoLock, Preserve)
      {
            ,   2,
        CPME,   1,    //bit2 core_pme_en
        U3EN,   1,    //bit3 u3_pme_en
        U2EN,   1     //bit4 u2_pme_en
      }

      Method (_PS3, 0, NotSerialized)
      {
        Store (One, CPME)
        Store (One, U2EN)
        Store (One, U3EN)

        \_SB.CSD3(MODPHY_SPD_GATING_XDCI)
      }
      Method (_PS0, 0, NotSerialized)
      {
        Store (Zero, CPME)
        Store (Zero, U2EN)
        Store (Zero, U3EN)

        If(LEqual(DVID,0xFFFF)) {
          Return()
        }
        \_SB.CSD0(MODPHY_SPD_GATING_XDCI)
      }

      Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
      {
        Return (Zero)
      }

      Method (_PR3, 0, NotSerialized)  // _PR3: Power Resources for D3hot
      {
        Return (Package (0x01)
        {
          USBC // return dummy package
        })
      }
    } // Scope(\_SB.PC00.XDCI)
  } // XDCE
//
// XDCI - end
//

//
// USB - END
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

//
// Human Interface Devices Start
//
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
          // power rail = NOT there for EHL U
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
        If(LNotEqual(\_SB.PC00.RP01.VDID, 0xFFFFFFFF)) {
          Notify(\_SB.PC00.RP01, 0x02)      // device wake
        }
          \_SB.CAGS(PSWP)
        }

      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP07, 0x02)      // device wake
        \_SB.CAGS(WLWK)    // WIFI_WAKE_N
      }

      //Wwan wake event
      If (\_SB.ISME(WWKP))
      {
        \_SB.SHPO(WWKP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(WWAN_PCIE_ROOT_PORT, 0x02)     // device wake
        \_SB.CAGS(WWKP)    // WWAN_WAKE_N
      }
      //X1 PCIe Connector (SLOT2) wake event
      If (\_SB.ISME(PSW2))
      {
        \_SB.SHPO(PSW2, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP07, 0x02)     // device wake
        \_SB.CAGS(PSW2)    // Clear GPE event status
      }
    }
  } //Scope(\_GPE)
//GPE Event handling - End

} // End SSDT
