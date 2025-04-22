/** @file
  ACPI RTD3 SSDT table for TGL U DDR4 RVP

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
    "Rtd3.aml",
    "SSDT",
    2,
    "Rtd3",
    "TglU_Rvp",
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
External(DGVR)
External(DGVP)
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
External(TPLT)
External(TPDT)
External(SLTS)
External(VMDE)
//External(DGOP)
External(DGBA)
External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(PRES, MethodObj)

#define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP04

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
External(\_SB.PC00.PEG0.PEGP, DeviceObj)
External(\_SB.PC00.RP09.PXSX, DeviceObj)
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
      }
    }

    ///
    /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR
    ///
    If(LNotEqual(HGMD,2)) {
      Scope(\_SB.PC00.RP05) {
        Name(RSTG, Package() {0, 0})
        Store(PSPR, Index(RSTG, 0))
        Store(PPSR, Index(RSTG, 1))
        Name(PWRG, Package() {0, 0})
        Store(PSPE, Index(PWRG, 0))
        Store(PPSP, Index(PWRG, 1))
        Name(WAKG, 0)
        Store(PSWP, WAKG)
        Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
        Name(SCLK, 6)
        Include("Rtd3Pcie.asl")
      }
    }

    ///
    /// PCIe RP09 RTD3 - M.2 SSD RTD3
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
      Name(SCLK, 3)
      Include("Rtd3Pcie.asl")
      Scope(\_SB.PC00.RP09.PXSX) {
        Name (SD3E, 1)
        Include("Rtd3PcieSsdStorage.asl")
      }
    }

    ///
    /// PCIE RTD3 - PCIE SLOT 2 - X1 CONNECTOR
    ///
    If(LNotEqual(HGMD,2)) {
      Scope(\_SB.PC00.RP08) {
        Name(RSTG, Package() {0, 0})
        Store(PSR2, Index(RSTG, 0))
        Store(SR2P, Index(RSTG, 1))
        Name(PWRG, Package() {0, 0})
        Store(PSP2, Index(PWRG, 0))
        Store(PS2P, Index(PWRG, 1))
        Name(WAKG, 0)
        Store(PSW2, WAKG)
        Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
        Name(SCLK, 5)
        Include("Rtd3Pcie.asl")
      }
    }

    ///
    /// PEG PCIE RTD3 - PEG PCIE SLOT 1 - X4 CONNECTOR
    ///
    Scope(\_SB.PC00.PEG0) {
      Name(RSTG, Package() {0, 0})
      Name(VCCE, Package() {0, 0})
      Store(PSPR, Index(RSTG, 0))
      Store(PPSR, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(PSPE, Index(PWRG, 0))
      Store(PPSP, Index(PWRG, 1))
      Store(DGVR, Index(VCCE, 0))
      Store(DGVP, Index(VCCE, 1))
      Name(WAKG, 0)
      Store(PSWP, WAKG)
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      // Note: IPC1 Command Timeout need to increase for dGPU only.
      Name(TMCS, 3000) // IPC1 Command Timeout Increase to 3 Secs.

      Include("Rtd3CpuPcie.asl")
      ///
      /// PCIe RTD3 - CPU M.2 SSD RTD3
      ///
      Scope(\_SB.PC00.PEG0.PEGP) {
        ///
        /// If AHCI/NVMe drive connected to GEN4 PCIe controller, change GPIO configuration for it
        ///
        If(LEqual(SLTS,1)) {
          Store(SD2R, Index(RSTG, 0))
          Store(SDR1, Index(RSTG, 1))

          Store(SD2P, Index(PWRG, 0))
          Store(SDP1, Index(PWRG, 1))

          Store(0, WAKG)
        }
        Name (SD3E, 1)
        Include("Rtd3PcieSsdStorage.asl")
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
// SATA - END
//


//
// USB - START
//
//

//

If (LEqual(And(RCG0, 0x08), 0x08)) {
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
      /// \ref i2c0_pr_ptpl
      ///@defgroup i2c0_scope
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
      /// Variable:
      Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON
      Method(PSTA, 1, Serialized){
        If (Arg0 == 0) {
          Return(0x01)
        }
        If (Arg0 == 1) {
          If(LEqual(\_SB.GGOV(TPP1), 1)) {
            Return(0x01)
          } Else {
            Return(0x00)
          }
        }
        Return(0x00)
      }
      Method(PON, 1, Serialized) /// _ON Method \n Turn on
      {
        If (Arg0 == 0) {
          // enable int line
          \_SB.SGRA(GPDI, 1)
        }
        ElseIf (Arg0 == 1) {
          // De-Assert GPIO RST
          \_SB.SGOV(TPR1, PPR1)
          // drive pwr high
          \_SB.SGOV(TPP1, PPP1)
          // update ONTM
          Store(Timer(), ONTM)
          // enable int line
          \_SB.SGRA(GPI1, PPI1)
        }
      }

      Method(POFF, 1, Serialized)  /// _OFF method \n Turn off
      {
        If (Arg0 == 0) {
          // disable int line
          \_SB.SGRA(GPDI, 0)
        }
        ElseIf (Arg0 == 1) {
          // disable int line
          Xor(PPI1, 1, Local0)
          \_SB.SGRA(GPI1, Local0)
          // drive pwr low
          Xor(PPP1, 1, Local0)
          \_SB.SGOV(TPP1, Local0)
          // Assert GPIO RST
          Xor(PPR1, 1, Local0)
          \_SB.SGOV(TPR1, Local0)
          // update ONTM
          Store(Zero , ONTM)  ///- Clear ONTM
        }
      }

      If (LGreater (TPDT, Zero)) {
        Scope(TPD0){
          Name (TD_P, Package(){\_SB.PC00.I2C0.PXTC})               // TD_P - Touch Device Power Package

          Alias(IC0D, TD_D)                                         // TD_D - Touch Device power on delay
          Alias(\_SB.PC00.I2C0.ONTM, TD_C)                     // TD_C - Touch Device I2C controller power on timestamp

          Method(_PS0) { PS0X() }
          Method(_PS3) { PS3X() }
        }// End Of Scope(TPD0)
      }

      If (TPLS == TCH_PNL_BUS_SPEED_400K) {  // If touch panel's bus speed is 400Khz?
        If (LGreater (TPLT, Zero)) {
          Scope(TPL1){
            Name (TD_P, Package(){\_SB.PC00.I2C0.PTPL})               // TD_P - Touch Device Power Package

            Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
            Alias(\_SB.PC00.I2C0.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

            Method(_PS0) { PS0X() }
            Method(_PS3) { PS3X() }
          }// End Of Scope(TPL1)
        }
      }
    }//  Scope(\_SB.PC00.I2C0)
//
// PCH I2C1 - TouchPanel Power control
//
    Scope(\_SB.PC00.I2C1){
      /// PS0X Method, called by PS0 method in PchSerialIo.asl
      Method(PS0X,0,Serialized)
      {
      }
      /// PS3X Method, called by PS3 method in PchSerialIo.asl
      Method(PS3X,0,Serialized)
      {
      }
      /// \ref i2c1_pr_pxtc
      ///@defgroup i2c1_scope  I2C1 Scope
      PowerResource(PXTC, 0, 0){
        Method(_STA){
          Return(PSTA())
        }

        Method(_ON){
          PON()
        }

        Method(_OFF){
          POFF()
        }
      } // End PXTC
      /// Variable:
      Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON
      Method(PSTA){
        If(LEqual(\_SB.GGOV(GPLP),1)) {
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(PON, 0) /// _ON Method \n Turn on
      {
        // De-Assert GPIO RST
        \_SB.SGOV(GPLR, PPLR)
        // drive pwr high
        \_SB.SGOV(GPLP, PPLP)
        // update ONTM
        Store(Timer(), ONTM)
        // enable int line
        \_SB.SGRA(GPLI, PPLI)
      }
      Method(POFF, 0)  /// _OFF method \n Turn off
      {
        // disable int line
        Xor(PPLI, 1, Local0)
        \_SB.SGRA(GPLI, Local0)
        // drive pwr low
        Xor(PPLP, 1, Local0)
        \_SB.SGOV(GPLP, Local0)
        // Assert GPIO RST
        Xor(PPLR, 1, Local0)
        \_SB.SGOV(GPLR, Local0)
        // update ONTM
        Store(Zero , ONTM)  ///- Clear ONTM
      }

      If (LGreater (TPLT, Zero)) {
        Scope(TPL1){
          Name (TD_P, Package(){\_SB.PC00.I2C1.PXTC})               // TD_P - Touch Device Power Package

          Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
          Alias(\_SB.PC00.I2C1.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

          Method(_PS0) { PS0X() }
          Method(_PS3) { PS3X() }
        }// End Of Scope(TPL1)
      }

    }//  Scope(\_SB.PC00.I2C1)

//
// PCH I2C2 - TouchPanel Power control
//
    Scope(\_SB.PC00.I2C2){

      /// PS0X Method, called by PS0 method in PchSerialIo.asl
      Method(PS0X,0,Serialized)
      {
      }
      /// PS3X Method, called by PS3 method in PchSerialIo.asl
      Method(PS3X,0,Serialized)
      {
      }
      /// \ref i2c2_pr_pxtc
      ///@defgroup i2c2_scope  I2C2 Scope
      PowerResource(PXTC, 0, 0){ /// Power rail for Touch Panel (GPP, H, 14)
        Method(_STA){
          Return(PSTA())
        }

        Method(_ON){
          PON()
        }

        Method(_OFF){
          POFF()
        }
      } // End PXTC
      /// Variable:
      Name(ONTM, 0) ///ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON
      Method(PSTA){
        If(LEqual(\_SB.GGOV(TPP1),1)) {
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(PON, 0) /// _ON Method \n Turn on
      {
        // De-Assert GPIO RST
        \_SB.SGOV(TPR1, 0)
        // drive pwr high
        \_SB.SGOV(TPP1, 1)
        // update ONTM
        Store(Timer(), ONTM)
        // enable int line
        \_SB.SGRA(GPI1, 1)
      }
      Method(POFF, 0)  /// _OFF method \n Turn off
      {
        // disable int line
        \_SB.SGRA(GPI1, 0)
        // drive pwr low
        \_SB.SGOV(TPP1, 0)
        // Assert GPIO RST
        \_SB.SGOV(TPR1, 1)
        // update ONTM
        Store(Zero , ONTM)  ///- Clear ONTM
      }

      If (LGreater (TPLT, Zero)) {
        Scope(TPL1){
          Name (TD_P, Package(){\_SB.PC00.I2C2.PXTC})               // TD_P - Touch Device Power Package

          Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
          Alias(\_SB.PC00.I2C2.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

          Method(_PS0) { PS0X() }
          Method(_PS3) { PS3X() }
        }// End Of Scope(TPL1)
      }

    }//  Scope(\_SB.PC00.I2C2)


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
      If(LNotEqual(HGMD,2)) {
        // X4 PCIe Connector (SLOT1) wake event
        If (\_SB.ISME(PSWP))
        {
          \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
          If(LNotEqual(\_SB.PC00.PEG0.VDID,0xFFFFFFFF)) { // If AHCI/NVMe drive is present on PEG0 port
            Notify(\_SB.PC00.PEG0, 0x02)   // device wake
          }
          If(LNotEqual(\_SB.PC00.RP05.VDID, 0xFFFFFFFF)) {
            Notify(\_SB.PC00.RP05, 0x02)   // device wake
          }
          \_SB.CAGS(PSWP)
        }
      }
      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP03, 0x02)      // device wake
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
        Notify(\_SB.PC00.RP08, 0x02)     // device wake
        \_SB.CAGS(PSW2)    // Clear GPE event status
      }
    }
  } //Scope(\_GPE)
//GPE Event handling - End

} // End SSDT
