/** @file
  ACPI DSDT table

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "PlatformBase.h"

#define TRAP_TYPE_DTS     0x02
#define TRAP_TYPE_BGD     0x04  // BIOS Guard

External(\HGMD) //Hybrid graphics

//
// DTS externals
//
External(\_SB.TRPD)
External(\_SB.TRPF)

//
// BIOS Guard externals
//
External(\_SB.BGMA)
External(\_SB.BGMS)
External(\_SB.BGIA)
//
// Intel(R) Dynamic Tuning Technology Externals
//
External(\_SB.PC00.LPCB.H_EC.CHRG,DeviceObj)
External(\_SB.IETM, DeviceObj)

External(\_SB.PC00.GFX0.TCHE)   // Technology enabled indicator
External(\_SB.PC00.GFX0.STAT)   // State Indicator
External(\_SB.PC00.GFX0.IUEH, MethodObj)

External(\_SB.TPM.PTS, MethodObj)
External(\_SB.PC00.PAUD.PUAM, MethodObj) //PUAM - PowerResource User Absent Mode

External(\_SB.PC00.LPCB.H_EC.CFAN, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.LSTE, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.VPWR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.RPWR, FieldUnitObj)

External(\_SB.PC00.LPCB.H_EC.WTMM, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.WTMV, FieldUnitObj)

External(\_SB.PC00.LPCB.H_EC.ECWT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ECRD, MethodObj)
External(\_SB.PC00.LPCB.H_EC.UPBT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.UTEC, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ONTM, IntObj)
External(\_SB.PC00.LPCB.H_EC.BNUM, IntObj)
External(ISAR, MethodObj, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})

#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7
#define TBT_DATA_WIDTH       4 // This represent DATA WIDTH for Each TBT Controller in a Variable

Name(ECUP, 1)  // EC State indicator: 1- Normal Mode 0- Low Power Mode
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State

External(TADL)
External(TADH)
External(PG3S)
External(\EPTU, FieldUnitObj)

External(SPTS, MethodObj)
External(SPNT, MethodObj)
External(SWAK, MethodObj)

External (\_SB.PC00.TDM0, DeviceObj)
External (\_SB.PC00.TDM1, DeviceObj)
External (\_SB.PC00.TDM0.STCM, MethodObj)
External (\_SB.PC00.TDM1.STCM, MethodObj)

// Create a Global MUTEX.
Mutex(MUTX, 0)
// OS Up mutex
Mutex(OSUM, 0)
// _WAK Finished Event
Event(WFEV)

// Define Port 80 as an ACPI Operating Region to use for debugging.  Please
// note that the Intel CRBs have the ability to ouput a Word to
// Port 80h for debugging purposes, so the model implemented here may not be
// able to be used on OEM Designs.

OperationRegion(PRT0,SystemIO,0x80,2)
Field(PRT0,WordAcc,Lock,Preserve)
{
  P80B, 16
}

// Port 80h Update:
//    Update 2 bytes of Port 80h.
//
//  Arguments:
//    Arg0: 0 = Write Port 80h
//          1 = Write Port 81h
//    Arg1: 8-bit Value to write
//
//  Return Value:
//    None

Name(P80T, 0) // temp buffer for P80

Method(D8XH,2,Serialized)
{
  If(LEqual(Arg0,0))    // Write Port 80h
  {
    Store(Or(And(P80T,0xFF00),Arg1),P80T)
  }
  If(LEqual(Arg0,1))    // Write Port 81h
  {
    Store(Or(And(P80T,0x00FF),ShiftLeft(Arg1,8)),P80T)
  }
  Store(P80T,P80B)
}

//
// Wrapper of D8XH, enabled only when ACPI debug enabled
//
Method(P8XH,2,Serialized)
{
  If(CondRefOf(MDBG)) { // Check if ACPI Debug SSDT is loaded
    D8XH(Arg0,Arg1)
  }
}


//
// Define SW SMI port as an ACPI Operating Region to use for generate SW SMI.
//
OperationRegion(SPRT,SystemIO, 0xB2,2)
Field (SPRT, ByteAcc, Lock, Preserve) {
  SSMP, 8
}

// Operational region for ACPI Control (SMI_EN) access
//
OperationRegion (SMIN, SystemIO, (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), 0x4)
Field (SMIN, AnyAcc, NoLock, Preserve)
{
  SMIE, 32
}

// The _PIC Control Method is optional for ACPI design.  It allows the
// OS to inform the ASL code which interrupt controller is being used,
// the 8259 or APIC.  The reference code in this document will address
// PCI IRQ Routing and resource allocation for both cases.
//
// The values passed into _PIC are:
//   0 = 8259
//   1 = IOAPIC

Method(\_PIC,1)
{
  Store(Arg0,GPIC)
  Store(Arg0,PICM)
}

// Prepare to Sleep.  The hook is called when the OS is about to
// enter a sleep state.  The argument passed is the numeric value of
// the Sx state.

Method(_PTS,1)
{
  D8XH(0,Arg0)    // Output Sleep State to Port 80h, Byte 0.
  D8XH(1,0)       // output byte 1 = 0, sleep entry


  // If code is executed, Wake from RI# via Serial Modem will be
  // enabled.  If code is not executed, COM Port Debugging throughout
  // all Sx states will be enabled.

  If(LEqual(Arg0,3))
  {
    Store(CPWE, CWEF)  // save GPE0_EN_127_96_CPU_WAKE_EN value to NVS
  }

  // If S4 or S5 state and PG3 enabled then sa
  ElseIf(LOr(LEqual(Arg0, 4), LEqual(Arg0, 5)))
  {
    If (LEqual(PG3S, 1)) {
      If (CondRefOf(\_SB.PC00.LPCB.H_EC.UTEC)) {
        //Programming BIOS to send DC timer mode only .Other timer mode will be sent as per future requirements.
        // 1. Program Timer mode to 0x00 : WADT_AC register
        //\_SB.PC00.LPCB.H_EC.UTEC(0x00, \ACWA)

        // 2. Program Timer mode to 0x01 : WADT_DC register
        \_SB.PC00.LPCB.H_EC.UTEC(0x01, \DCWA)

        // 3. Program Timer mode to 0x10 : WADT_EXP_AC register
        //\_SB.PC00.LPCB.H_EC.UTEC(0x10, \ACET)

        // 4. Program Timer mode to 0x11 : WADT_EXP_DC register
        //\_SB.PC00.LPCB.H_EC.UTEC(0x11, \DCET)
      }
    }
  }

  // Generate a SW SMI trap to save some NVRAM data back to CMOS.

  //  Don't enable IGD OpRegion support yet.
  //  TRAP(1, 81)
  //
  // Call TPM.PTS
  //
  If(CondRefOf(\_SB.TPM.PTS))
  {
    //
    // Call TPM PTS method
    //
    \_SB.TPM.PTS (Arg0)
  }

  // Call SOC Prepare to Sleep
  SPTS (Arg0)
}

// Platform Hook for _BCL method for Igfx.
// Platform can also add logic here  to create run time creation of package
// example: return package without 0-10 values when in tablet mode,else return all values 0-100.
Method(PBCL ,0)
{
  //
  // List of supported brightness levels in the following sequence.
  // Level when machine has full power.
  // Level when machine is on batteries.
  // Other supported levels.
  //
  Return(Package(){80, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100})
}

// Wake.  This hook is called when the OS is about to wake from a
// sleep state.  The argument passed is the numeric value of the
// sleep state the system is waking from.

Method(_WAK,1,Serialized)
{
  D8XH(1,0xAB)    // Beginning of _WAK.

  ADBG("_WAK")

  Name (RPWM, 0) // Root Port Wake Mask

  If (LEqual(\DSTS, 0)) { // Undocked
    Store (0, RPWM)
  } Else {
    Store (0x180, RPWM) // Skip RP 7 and 8
  }

  // Call SOC Wake
  SWAK (Arg0, RPWM)

  If(LEqual(Arg0,3))
  {
    // Turn off the CPU Fan for all OSes if Active Cooling
    // is disabled.
    If(LEqual(0,ACTT))
    {
      If(LEqual(\ECON,1))
      {
        \_SB.PC00.LPCB.H_EC.ECWT(0, RefOf(\_SB.PC00.LPCB.H_EC.CFAN))
      }
    }
  }

  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {

    //
    // Check to send Convertible/Dock state changes upon resume from Sx.
    //
    If(And(GBSX,0x40))
    {
      \_SB.PC00.GFX0.IUEH(6)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit3 of PB1E(Slate/Notebook status)
      //
      Xor(PB1E, 0x08, PB1E)

      //
      // Update button status.
      //
      If (\ECON)
      {
        If(And(PB1E, 0x08)){
          If(CondRefOf(\_SB.PC00.LPCB.H_EC.UPBT))
          {
            \_SB.PC00.LPCB.H_EC.UPBT(CONVERTIBLE_BUTTON, One)
          }
        }
        Else
        {
          If(CondRefOf(\_SB.PC00.LPCB.H_EC.UPBT))
          {
            \_SB.PC00.LPCB.H_EC.UPBT(CONVERTIBLE_BUTTON, Zero)
          }
        }
      }
    }

    If(And(GBSX,0x80))
    {
      \_SB.PC00.GFX0.IUEH(7)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit4 of PB1E (Dock/Undock status)
      //
      Xor(PB1E, 0x10, PB1E)

      //
      // Update button status.
      //
      If (\ECON)
      {
        If(And(PB1E, 0x10))
        {
          If(CondRefOf(\_SB.PC00.LPCB.H_EC.UPBT))
          {
            \_SB.PC00.LPCB.H_EC.UPBT(DOCK_INDICATOR, One)
          }
        }
        Else
        {
          If(CondRefOf(\_SB.PC00.LPCB.H_EC.UPBT))
          {
            \_SB.PC00.LPCB.H_EC.UPBT(DOCK_INDICATOR, Zero)
          }
        }
      }
    }

    //
    // Prevent reinitializing Virtual GPIO Button status
    //
    If (\ECON)
    {
      If(CondRefOf(\_SB.PC00.LPCB.H_EC.ONTM))
      {
        Store(1, \_SB.PC00.LPCB.H_EC.ONTM)
      }
    }

    If(LEqual(\ECON,1))
    {
//@todo: Waiting for DOCK offect data
//      // Detect the change of Dock state
//      If(LNotEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.DOCK)), \DSTS))
//      {
//        Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.DOCK)), \DSTS)
//        If(LEqual(\DSTS, 1))  // Docked.
//        {
//          If(LEqual(Arg0,3))
//          {
//            Sleep(1000) // Delay 1 second for hot docking stability
//            Store(DKSM, \SSMP)
//            Sleep(1000) // Delay 1 second for hot docking stability
//          }
//          Notify(\_SB.PC00.DOCK, 0)
//        }
//        Else      // Undocked.
//        {
//          Notify(\_SB.PC00.DOCK, 1)
//        }
//      }

      // Detect the change of Power State.
      If(LEqual(\_SB.PC00.LPCB.H_EC.BNUM,0))
      {
        If(LNotEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.VPWR)),PWRS))
        {
          Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.VPWR)),PWRS)
          // Perform needed ACPI Notifications.
          SPNT()
        }
      }
      Else
      {
        If(LNotEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.RPWR)),PWRS))
        {
          Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.RPWR)),PWRS)
          // Perform needed ACPI Notifications.
          SPNT()
        }
      }
    }
  } // If S3 or S4
  If(LEqual(\ECON,1)) {
    If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
    {
      //
      // If Using Control Method Power Button, notify PWRD device with 0x2
      //
#if FixedPcdGetBool(PcdAdlNSupport) == 0
      If(LEqual(\_SB.PWRB.PBST, 0x1)) {
        If(PBSS) { //Power Button woke the system
          Notify(\_SB.PWRB, 0x02) // Send release notification to Power Button device 0x02
          Store(1, PBSS)
        }
      }
#endif
    }
  }

  Return(Package(){0,0})
}

// Get Buffer:
//    This method will take a buffer passed into the method and
//    create then return a smaller buffer based on the pointer
//    and size parameters passed in.
//
//  Arguments:
//    Arg0: Pointer to start of new Buffer in passed in Buffer.
//    Arg1: Size of Buffer to create.
//    Arg2: Original Buffer
//
//  Return Value:
//    Newly created buffer.
Method(GETB,3,Serialized)
{
  Multiply(Arg0,8,Local0)     // Convert Index.
  Multiply(Arg1,8,Local1)     // Convert Size.
  CreateField(Arg2,Local0,Local1,TBF3)  // Create Buffer.

  Return(TBF3)        // Return Buffer.
}

//
// Memory window to the CTDP registers starting at MCHBAR+5000h.
//
  OperationRegion (CPWR, SystemMemory, Add(\_SB.PC00.GMHB(),0x5000), 0x1000)
  Field (CPWR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x938), // PACKAGE_POWER_SKU_UNIT (MCHBAR+0x5938)
    PWRU,  4,       // Power Units [3:0] unit value is calculated by 1 W / Power(2,PWR_UNIT). The default value of 0011b corresponds to 1/8 W.
    Offset (0x9A0), // TURBO_POWER_LIMIT1 (MCHBAR+0x59A0)
    PPL1, 15,       // PKG_PWR_LIM_1 [14:0]
    PL1E,1,         // PKG_PWR_LIM1_EN [15]
    CLP1,1,         // Package Clamping Limitation 1
  }
Name(CLMP, 0) // save the clamp bit
Name(PLEN,0) // save the power limit enable bit
Name(PLSV,0x8000) // save value of PL1 upon entering CS
Name(CSEM, 0) //semaphore to avoid multiple calls to SPL1.  SPL1/RPL1 must always be called in pairs, like push/pop off a stack
//
// SPL1 (Set PL1 to 4.5 watts with clamp bit set)
//   Per Legacy Thermal management CS requirements, we would like to set the PL1 limit when entering CS to 4.5W with clamp bit set via MMIO.
//   This can be done in the ACPI object which gets called by graphics driver during CS Entry.
//   Likewise, during CS exit, the BIOS must reset the PL1 value to the previous value prior to CS entry and reset the clamp bit.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(SPL1,0,Serialized)
{
    Name(PPUU,0) // units
    If (LEqual(CSEM, 1))
    {
      Return() // we have already been called, must have CS exit before calling again
    }
    Store(1, CSEM) // record first call

    Store (PPL1, PLSV) // save PL1 value upon entering CS
    Store (PL1E, PLEN) // save PL1 Enable bit upon entering CS
    Store (CLP1, CLMP) // save PL1 Clamp bit upon entering CS

    If (LEqual(PWRU,0)) {  // use PACKAGE_POWER_SKU_UNIT - Power Units[3:0]
      Store(1,PPUU)
    } Else {
      ShiftLeft(Decrement(PWRU),2,PPUU) // get units
    }

    Multiply(PLVL,PPUU,Local0)  // convert SETUP value to power units in milli-watts
    Divide(Local0,1000,,Local1) // convert SETUP value to power units in watts
    Store(Local1, PPL1)   // copy value to PL1
    Store(1, PL1E)     // set Enable bit
    Store(1, CLP1)     // set Clamp bit
}
//
// RPL1 (Restore the PL1 register to the values prior to CS entry)
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(RPL1,0,Serialized)
{
    Store (PLSV, PPL1)  // restore value of PL1 upon exiting CS
    Store(PLEN, PL1E)   // restore the PL1 enable bit
    Store(CLMP, CLP1)   // restore the PL1 Clamp bit
    Store(0, CSEM)      // restore semaphore
}

Name(UAMS, 0) // User Absent Mode state, Zero - User Present; non-Zero - User not present
Name(GLCK, 0) // a spin lock to avoid multi execution of GUAM
// GUAM - Global User Absent Mode
//    Run when a change to User Absent mode is made,  e.g. screen/display on/off events.
//    Any device that needs notifications of these events includes its own UAMN Control Method.
//
//    Arguments:
//      Power State:
//        00h = On
//        01h = Standby
//        other value = do nothing & return
//
//    Return Value:
//      None
//
Method(GUAM,1,Serialized)
{
  Switch(ToInteger(Arg0))
  {
    Case(0) // exit CS
    {
      If(LEqual(GLCK, 1)){
        store(0, GLCK)

        P8XH(0, 0xE1)
        P8XH(1, 0xAB)

        If(PSCP){
          // if P-state Capping s enabled
          If (CondRefOf(\_SB.PR00._PPC))
          {
            Store(Zero, \_SB.CPPC)
            SPNT()
          }
        }
        If(PLCS){
          RPL1() // restore PL1 to pre-CS value upon exiting CS
        }
      } // end GLCK=1
    } // end case(0)

    Case(1) // enter CS
    {
      If(LEqual(GLCK, 0)){
        store(1, GLCK)

        P8XH(0, 0xE0)
        P8XH(1, 00)

        If(PSCP){
          // if P-state Capping is enabled
          If (LAnd(CondRefOf(\_SB.PR00._PSS), CondRefOf(\_SB.PR00._PPC)))
          {
            SPNT()
          }
        }
        If(PLCS){
          SPL1() // set PL1 to low value upon CS entry
        }
      } // end GLCK=0
    } // end case(1)
    Default{
      Return()  // do nothing
    }
  } // end switch(arg0)

  Store(LAnd(Arg0, LNot(PWRS)), UAMS)  // UAMS: User Absent Mode state, Zero - User Present; non-Zero - User not present
  P_CS()                               // Powergating during CS

} // end method GUAM()

// Power CS Powergated Devices:
//    Method to enable/disable power during CS
Method(P_CS,0,Serialized)
{
    // NOTE: Do not turn ON Touch devices from here. Touch does not have PUAM
    If(CondRefOf(\_SB.PC00.PAUD.PUAM)){           // Notify Codec(HD-A/ADSP)
        \_SB.PC00.PAUD.PUAM()
    }
}

// SMI I/O Trap:
//    Generate a Mutex protected SMI I/O Trap.
//
//  Arguments:
//    Arg0: I/O Trap type.
//               2 - For DTS
//               4 - For BIOS Guard Tools
//    Arg1: SMI I/O Trap Function to call.
//
//  Return Value:
//    SMI I/O Trap Return value.
//      0 = Success.  Non-zero = Failure.
Method(TRAP,2,Serialized)
{
  Store(Arg1,SMIF)        // Store SMI Function.

  If(LEqual(Arg0,TRAP_TYPE_BGD))  // Is BIOS Guard TOOLS IO Trap?
  {
    Store(0,\_SB.TRPF)         // Generate IO Trap
  }

  Return(SMIF)            // Return SMIF.  0 = Success.
}

// Note:  Only add the indicator device needed by the platform.

//
// System Bus
//
Scope(\_SB.PC00)
{

  Method(PTMA)
  {
    Return(\_SB.BGMA)
  }

  Method(PTMS)
  {
    Return(\_SB.BGMS)
  }

  Method(PTIA)
  {
    Return(\_SB.BGIA)
  }

  Method(_INI,0, Serialized)
  {
    // Determine the OS and store the value, where:
    //
    //   OSYS = 1000 = Operating systems except the following list.
    //   OSYS = 2001 = WINXP, RTM or SP1.
    //   OSYS = 2002 = WINXP SP2.
    //   OSYS = 2006 = Vista.
    //   OSYS = 2009 = Windows 7 and Windows Server 2008 R2.
    //   OSYS = 2012 = Windows 8 and Windows Server 2012.
    //   OSYS = 2013 = Windows Blue.
    //

    // Set Default 1000 when OSI method does not find a matching OS.
    Store(1000,OSYS)

    // Check for a specific OS which supports _OSI.

    If(CondRefOf(\_OSI))
    {
      If(\_OSI("Windows 2001")) // Windows XP
      {
        Store(2001,OSYS)
      }

      If(\_OSI("Windows 2001 SP1")) //Windows XP SP1
      {
        Store(2001,OSYS)
      }

      If(\_OSI("Windows 2001 SP2")) //Windows XP SP2
      {
        Store(2002,OSYS)
      }

      If (\_OSI( "Windows 2001.1"))  //Windows Server 2003
      {
        Store(2003,OSYS)
      }

      If(\_OSI("Windows 2006")) //Windows Vista
      {
        Store(2006,OSYS)
      }

      If(\_OSI("Windows 2009")) //Windows 7 and Windows Server 2008 R2
      {
        Store(2009,OSYS)
      }

      If(\_OSI("Windows 2012")) //Windows 8 and Windows Server 2012
      {
        Store(2012,OSYS)
      }

      If(\_OSI("Windows 2013")) //Windows 8.1 and Windows Server 2012 R2
      {
        Store(2013,OSYS)
      }

      If(\_OSI("Windows 2015")) //Windows 10
      {
        Store(2015,OSYS)
      }
    }
  }
}

Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //
  Name(PICM, 0)

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother")
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package

  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If(And(ShiftLeft(1, Arg1), Local0))
    {
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    }
    Else
    {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0)
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

    Return(PRWP)
  }
}

Scope (\_SB)
{
  Name (OSU4, 0)     // OS native USB4 support - 0: native USB4 is not supported, 1: native USB4 is supported
  Name (OSCM, 0xFF)  // CM mode applied for OS phase - 0 : FW CM, 1 - SW CM, 0xFF: uninitialied

  #define CMOS_CM_MODE_REG            0xF2

  If (LEqual (U4SE, 1))
  {
    //
    // Operation region for CMOS access
    //
    OperationRegion (CSIO, SystemIO, 0x72, 2)
    Field (CSIO, ByteAcc, NoLock, Preserve)
    {
      IO72, 8,
      IO73, 8
    }

    //
    // Name: WCMP
    // Description: Write CM mode to the platform settings
    // Input: Arg0 - CM mode value
    //
    Method (WCMP, 1, Serialized)
    {
      Store (CMOS_CM_MODE_REG, IO72)
      Store (Arg0, IO73)
    }
  }

  #define CM_MASK_ITBT_0              0x01
  #define CM_MASK_ITBT_1              0x02

  //
  // Name: DOCM
  // Description: Apply OS requested CM mode to host routers on platform
  // Return : CM applied status (0 = Apply CM mode successfully, 0xFF = CM mode is not applied)
  //
  // Local0 - CM applied status (0 = CM mode is applied to host router, 0xFF = CM mode is not applied to any host router)
  //
  Method (DOCM, 0, Serialized)
  {

    If (LEqual (U4SE, 0))
    {
      Return (0xFF)
    }


    //
    // Initialize CM applied status to 0xFF
    //
    Store (0xFF, Local0)

    //
    // If CM mode is not applied to any host router, OSCM is set to FW CM
    //
    Store (0, OSCM)

    //
    // Enabled host router mask (Bit 0-3: iTBT0-iTBT3, Bit 4-7: dTBT0-dTBT3)
    //
    ADBG (Concatenate ("DOCM: Enabled host router mask on platform = 0x", ToHexString (CMSK)))

    If (Lequal (Local0, 0xFF))
    {
      ADBG ("DOCM: CM mode is not applied to any host router, native USB4 capability is not supported")
    }

    //
    // Save the latest applied CM mode to the platform settings if OS Dependent option is selected in BIOS setup.
    // U4CM bit 6:4 = CM mode setup option
    // 0 = Firmware CM
    // 1 = Software CM
    // 2 = OS Dependent
    // 3 = CM Debug
    //
    If (LEqual (And (U4CM, 0x70), 0x20))
    {
      \_SB.WCMP (OSCM)
    }

    Return (Local0)
  }

  //
  // Name: U4FN
  // Description: Handle _OSC USB4 capability of platform-wide UUID
  // Arguments:
  // Arg0 - Status DWORD of _OSC Arg3
  // Arg1 - Capability DWORD of _OSC Arg3
  // Return : USB4 capability support status (0 = Accept _OSC USB4 request, 0xFF = USB4 capability bit should be cleared)
  //
  // Local0 - CM apply status (0 = CM mode is applied to host router, 0xFF = CM mode is not applied to any host router)
  // Local1 - DOCM method returned value
  // Local2 - CM mode setup option
  //

  Method (U4FN, 2, Serialized)
  {
    ADBG (Concatenate ("U4FN: _OSC STS = ", ToHexString (Arg0)))
    ADBG (Concatenate ("U4FN: _OSC CAP = ", ToHexString (Arg1)))


    If (LEqual (U4SE, 0))
    {
      Store (0, OSCM)

      //
      // Check OS natvie USB4 capability support
      //
      If (And (Arg1, 0x40000))
      {
        //
        // OS supports native USB4 capability, but platform doesn't support
        // Return 1 to indicate USB4 capability bit should be cleared
        //
        ADBG ("U4FN: USB4 capability bit should be cleared since platform doesn't support")
        Return (0xFF)
      }
      Else
      {
        ADBG ("U4FN: OS and platform doesn't support native USB4")
        Return (0)
      }
    } Else {

      Store (0, Local0)

      //
      // Check bit 18 for native USB4 support
      // If bit 18 is set, OS requests to apply SW CM
      // If bit 18 is not set, use FW CM
      // It's possible that bit 18 is set by OS in the beginning, but be cleared in the next call if bit 18 is cleared in _OSC.
      // For example:
      // 1. _OSC bit 18 set, query bit = 0  => OSU4 = 1, but FW CM is selected in BIOS setup, report bit 18 cleared in _OSC
      // 2. _OSC bit 18 cleared, query bit = 1 => OSU4 = 0
      //
      If (And (Arg1, 0x40000)) {
        //
        // OS supports native USB4
        //
        Store (1, OSU4)
      } Else {
        Store (0, OSU4)
      }

      //
      // CM mode has been FW CM in pre-boot if Firmware CM mode is selected in BIOS setup
      // CM mode switch will not be required in _OSC
      //
      // U4CM bit 6:4 = CM mode setup option
      // 0 = Firmware CM
      // 1 = Software CM
      // 2 = OS Dependent
      // 3 = CM Debug
      //
      If (LEqual (And (U4CM, 0x70), 0))
      {
        If (LEqual (OSU4, 1))
        {
          ADBG ("U4FN: Platform doesn't support SW CM, clear USB4 capability bit")
          Store (0xFF, Local0)
        } Else {
          ADBG ("U4FN: OS and platform only support FW CM")
          Store (0, Local0)
        }

        //
        // Return without CM mode switch since it's not required
        //
        Return (Local0)
      }

      //
      // Apply OS requested CM mode to host routers on platform if query bit is cleared
      //
      If (LEqual (And (Arg0, 0x01), 0))
      {
        Store (DOCM (), Local1)
        //
        // USB4 capability bit should be cleared if the bit is 1 but SW CM mode is not applied to host router successfully
        //
        If (LAnd (Lequal (OSU4, 1), Lequal (Local1, 0xFF)))
        {
          Store (0xFF, Local0)
        }
      } Else {
        //
        // Ideally, OS requested CM mode should be accepted
        // Except OS requests SW CM but no host router is enabled or CM switch failure happens
        //
        //
        // USB4 capability bit should be cleared if the latest applied CM mode for OS is FW CM
        //
        If (LAnd (Lequal (OSU4, 1), Lequal (OSCM, 0)))
        {
          ADBG ("U4FN: Native USB4 capability bit should be cleared since FW CM is applied for OS")
          Store (0xFF, Local0)
        }
      }

      Return (Local0)
    }
  } // Method (U4FN, 2, Serialized)
}   // Scope (\_SB)

Scope (\_SB)
{
  Name(OSCI, 0)  // \_SB._OSC DWORD2 input
  Name(OSCO, 0)  // \_SB._OSC DWORD2 output
  Name(OSCP, 0)  // \_SB._OSC CAPABILITIES

  // _OSC (Operating System Capabilities)
  //    _OSC under \_SB scope is used to convey platform wide OSPM capabilities.
  //    For a complete description of _OSC ACPI Control Method, refer to ACPI 5.0
  //    specification, section 6.2.10.
  // Arguments: (4)
  //    Arg0 - A Buffer containing the UUID "0811B06E-4A27-44F9-8D60-3CBBC22E7B48"
  //    Arg1 - An Integer containing the Revision ID of the buffer format
  //    Arg2 - An Integer containing a count of entries in Arg3
  //    Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //    A Buffer containing the list of capabilities
  //
  Method(_OSC,4,Serialized)
  {
    ADBG ("_OSC")

    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)

    //
    // Check UUID
    //
    If(LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
    {
      //
      // Check Revision
      //
      If(LEqual(Arg1,One))
      {
        Store(CAP0, OSCP)
        If(And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
          If(LNotEqual(And(HGMD,0x0F),2)) // Check Hybrid graphics is not enabled in bios setup [HgModeMuxless]?
          {
            If(LEqual(RTD3,0)) // Is RTD3 support disabled in Bios Setup?
            {
              // RTD3 is disabled via BIOS Setup.
              And(CAP0, 0xFFFFFFFB, CAP0) // Clear _PR3 capability
              Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
            }
          }
        }

        //
        // Process _OSC USB4 capability
        // Clear Native USB4 capability bit if 0xFF is returned
        //
        If (LEqual (U4FN (STS0, CAP0), 0xFF))
        {
          ADBG ("_OSC: Native USB4 capability bit is cleared")
          And (CAP0, 0xFFFBFFFF, CAP0) // Clear Native USB4 capability
          Or (STS0, 0x10, STS0) // Indicate capability bit is cleared
        }
      } Else {
        And (STS0, 0xFFFFFF00, STS0)
        Or (STS0, 0xA, STS0) // Unrecognised Revision and report OSC failure
      }  // Check Revision
    } ElseIf (LEqual (Arg0, ToUUID ("23A0D13A-26AB-486C-9C5F-0FFA525A575A"))) {
      //
      // Support USB capability UUID only if SW CM mode is applied.
      // OSCM is set to 1 only if CM mode is switched to SW CM successfully when _OSC platform-wide uuid with query bit cleared is called.
      //
      If (LEqual (OSCM, 1))
      {
        //
        // Point to the third DWORD of the Arg3 buffer (OSPM Control field for USB4)
        //
        CreateDWordField (Arg3, 8, CTRL)  // USB4 Control capabilities

        //
        // Check Revision
        //
        If (LEqual (Arg1, One))
        {

          //
          // Supported USB4 control fields:
          //   bit 0 - USB Tunneling
          //   bit 1 - DisplayPort Tunneling
          //   bit 2 - PCIe Tunneling
          //   bit 3 - Inter-Domain USB4 Internet Protocol
          //   bit 31:4 - Reserved
          //
          If (And (CTRL, 0xFFFFFFF0))
          {
            And (CTRL, 0x0F, CTRL)     // Clear unsupported bits
            Or (STS0, 0x10, STS0)      // Indicate capability bit is cleared
          }

          If (LEqual (EPTU, 0))
          {
            And (CTRL, 0x0B, CTRL)       // disable PCI Express Tunneling over USB4
            Or (STS0, 0x10, STS0)      // Indicate capability bit is cleared
          }
        } Else {
          And (STS0, 0xFFFFFF00, STS0)
          Or (STS0, 0xA, STS0) // Unrecognised Revision and report OSC failure
        } // If (LEqual (Arg1, One))
      } Else {
        And (STS0, 0xFFFFFF00, STS0)
        Or (STS0, 0x6, STS0) // Unrecognised UUID and report OSC failure
      }  // If (LEqual (OSCM, 1))
    } Else {
      And (STS0, 0xFFFFFF00, STS0)
      Or (STS0, 0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return (Arg3)
  } // End _OSC
} // End of Scope(\_SB)

//
// CS Wake up event support
//
Scope (\_SB)
{
  // Define Sleep button to put the system in sleep
  Device (SLPB)
  {
    Name (_HID, EISAID ("PNP0C0E"))
    Name (_STA, 0x0B)
    // Bit0 - the device is present: Yes.
    // Bit1 - the device is enabled and decoding its resources: Yes.
    // Bit2 - the device should be shown in the UI: No.
    // Bit3 - the device is functioning properly: Yes.
    // Bit4 - the battery is present: N/A
  }
} // End of Scope(\_SB)

//
// xPCS (Physical Coding Sublayer) IP in TSN
//
Scope (\_SB)
{
  Device (PCS0)
  {
    Name(_HID, "INTC1034")
    Name(_UID, 0x0)
    Method(_STA)
    {
      If (LEqual(TSD0, 1)) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method (_CRS, 0x0, NotSerialized) {
      Name(PCSR,ResourceTemplate(){
        Memory32Fixed (ReadWrite, 0x0,0x4, MDAL)
        Memory32Fixed (ReadWrite, 0x0,0x4, MDDL)
      })

      //
      // Access to Registers in PCS unit indirectly through MDIO Link
      //
      CreateDWordField(PCSR,MDAL._BAS, MAL0)
      Store( Add(\_SB.PC00.GTSN.TADL,0x200), MAL0) // MAC_MDIO_Address
      CreateDWordField(PCSR,MDDL._BAS, MDL0)
      Store( Add(\_SB.PC00.GTSN.TADL,0x204), MDL0) // MAC_MDIO_Data
      Return(PCSR)
    }
  }
} // End of Scope(\_SB)
OperationRegion (OCWD, SystemIO, (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL), 0x4)
Field(OCWD, DWordAcc, NoLock, Preserve)
{
    ,      8,
    ,      8,
    FWSC,  8,    // Over-Clocking WDT Scratchpad (OC_WDT_SCRATCH)
}
//
// Platform specific FWU trigger method
//
Method(FWUC, 2)
{
  If(LEqual(Arg0, Zero)) {
    // Read
    And(FWSC, 0x00FF, Local0)
  } Else {
    // Write
    And(ToInteger(Arg1), 0xFF, Local0)
    And(FWSC, 0xFF00, Local1)
    Or(Local0, Local1, FWSC)
  }
  Return (Local0)
}