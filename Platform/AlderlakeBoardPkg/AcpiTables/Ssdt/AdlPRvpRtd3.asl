/** @file
  ACPI RTD3 SSDT table for ADL P LP4 RVP

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <AcpiDebug.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "AdlP_Rvp",
    0x1000
    )
{
External(S0ID)
//
//Pcie Slot 1 Pins
//
External(PSPR)
External(PPSP)
External(PSPE)
External(PPSR)
External(PSWP)

//
// Pch ssd Pins
//
External(SSDP)
External(SSDR)
External(SDRP)
External(SDPP)

//
// CPU  ssd1 Pins
//
External(SDR1)
External(SDP1)
External(SD2R)
External(SDR2)
External(SD2P)

//
// CPU ssd2 Pins
//
External(SD5R)
External(SDR5)
External(SD5P)
External(SDP5)

//
// DG Pins
//
External(DG2P)
External(DGP2)
External(DGR2)
External(DG2R)
External(P1WP)
External(XDCE)

External(GBED)

//
// Touch panel Pins
//
External(GPDI)
External(GPLI)
External(GPLP)
External(GPLR)
External(PPDI)
External(PPLI)
External(PPLP)
External(PPLR)
External(TPLS)

//
//WLAN wake Pins
//
External(WLWK)
External(TPDT)
External(TPLT)

//
//X4 dt slots
//

External(PSW2)
External(PSR2)
External(SR2P)
External(PSP2)
External(PS2P)

//
//sata
//

External(SATP)
External(STPP)
External(R8DC)

//
// Wake Rework on RP08
//
External (R8WR)

Include ("Rtd3Common.asl")
External(\_SB.PC00.RP09.PXSX,DeviceObj)
External(\_SB.PC00.PEG0.PEGP,DeviceObj)
External(\_SB.PC00.PEG1.PEGP,DeviceObj)
External(\_SB.PC00.PEG2.PEGP,DeviceObj)
External(\_SB.PC00.RP05.PXSX.WIST,MethodObj)
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)
External(\_SB.PC00.UA02.BTH0, DeviceObj)

External(\_SB.PC00.I2C0, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C0.TPD0, DeviceObj) // Touch pad
External(\_SB.PC00.I2C0.TPD0._STA, MethodObj)
External(\_SB.PC00.I2C0.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C0.TPL1._STA, MethodObj)

External(\_SB.PC00.I2C1, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C1.TPL1._STA, MethodObj)

External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)

//Board configuration

//PCIe P5 -M.2 KEY E WLAN -SRC CLK-2
//PCIe P6 -M.2 KEY B WWAN -SRC CLK-5
//PCIe P7 -GBE LAN / x4 PCIe DT Slot (Pair 2) -SRC CLK-6
//PCIe P8 -x4 PCIe DT Slot (Pair 1) -SRC CLK-7
//PCIe P9:P12 -PCIe X4 Nvme / x1 M.2 SATA/UFS/Sata direct -SRC CLK-1
//PCIe4/PEG0 P0:P3- X4 SSD1 -SRC CLK-0
//PCIe5/PEG1 P0:P7- X8 DG/DG2 -SRC CLK-3
//PCIe4/PEG2 P4:P7- X4 SSD2 -SRC CLK-4

// PCIe root ports - START
    ///
    /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR
    ///
    If (LEqual(R8DC,0)) {
      Scope(\_SB.PC00.RP08) {
        Name(RSTG, Package() {0, 0})
        Store(PSPR, Index(RSTG, 0))
        Store(PPSR, Index(RSTG, 1))
        Name(PWRG, Package() {0, 0})
        Store(PSPE, Index(PWRG, 0))
        Store(PPSP, Index(PWRG, 1))
        Name(WAKG, 0)
        Store(PSWP,WAKG)
        Name(SCLK, 7)
        Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
        Include("PcieRpGenericPcieDeviceRtd3.asl")
      }
    }

    ///
    /// PCIe RP09 RTD3 - PCH M.2 SSD RTD3
    ///
    Scope(\_SB.PC00.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(SSDR, Index(RSTG, 0))
      Store(SDRP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SSDP, Index(PWRG, 0))
      Store(SDPP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 1)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP09.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// CPU M.2 SSD RTD3
    ///
    Scope(\_SB.PC00.PEG0) {
      Name(RSTG, Package() {0, 0})
      Name(PWRG, Package() {0, 0})
      Store(SD2R, Index(RSTG, 0))
      Store(SDR1, Index(RSTG, 1))
      Store(SD2P, Index(PWRG, 0))
      Store(SDP1, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(0, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.PEG0.PEGP) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }


    ///
    /// CPU M.2 SSD RTD3
    ///
    Scope(\_SB.PC00.PEG2) {
      Name(RSTG, Package() {0, 0})
      Name(PWRG, Package() {0, 0})
      Store(SD5R, Index(RSTG, 0))
      Store(SDR5, Index(RSTG, 1))
      Store(SD5P, Index(PWRG, 0))
      Store(SDP5, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(0, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 4)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.PEG2.PEGP) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }

    ///
    /// PEG PCIE RTD3 - PEG PCIE SLOT 3 - X8 CONNECTOR
    ///
   Scope(\_SB.PC00.PEG1) {
      Name(RSTG, Package() {0, 0})
      Name(PWRG, Package() {0, 0})
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 3)
      Store(DG2R, Index(RSTG, 0))
      Store(DGR2, Index(RSTG, 1))
      Store(DG2P, Index(PWRG, 0))
      Store(DGP2, Index(PWRG, 1))
      Name(WAKG, 0)
      Store(P1WP, WAKG)
      // Note: IPC1 Command Timeout need to increase for dGPU only.
      Name (TMCS, 3000) // IPC Command Timeout Increase to 3 Secs.
      If(LNotEqual (DGBA, 0)) {
        Include ("PcieRpDiscreteGraphicsDeviceRtd3Hook.asl")
        Include ("PcieRpDiscreteGraphicsHpdAsD3WakeHook.asl")
      }
      Scope (\_SB.PC00.PEG1.PEGP) {
        Include ("PcieRpSsdStorageRtd3Hook.asl")
      }
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }
// PCIe root ports - END
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
// PCH I2C0 - TouchPad Power control
//
    Scope(\_SB.PC00.I2C0){

      Method(PS0X,0,Serialized)
      {
      }
      /// PS3X Method, called by PS3 method in PchSerialIo.asl
      Method(PS3X,0,Serialized)
      {
      }
      /// \ref i2c0_pr_pxtc
      ///@defgroup i2c0_scope  I2C1 Scope
      If (LNotEqual(TPDT,0)) {
      PowerResource(PXTC, 0, 0){
        Method(_STA){
          Return(PSTA(0))
        }

        Method(_ON){
          PON(0)
        }

        Method(_OFF){
          POFF(0)
        }
      } // End PXTC
      }
      /// \ref i2c0_pr_ptpl
      ///@defgroup i2c0_scope
      If (LNotEqual(TPLT,0)) {
      PowerResource(PTPL, 0, 0){
        Method(_STA){
          Return(PSTA(1))
        }

        Method(_ON){
          PON(1)
        }

        Method(_OFF){
          POFF(1)
          }
        }
      }
      /// Variable:
      Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON
      Method(PSTA, 1, Serialized){
        If (Arg0 == 0) {
          Return(0x01)
        }
        If (Arg0 == 1) {
          If(LEqual(\_SB.GGOV(GPLP), 1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Return(0x00)
      }
      // Timing of Power and Reset for I2C Touch Panel
      //  _____________________________________________________________________________________________________
      // | minimum required time |  Reset de-assert from Power On (ms)   |     Power Off from Reset assert (ms)|
      // |_______________________|_______________________________________|_____________________________________|
      // | current setting       |                2                      |                3                    |
      // |_______________________|_______________________________________|_____________________________________|
      // * no maximum time limitation.
      Method(PON, 1, Serialized) /// _ON Method \n Turn on
      {
        If (Arg0 == 0) {
          // enable int line
          \_SB.SGRA(GPDI, PPDI)
        }
        ElseIf (Arg0 == 1) {
        // drive pwr high
        \_SB.SGOV(GPLP, PPLP)
        Sleep(2)                  // time for Reset de-assert from Power On
          // De-Assert GPIO RST
        \_SB.SGOV(GPLR, PPLR)
          // update ONTM
          Store(Timer(), ONTM)
          // enable int line
        \_SB.SGRA(GPLI, PPLI)
        }
      }

      Method(POFF, 1, Serialized)  /// _OFF method \n Turn off
      {
        If (Arg0 == 0) {
          // disable int line
          Xor(PPDI, 1, Local0)
          \_SB.SGRA(GPDI, Local0)
        }
        ElseIf (Arg0 == 1) {
          // disable int line
        Xor(PPLI, 1, Local0)
        \_SB.SGRA(GPLI, Local0)
        // Assert GPIO RST
        Xor(PPLR, 1, Local0)
        \_SB.SGOV(GPLR, Local0)
        Sleep(3)                   // time for Power Off from Reset assert
        // drive pwr low
        Xor(PPLP, 1, Local0)
        \_SB.SGOV(GPLP, Local0)
          // update ONTM
          Store(Zero , ONTM)  ///- Clear ONTM
        }
      }

      If (LNotEqual(TPDT,0)) {
      Scope(TPD0){
        Name (TD_P, Package(){\_SB.PC00.I2C0.PXTC})               // TD_P - Touch Device Power Package

        Alias(IC0D, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PC00.I2C0.ONTM, TD_C)                     // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPD0)
     }

      If (LNotEqual(TPLT,0)) {
        If (TPLS == 1) {  // If touch panel's bus speed is 400Khz? TCH_PNL_BUS_SPEED_400K
          Scope(TPL1){
            Name (TD_P, Package(){\_SB.PC00.I2C0.PTPL})               // TD_P - Touch Device Power Package

            Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
            Alias(\_SB.PC00.I2C0.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

            Include("Rtd3I2cTouchDev.asl")
            Method(_PS0) { PS0X() }
            Method(_PS3) { PS3X() }
          }// End Of Scope(TPL1)
        }
      }
    }//  Scope(\_SB.PC00.I2C0)

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
  Scope (\_GPE) {
    Method (_L14) {
       If (LEqual(R8WR,1)) {
        \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP08, 0x02)   // Device wake
      } ElseIf (CondRefOf (\_SB.PC00.PEG1.DHDW)) { // DG support HPD as D3 Wake GPIO and DHDW exists
        \_SB.PC00.PEG1.DHDW () // Disable Discrete Graphics HPD SCI implementated As D3 Wake GPIO
        Notify(\_SB.PC00.PEG1, 0x00)   // Bus Check
      } Else {
        \_SB.SHPO(\_SB.PC00.PEG1.WAKG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.PEG1, 0x02)   // Device wake
      }
    }
    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP05, 0x02)      // device wake
        \_SB.CAGS(WLWK)    // WIFI_WAKE_N
      }

      //Pcie x4 slot wake event
      If (\_SB.ISME(PSWP))
      {
        \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        \_SB.CAGS(PSWP)
      }
    }
  } //Scope(\_GPE)
//GPE Event handling - End
} // End SSDT
