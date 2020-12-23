/** @file
  ACPI RTD3 SSDT table for EHL U DDR4 CRB

 Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
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
//External(HGMD)
External(\ECON, IntObj)
External(\WIRC, IntObj)
External(\WGRC, IntObj)
External(\WWRC, IntObj)
External(SDS9)
External(WRTO)
External(PRST)
External(WPRP)
External(GBED)

Include ("Rtd3Common.asl")

External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)
External(PEPC)
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
External(\_SB.PEPD, DeviceObj)
External(\_SB.GHPO, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.PC00.RP07.PXSX.WIST, MethodObj)
External(\_SB.GBTR, MethodObj)
External(\_SB.BTRK, MethodObj)
External(\_SB.PC00.I2C0, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C0.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C0.TPD0, DeviceObj) // Touch pad
External(\_SB.PC00.I2C2, DeviceObj) //I2C2 Controller
External(\_SB.PC00.I2C2.TPL1, DeviceObj) // Touch panel 2
External(\_SB.PC00.I2C0.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C1.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C2.TPL1._STA, MethodObj)
External(\_SB.PC00.I2C0.TPD0._STA, MethodObj)
External(\_SB.PC00.UA00.BTH0, DeviceObj)
#define WWAN_PCIE_ROOT_PORT \_SB.PC00.RP04
External(WWAN_PCIE_ROOT_PORT.PXSX, DeviceObj) // For WWAN
External(\_SB.PC00.RP09.PXSX, DeviceObj)
External(\_SB.PC00.RP13.PXSX, DeviceObj)
External(WWAN_PCIE_ROOT_PORT.LASX)

// PCIe root ports - START

  ///
  /// PCIE RTD3 - PCIE SLOT 1 - X4 CONNECTOR
  ///
  //If(LNotEqual(HGMD,2)) {
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
      Include("Rtd3Pcie.asl")
    }
  //}

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
        Include("Rtd3PcieSsdStorage.asl")
      }
    }

    ///
    /// PCIe RP13 RTD3 - M.2 SSD RTD3
    ///
    Scope(\_SB.PC00.RP13) {
      Name(RSTG, Package() {0, 0})
      Store(SD2R, Index(RSTG, 0))
      Store(SDR1, Index(RSTG, 1))
      Name(PWRG, Package() {0, 0})
      Store(SD2P, Index(PWRG, 0))
      Store(SDP1, Index(PWRG, 1))
      Name(WAKG, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(WAKP, 0)             // must be defined due to compiler bug, will be removed when fixed https://bugs.acpica.org/show_bug.cgi?id=1432
      Name(SCLK, 0)
      Include("Rtd3Pcie.asl")
      Scope(\_SB.PC00.RP13.PXSX) {
        Include("Rtd3PcieSsdStorage.asl")
      }
    }

  ///
  /// PCIE RTD3 - PCIE SLOT 2 - X1 CONNECTOR
  ///
  //If(LNotEqual(HGMD,2)) {
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
      Include("Rtd3Pcie.asl")
    }
  //}

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

    Scope(\_SB.PC00.XHCI){

      /// USPP: Both HS/SS Ports are disconnected=1, else=0, updated inside PS2X/PS0X method call \n
      /// Bit 0 : USB SD Card Reader \n
      /// Bit 1 : USB(HS1 : 0x480 /SS0 : 0x510) \n
      /// BIT 2 : USB(HS2 : 0x490 / SS1 : 0x520) \n
      /// UPWR: Control power status 1=OFF, 0=ON \n
      Name(UPWR,0)
      Name(USPP,0)

      /// @defgroup xhc_duam        XHCI Device User Absent Mode (DUAM) method
      Method(DUAM,0,Serialized)
      {
        OperationRegion (XHCM, SystemMemory, (MEMB & 0xFFFF0000), 0x600)
        Field (XHCM, DWordAcc, NoLock, Preserve)
        {
          Offset(0x2),
          XHCV,16,
          Offset(0x480),
          HP01, 1, // HS port 1
          Offset(0x490),
          HP02, 1, // HS port 2
          Offset(0x530),
          SP00, 1, // SS port 0
          Offset(0x540),
          SP01, 1, // SS port 1
        }

      if(\UAMS != 0x00) /// if User not present
      {
        \_SB.PC00.XHCI.UPWR = 0x00 // Store(0x00,\_SB.PC00.XHCI.UPWR)
        /// Enterng CS, Remove power if SD card not present

        If(XHCV == 0xFFFF) ///- If Controller in D3Hot(MEM_BASE not decoded), remove power base on USPP flags
        {
          ///- >>check USPP BIT0:2 to determine power off criteria
          ///- >>if criteria met, turn off power and update UPWR

          If(\_SB.PC00.XHCI.USPP & 0x02) // USB(HS1 : 0x480 /SS0 : 0x530)
          {
            (\_SB.PC00.XHCI.UPWR |= 0x02)
          }
          If(\_SB.PC00.XHCI.USPP & 0x04) // USB(HS2 : 0x490 / SS1 : 0x520)
          {
            (\_SB.PC00.XHCI.UPWR |= 0x04)
          }
        ///- If Controller is not in D3hot (XHCI BAR is decoded)
        }Else{ //    If(Lequal(XHCV,0xFFFF))

          ///- >>check currect port connect status to determine power off criteria
          ///- >>if criteria met, turn off power and update UPWR

          If((HP01 == 0x00) && (SP00 == 0x00)) // USB(HS1 : 0x480 /SS0 : 0x510)
          {
            (\_SB.PC00.XHCI.UPWR |= 0x02)
          }
          If((HP02 == 0x00) && (SP01 == 0x00)) // USB(HS2 : 0x490 / SS1 : 0x520)
          {
            (\_SB.PC00.XHCI.UPWR |= 0x04)
          }
        }//    If(XHCV == 0xFFFF)
      }Else{ //if(\UAMS != 0x00)
        /// Exiting CS , Apply power if power removed
        if(\_SB.PC00.XHCI.UPWR & 0x02) // USB(HS1 : 0x480 /SS0 : 0x510)
        {
        }
        if(\_SB.PC00.XHCI.UPWR & 0x04) // USB(HS2 : 0x490 / SS1 : 0x520)
        {
        }
      } //if(\UAMS != 0x00) // User not present

        /// @addtogroup xhc_duam
      } // End of DUAM
    }

    Scope(\_SB.PC00.XHCI.RHUB){ //USB XHCI RHUB
      /// PS0X method is called by PS0 method in PchXhci.asl
      Method(PS0X,0,Serialized)
      {
        \_SB.PC00.XHCI.USPP = 0x00 // Store(0x00,\_SB.PC00.XHCI.USPP)  ///- clear all bits in USPP
      }

      /// PS2X method is called by PS2 method in PchXhci.asl
      /// It updates USPP variable base on USB port current connect status
      Method(PS2X,0,Serialized)
      {
        OperationRegion (XHCM, SystemMemory, (MEMB & 0xFFFFFFFFFFFF0000), 0x600)
        Field (XHCM, DWordAcc, NoLock, Preserve)
        {
          Offset(0x2),
          XHCV,16,
          Offset(0x480),
          HP01, 1, // HS port 1
          Offset(0x490),
          HP02, 1, // HS port 2
          Offset(0x530),
          SP00, 1, // SS port 0
          Offset(0x540),
          SP01, 1, // SS port 1
        }

        If(XHCV == 0xFFFF) /// Check if Controller in D3Hot(MEM_BASE not decoded)
        {
          Return() ///- if yes, no MMIO access, do nothing and returns
        }
        If((HP01 == 0x00) && (SP00 == 0x00)) /// Check USB ports (HS1 : 0x480 /SS0 : 0x510) (0: not connected)
        {
          (\_SB.PC00.XHCI.USPP |= 0x02)  ///- Set USPP BIT1
        }
        If((HP02 == 0x00) && (SP01 == 0x00)) /// Check USB ports (HS2 : 0x490 / SS1 : 0x520) (0: not connected)
        {
          (\_SB.PC00.XHCI.USPP |= 0x04) ///- Set USPP BIT2
        }
      } // End of PS2 method
      ///@defgroup xhc_rhub_ps2x XHCI RHUB PS2X Method

      /// PS3X method is called by _PS3 method in PchXhci.asl
      Method(PS3X,0,Serialized)
      {
      } // End of PS3 method
      ///@defgroup xhc_rhub_ps3x XHCI RHUB PS3X Method
    }

    If(LNotEqual(And(XHPR,0xF),0x0)){
      Scope(\_SB.PC00.XHCI.RHUB) {
        PowerResource(PX01, 0, 0){ /// power rail for USB3.0 ports 1 - NA
          Method(_STA){ /// _STA Method
            Return(0x00)
          }

          Method(_ON, 0)    /// _ON Method
          { /// Turn on
          }

          Method(_OFF, 0)   /// _OFF Method
          { /// Turn off
          }
          ///@defgroup xhc_pr_px01 XHCI Power Resource PX01
        } // End PX01
      } // End \_SB.PC00.XHCI.RHUB
      }

    If((XHPR & 0xF0) != 0x0){
      Scope(\_SB.PC00.XHCI.RHUB) {
        PowerResource(PX02, 0, 0){ /// Power rail for USB3.0 ports 2&3&5 - NA
          Method(_STA){     /// _STA method
            Return(0x00)
          }
          Method(_ON, 0)    /// _ON Method
          {
          }

          Method(_OFF, 0)   /// _OFF Method
          {
          }
        } // End PX02
      }
    }

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
      Method(_PR2) {
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
      OperationRegion (GENR, SystemMemory, ((XDCB & 0xFFFFFFFFFFFFFF00) + 0x10F81C), 0x4)  //AON MMIO - 10F81C: APBFC_U3PMU_CFG5 //Add(And(XDCB, 0xFF000000), 0x10F81C)
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

        If(LNotEqual(DVID, 0xFFFF)) {
          \_SB.CSD0(MODPHY_SPD_GATING_XDCI)
        }
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

If (GBES != 0) {
  Scope(\_SB.PC00.GLAN)
  {
    Method (_PS3, 0, NotSerialized)
    {
      \_SB.CSD3(MODPHY_SPD_GATING_GBE)
    }
    Method (_PS0, 0, NotSerialized)
    {
      If(!GBED){  // If GBE_FDIS_PMC == 0
        \_SB.CSD0(MODPHY_SPD_GATING_GBE)
      }
    }
  } // Scope(\_SB.PC00.GLAN)
}

//
// Human Interface Devices Start
//

#if 0
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
        If(\_SB.GGOV(GPLP) == 1) {
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
      Method(PON, 0) /// _ON Method \n Turn on
      {
        // De-Assert GPIO RST
        \_SB.SGOV(GPLR, 0)
        // drive pwr high
        \_SB.SGOV(GPLP, 1)
        // update ONTM
        Store(Timer(), ONTM)
        // enable int line
        //\_SB.SGRA(GPLI, 1) //disable
      }
      Method(POFF, 0)  /// _OFF method \n Turn off
      {
        // disable int line
        //\_SB.SGRA(GPLI, 0) //disable
        // drive pwr low
        \_SB.SGOV(GPLP, 0)
        // Assert GPIO RST
        \_SB.SGOV(GPLR, 1)
        // update ONTM
        ONTM = Zero // Store(Zero , ONTM)  ///- Clear ONTM
      }

      Scope(TPL1){
        Name (TD_P, Package(){\_SB.PC00.I2C1.PXTC})                                  // TD_P - Touch Device Power Package

        Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
        Alias(\_SB.PC00.I2C1.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

        Include("Rtd3I2cTouchDev.asl")
        Method(_PS0) { PS0X() }
        Method(_PS3) { PS3X() }
      }// End Of Scope(TPL1)


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
        //\_SB.SGRA(GPI1, 1) //disable
      }
      Method(POFF, 0)  /// _OFF method \n Turn off
      {
        // disable int line
        //\_SB.SGRA(GPI1, 0) //disable
        // drive pwr low
        \_SB.SGOV(TPP1, 0)
        // Assert GPIO RST
        \_SB.SGOV(TPR1, 1)
        // update ONTM
        Store(Zero , ONTM)  ///- Clear ONTM
      }

      If (TPLS == TCH_PNL_BUS_SPEED_1M) {  // If touch panel's bus speed is 1MHz?
        Scope(TPL1){
          Name (TD_P, Package(){\_SB.PC00.I2C2.PXTC})                                  // TD_P - Touch Device Power Package

          Alias(IC1D, TD_D)                                         // TD_D - Touch Device power on delay
          Alias(\_SB.PC00.I2C2.ONTM, TD_C)                          // TD_C - Touch Device I2C controller power on timestamp

          Include("Rtd3I2cTouchDev.asl")
          Method(_PS0) { PS0X() }
          Method(_PS3) { PS3X() }
        }// End Of Scope(TPL1)
      }
    }//  Scope(\_SB.PC00.I2C2)
#endif //Touchpad

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
          _STA = One // Store(One, _STA)        ///- Set Logocal power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
                    ///@addtogroup pr_paud
        } // End _ON

        Method(_OFF, 0){    /// _OFF method \n
          _STA = Zero // Store(Zero, _STA)    ///- Set the current power state
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
          If ((^_STA == Zero) && (\UAMS != 0)) { ///New state = OFF Check if (_STA ==0 && \UAMS != 0) \n
          } Else { /// New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
            /// Turn power on \n
            If(^PSTA != One) { ///- Skip below if Power Resource is already in ON
              ^PSTA = 1 // Store(One, ^PSTA)  ///- >> Set PSTA to 1
              ^ONTM = Timer() // Store(Timer(), ^ONTM) ///- >> Start the timer for this PR
            }
          }
        ///@defgroup pr_paud_puam Power Resource User Absent Mode for onboard Audio CODEC
        } //PUAM
      } //PAUD
    } //Scope(\_SB.PC00)

//
// Check HDAS (HD-Audio) controller present
//
    If(\_SB.PC00.HDAS.VDID != 0xFFFFFFFF) {
      Scope(\_SB.PC00.HDAS) {
        Method(PS0X,0,Serialized)     /// Platform D0 Method for HD-A Controller
        {
          If(\_SB.PC00.PAUD.ONTM == 0){    ///- Check if ONTM=0
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
          Local0 = (Timer() - \_SB.PC00.PAUD.ONTM) / 10000 // Divide(Subtract(Timer(), \_SB.PC00.PAUD.ONTM), 10000, , Local0) ///- Store Elapsed time in ms, ignore remainder
          Local1 = AUDD + VRRD //Add(AUDD, VRRD, Local1) ///- Incorporate VR Rampup Delay
          If(Local0 < Local1) { ///- Do not sleep if already past the delay requirement audio
            ///- Delay for device init
            Sleep(Local1 - Local0) //Sleep(Subtract(Local1, Local0)) ///- Sleep (AUDD + VRRD - time elapsed)
          }
        }

        ///Associate _PR0 with \ref pr_paud
        Name(_PR0, Package(){\_SB.PC00.PAUD})
      ///@defgroup hdef_scope       Intel High Definition Audio Scope
      }
    }// If(\_SB.PC00.HDAS.VDID != 0xFFFFFFFF)
//GPE Event handling - Start
  Scope(\_GPE) {
    //
    // Alternate _L6F(), to handle 2-tier RTD3 GPE events here
    //
    Method(AL6F) {
      //If(LNotEqual(HGMD,2)) {
        // X4 PCIe Connector (SLOT1) wake event
        If (\_SB.ISME(PSWP))
        {
          \_SB.SHPO(PSWP, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
          Notify(\_SB.PC00.RP01, 0x02)      // device wake
          \_SB.CAGS(PSWP)
        }
      //}

      // WLAN wake event
      If (\_SB.ISME(WLWK))
      {
        \_SB.SHPO(WLWK, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.RP07, 0x02)      // device wake
        \_SB.CAGS(WLWK)    // WIFI_WAKE_N
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
