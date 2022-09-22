/** @file
  ACPI RTD3 SSDT table for ADL N RVP

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "AdlN_Crb",
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
External(WWEN)

External(TPDT)
External(TPLT)
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

//
//2.5Gbe LAN Foxville I225
//
External(FVWP)
External(FVRE)
External(FVRP)
External(FVPE)
External(FVPP)
External(FVSP)


//
//sata
//

External(SATP)
External(STPP)

//
//GbE
//
External(GBED)

External(XDCE)

Include ("Rtd3Common.asl")
#define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP04
External(WWAN_PCIE_ROOT_PORT.PXSX, DeviceObj)
External(WWAN_PCIE_ROOT_PORT.LASX)
External(\_SB.PC00.RP12.PXSX,DeviceObj)
External(\_SB.PC00.RP04.PXSX.WIST,MethodObj)
External(\_SB.PC00.XHCI.RHUB.HS08, DeviceObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)


External(\_SB.PC00.I2C0, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C0.TPD0, DeviceObj) // Touch pad
External(\_SB.PC00.I2C0.TPD0._STA, MethodObj)
External(\_SB.PC00.I2C0.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C0.TPL1._STA, MethodObj)

External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)

External(\_SB.PC00.SAT0.PRT0.PRES, MethodObj)

//Board configuration

//PCIe P3 -M.2 KEY E WWAN7360 -SRC CLK-NA(REWORK)
//PCIe P4 -M.2 KEY B WLAN Wifi-BT -SRC CLK-2
//PCIe P4 -M.2 KEY B WWAN7360 -SRC CLK-2(REWORK)
//PCIe P7 -x4 PCIe DT Slot (1Pair -x1 cem slot -sd card/TSN AIC) -SRC CLK-NA TSN use-case
//PCIe P9 -x4 PCIe DT Slot (x1 cem slot -sd card/TSN AIC) -SRC CLK-3
//PCIe P10 - FoxVille 2.5G PHY I225 -SRC CLK-1
//PCIe P11 -Sata Direct(SATA P0) -SRC CLK-NA
//PCIe P9:P12 -PCIe X4 ssd -SRC CLK-0(LAN REVERSAL) (REWORK)
//PCIe P12 -PCIe X4 ssd (only one lane) -SRC CLK-0

// PCIe root ports - START
  /// PCIE RTD3 - PCIe M.2 CONNECTOR WWAN
    ///
    If (LAnd(LNotEqual(WWEN,0),LNotEqual(WRTO,0)) ) {
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
      }
    }
    ///
    /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR but only 1/2 lanes
    ///
    Scope(\_SB.PC00.RP09) {
      Name(RSTG, Package() {0, 0})
      Store(PSPR, Index(RSTG, 0))
      Store(PPSR, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSPE, Index(PWRG, 0))
      Store(PPSP, Index(PWRG, 1))
      Name(WAKG, 0)
      Name(SCLK, 3)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      store(PSWP,WAKG)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
    }

    ///
    /// 2.5Gbe LAN Foxville I225
    ///

    //FVWP, 32,      // [FoxLanWakeGpio         ] Foxville I225 Wake Gpio pin
    //FVRE, 32,      // [FoxLanRstGpio          ] Foxville I225 Reset Gpio pin
    //FVRP, 8,        // [FoxLanRstGpioPolarity  ] Foxville I225 Reset Gpio pin polarity
    //FVPE, 32,      // [FoxLanDisableNGpio  ] Foxville I225 Disable N Gpio pin
    //FVPP, 8,       // [FoxLanDisableNGpioPolarity] Foxville I225 Disable N Gpio pin polarity
    //FVSP, 8,       // [FoxLanSupport] Foxville I225 support configuration
    If(LEqual(FVSP,1)) {
      Scope(\_SB.PC00.RP10) {
        Name(RSTG, Package() {0, 0})
        Store(FVRE, Index(RSTG, 0))
        Store(FVRP, Index(RSTG, 1))
        Name(PWRG, Package() {0, 0})
        Store(FVPE, Index(PWRG, 0))
        Store(FVPP, Index(PWRG, 1))
        Name(WAKG, 0)
        Store(FVWP, WAKG)
        Name(WAKP, 0)
        Name(SCLK, 1)
        Include("PcieRpGenericPcieDeviceRtd3.asl")
      }
    }

    ///
    /// PCIe RP12 RTD3 - PCH M.2 SSD RTD3
    ///
    Scope(\_SB.PC00.RP12) {
      Name(RSTG, Package() {0, 0})
      Store(SSDR, Index(RSTG, 0))
      Store(SDRP, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SSDP, Index(PWRG, 0))
      Store(SDPP, Index(PWRG, 1))
      Name(WAKG, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)              // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      Include("PcieRpGenericPcieDeviceRtd3.asl")
      Scope(\_SB.PC00.RP12.PXSX) {
        Include("PcieRpSsdStorageRtd3Hook.asl")
      }
    }


// PCIe root ports - END

//
// SATA - START
//
  Scope(\_SB.PC00.SAT0) {
    Scope(PRT0) {
      If (PRES ()) {
        Name(PWRG, Package() {0, 0})
        Store(SATP, Index(PWRG, 0))
        Store(STPP, Index(PWRG, 1))
      }
    }
    Include("Rtd3Sata.asl")
  }


//
// USB - START
//

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

//

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
        Scope(TPL1){
          Name (TD_P, Package(){\_SB.PC00.I2C0.PTPL})               // TD_P - Touch Device Power Package

          Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
          Alias(\_SB.PC00.I2C0.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

          Include("Rtd3I2cTouchDev.asl")
          Method(_PS0) { PS0X() }
          Method(_PS3) { PS3X() }
        }// End Of Scope(TPL1)
      }
    } //  Scope(\_SB.PC00.I2C0)

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
      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP04, 0x02)      // device wake
        \_SB.CAGS(WLWK)    // WIFI_WAKE_N
      }

      //Wwan wake event
      If (\_SB.ISME(WWKP))
      {
        \_SB.SHPO(WWKP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(WWAN_PCIE_ROOT_PORT, 0x02)     // device wake
        \_SB.CAGS(WWKP)    // WWAN_WAKE_N
      }

      //Pcie x1 DT slot wake event
      If (\_SB.ISME(PSWP))
      {
        \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP09, 0x02)      // device wake
        \_SB.CAGS(PSWP)
      }
    }
  } //Scope(\_GPE)
//GPE Event handling - End
} // End SSDT
