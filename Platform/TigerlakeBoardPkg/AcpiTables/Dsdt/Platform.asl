/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PlatformBase.h"
#include "Register/PmcRegs.h"

#define TRAP_TYPE_DTS     0x02
#define TRAP_TYPE_BGD     0x04  // BIOS Guard

// Define the following External variables to prevent a WARNING when
// using ASL.EXE and an ERROR when using IASL.EXE.

External(\PF00, IntObj) // PR00 _PDC Flags
External(\PF01)
External(\PF02)
External(\PF03)
External(\PF04)
External(\PF05)
External(\PF06)
External(\PF07)
External(\PF08)
External(\PF09)
External(\PF10)
External(\PF11)
External(\PF12)
External(\PF13)
External(\PF14)
External(\PF15)
External(\_SB.CFGD)
External(\HGMD)

//
// DTS externals
//
External(\_SB.DTSF)
External(\_SB.DTSE)
External(\_SB.PDTS)
External(\_SB.PKGA)
External(\_SB.TRPD)
External(\_SB.TRPF)
External(\_SB.DSAE)

//
// Intel(R) Dynamic Tuning Technology Externals
//
External(\_SB.PC00.LPCB.H_EC.CHRG,DeviceObj)
External(\_SB.IETM, DeviceObj)
//
//  SGX
//
External(\_SB.EPCS)
External(\_SB.EMNA)
External(\_SB.ELNG)

External(\_SB.PC00.GFX0.TCHE)   // Technology enabled indicator
External(\_SB.PC00.GFX0.STAT)   // State Indicator
External(\_SB.PC00.GFX0.IUEH, MethodObj)

External(\_SB.TPM.PTS, MethodObj)
External(\_SB.PC00.PAUD.PUAM, MethodObj) //PUAM - PowerResource User Absent Mode
External(\_SB.PC00.XHCI.DUAM, MethodObj)  //DUAM - Device User Absent Mode for XHCI controller
External(\_SB.NVDR.RSTP, MethodObj)

External(TRTD)
External(ITRT)
External(\_SB.PC00.TCON, MethodObj)
External(\_SB.PC00.TG0N, MethodObj)
External(\_SB.PC00.TG1N, MethodObj)

External(\_SB.PC00.LPCB.H_EC.CFAN, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.LSTE, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.VPWR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.RPWR, FieldUnitObj)
External(\_SB.PC00.LPCB.H_EC.ECWT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ECRD, MethodObj)
External(\_SB.PC00.LPCB.H_EC.UPBT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ONTM, IntObj)
External(\_SB.PC00.LPCB.H_EC.BNUM, IntObj)
External(\_SB.PC00.LPCB.H_EC.LID0, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.BAT0, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.BAT1, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.BAT2, DeviceObj)

#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7


External (PDIW)
#define DynamicSwitchEnable   1
External(\_SB.PC00.DPFC, MethodObj)

Name(ECUP, 1)  // EC State indicator: 1- Normal Mode 0- Low Power Mode
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State


External(\_SB.PC00.TRP0, DeviceObj)
External(\_SB.PC00.TRP1, DeviceObj)
External(\_SB.PC00.TRP2, DeviceObj)
External(\_SB.PC00.TRP3, DeviceObj)
External(\_SB.PC01, DeviceObj)
External(\_SB.PC01.TRP0, DeviceObj)
External(\_SB.PC01.TRP1, DeviceObj)
External(\_SB.PC01.TRP2, DeviceObj)
External(\_SB.PC01.TRP3, DeviceObj)
External(\_SB.PC00.TRP0.HPSX, IntObj)
External(\_SB.PC00.TRP1.HPSX, IntObj)
External(\_SB.PC00.TRP2.HPSX, IntObj)
External(\_SB.PC00.TRP3.HPSX, IntObj)
External(\_SB.PC00.TRP0.HPEX, IntObj)
External(\_SB.PC00.TRP1.HPEX, IntObj)
External(\_SB.PC00.TRP2.HPEX, IntObj)
External(\_SB.PC00.TRP3.HPEX, IntObj)
External(\_SB.PC00.TRP0.PMEX, IntObj)
External(\_SB.PC00.TRP1.PMEX, IntObj)
External(\_SB.PC00.TRP2.PMEX, IntObj)
External(\_SB.PC00.TRP3.PMEX, IntObj)
External(\_SB.PC00.TRP0.PMSX, IntObj)
External(\_SB.PC00.TRP1.PMSX, IntObj)
External(\_SB.PC00.TRP2.PMSX, IntObj)
External(\_SB.PC00.TRP3.PMSX, IntObj)
External(\_SB.PC01.TRP0.HPSX, IntObj)
External(\_SB.PC01.TRP1.HPSX, IntObj)
External(\_SB.PC01.TRP2.HPSX, IntObj)
External(\_SB.PC01.TRP3.HPSX, IntObj)
External(\_SB.PC01.TRP0.HPEX, IntObj)
External(\_SB.PC01.TRP1.HPEX, IntObj)
External(\_SB.PC01.TRP2.HPEX, IntObj)
External(\_SB.PC01.TRP3.HPEX, IntObj)
External(\_SB.PC01.TRP0.PMEX, IntObj)
External(\_SB.PC01.TRP1.PMEX, IntObj)
External(\_SB.PC01.TRP2.PMEX, IntObj)
External(\_SB.PC01.TRP3.PMEX, IntObj)
External(\_SB.PC01.TRP0.PMSX, IntObj)
External(\_SB.PC01.TRP1.PMSX, IntObj)
External(\_SB.PC01.TRP2.PMSX, IntObj)
External(\_SB.PC01.TRP3.PMSX, IntObj)

External(TBTD, MethodObj)
External(TBTF, MethodObj)
External(MMRP, MethodObj)
External(MMTB, MethodObj)
External(TBFF, MethodObj)
External(FFTB, MethodObj)

External(\_SB.TCWK, MethodObj)

// Comms Hub module support
//include("CommsHub.asl")

// Create a Global MUTEX.

Mutex(MUTX,0)

// OS Up mutex
Mutex(OSUM, 0)
// WWAN Mutex
Mutex(WWMT, 0)
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
    //
    // Disable update DTS temperature and threshold value in every SMI
    //
    If(CondRefOf(\_SB.DTSE)){
      If(LAnd(\_SB.DTSE, LGreater(TCNT, 1)))
      {
        TRAP(TRAP_TYPE_DTS,30)
      }
    }
    Store(CPWE, CWEF)  // save GPE0_EN_127_96_CPU_WAKE_EN value to NVS
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
  If (CondRefOf(\_SB.PC00.TXHC)) {
    //
    // Bring system out of TC cold before enter Sx
    //
    If (TRTD) {
      //
      // Bring System out of TC cold
      //
      \_SB.PC00.TCON ()
    }
    If (ITRT) {
      //
      // Bring TBT group0/1 out of D3 cold if it is in D3 cold
      //
      \_SB.PC00.TG0N ()
      \_SB.PC00.TG1N ()
    }
  }
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

  If(NEXP)
  {
      // Reinitialize the Native PCI Express after resume

    If(And(OSCC,0x01))
    {
      NHPG()
    }
    If(And(OSCC,0x04))  // PME control granted?
    {
      NPME()
    }
  }

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
    If(CondRefOf(\_SB.DTSE)){
      If(LAnd(\_SB.DTSE, LGreater(TCNT, 1)))
      {
        TRAP(TRAP_TYPE_DTS, 20)
      }
    }

    If(LEqual(\ECON,1))
    {
      // Update Lid state after S3 or S4 resume
      If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
      {
        Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.LSTE)), LIDS)

        If(IGDS)
        {
            If (LEqual(LIDS, 0))
            {
              Store(0x80000000,\_SB.PC00.GFX0.CLID)
            }
            If (LEqual(LIDS, 1))
            {
              Store(0x80000003,\_SB.PC00.GFX0.CLID)
            }
        }
        Notify(\_SB.PC00.LPCB.H_EC.LID0,0x80)
      }

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
          PNOT()
        }
      }
      Else
      {
        If(LNotEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.RPWR)),PWRS))
        {
          Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.RPWR)),PWRS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
    }

    // For PCI Express Express Cards, it is possible a device was
    // either inserted or removed during an Sx State.  The problem
    // is that no wake event will occur for a given warm inseration
    // or removal, so the OS will not become aware of any change.
    // To get around this, re-enumerate all Express Card slots.
    //
    // If the Root Port is enabled, it may be assumed to be hot-pluggable.

    If(LNotEqual(\_SB.PC00.RP01.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP01,0)
    }

    If(LNotEqual(\_SB.PC00.RP02.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP02,0)
    }

    If(LNotEqual(\_SB.PC00.RP03.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP03,0)
    }

    If(LNotEqual(\_SB.PC00.RP04.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP04,0)
    }

    If(LNotEqual(\_SB.PC00.RP05.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP05,0)
    }

    If(LNotEqual(\_SB.PC00.RP06.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP06,0)
    }

    If(LNotEqual(\_SB.PC00.RP07.VDID,0xFFFFFFFF))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PC00.RP07,0)
      }
    }

    If(LNotEqual(\_SB.PC00.RP08.VDID,0xFFFFFFFF))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PC00.RP08,0)
      }
    }

    If(LNotEqual(\_SB.PC00.RP09.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP09,0)
    }

    If(LNotEqual(\_SB.PC00.RP10.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP10,0)
    }

    If(LNotEqual(\_SB.PC00.RP11.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP11,0)
    }

    If(LNotEqual(\_SB.PC00.RP12.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP12,0)
    }

    If(LNotEqual(\_SB.PC00.RP13.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP13,0)
    }

    If(LNotEqual(\_SB.PC00.RP14.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP14,0)
    }

    If(LNotEqual(\_SB.PC00.RP15.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP15,0)
    }

    If(LNotEqual(\_SB.PC00.RP16.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP16,0)
    }

    If(LNotEqual(\_SB.PC00.RP17.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP17,0)
    }

    If(LNotEqual(\_SB.PC00.RP18.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP18,0)
    }

    If(LNotEqual(\_SB.PC00.RP19.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP19,0)
    }

    If(LNotEqual(\_SB.PC00.RP20.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP20,0)
    }
    If(LNotEqual(\_SB.PC00.RP21.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP21,0)
    }
    If(LNotEqual(\_SB.PC00.RP22.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP22,0)
    }
        If(LNotEqual(\_SB.PC00.RP23.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP23,0)
    }
        If(LNotEqual(\_SB.PC00.RP24.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP24,0)
    }

    If(LNotEqual(\_SB.PC00.RP21.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP21,0)
    }
    If(LNotEqual(\_SB.PC00.RP22.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP22,0)
    }
    If(LNotEqual(\_SB.PC00.RP23.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP23,0)
    }
    If(LNotEqual(\_SB.PC00.RP24.VDID,0xFFFFFFFF))
    {
      Notify (\_SB.PC00.RP24,0)
    }
    If (CondRefOf(\_SB.PC00.TXHC)) {
      \_SB.TCWK(Arg0)
    }
  }

  If (LEqual(ECON,1)) {
    If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
    {
      //
      // If Using Control Method Power Button, notify PWRD device with 0x2
      //
      If(LEqual(\_SB.PWRB.PBST, 0x1)) {
        If(PBSS) { //Power Button woke the system
          Notify(\_SB.PWRB, 0x02) // Send release notification to Power Button device 0x02
          Store(1, PBSS)
        }
      }
    }
  }

  If(LEqual(Arg0,3))
  {
    //
    // Call NVDR.RSTP
    //
    If(CondRefOf(\_SB.NVDR.RSTP))
    {
      \_SB.NVDR.RSTP ()
    }

    //
    // Proactive Check DPIn Dynamic Switch Supportivity when S3 resume
    // Send V_IOM_BIOS_DPIN_SWITCH_CMD to IOM if the feature is supported
    // Report Function 20 if Dynamic Switch is Enabled by user (PDIW)
    //
    If (LAnd (LAnd (PDIW, DynamicSwitchEnable), CondRefOf (PDIW))) {
      Name (OPTS, Buffer (4){0,0,0,0})
      CreateByteField (OPTS, 0x00, CMST) // Command Status field
                                         //         Success - 0
                                         //         Fail    - 1
      CreateByteField (OPTS, 0x01, RTB1) // Return Buffer 1

      //Send V_IOM_BIOS_DPIN_SWITCH_CMD
      Store (\_SB.PC00.DPFC (), OPTS)

    }
    Else
    {
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

// Power Notification:
//    Perform all needed OS notifications during a
//    Power Switch.
//
//  Arguments:
//    None
//
//  Return Value:
//    None

Method(PNOT,0,Serialized)
{
  //
  // If MP enabled and driver support is present, notify all
  // processors.
  //
  If(LGreater(TCNT, 1))
  {
    If(And(\PF00,0x0008)){
      Notify(\_SB.PR00,0x80)    // Eval PR00 _PPC.
    }
    If(And(\PF01,0x0008)){
      Notify(\_SB.PR01,0x80)    // Eval PR01 _PPC.
    }
    If(And(\PF02,0x0008)){
      Notify(\_SB.PR02,0x80)    // Eval PR02 _PPC.
    }
    If(And(\PF03,0x0008)){
      Notify(\_SB.PR03,0x80)    // Eval PR03 _PPC.
    }
    If(And(\PF04,0x0008)){
      Notify(\_SB.PR04,0x80)    // Eval PR04 _PPC.
    }
    If(And(\PF05,0x0008)){
      Notify(\_SB.PR05,0x80)    // Eval PR05 _PPC.
    }
    If(And(\PF06,0x0008)){
      Notify(\_SB.PR06,0x80)    // Eval PR06 _PPC.
    }
    If(And(\PF07,0x0008)){
      Notify(\_SB.PR07,0x80)    // Eval PR07 _PPC.
    }
    If(And(\PF08,0x0008)){
      Notify(\_SB.PR08,0x80)    // Eval PR08 _PPC.
    }
    If(And(\PF09,0x0008)){
      Notify(\_SB.PR09,0x80)    // Eval PR09 _PPC.
    }
    If(And(\PF10,0x0008)){
      Notify(\_SB.PR10,0x80)    // Eval PR10 _PPC.
    }
    If(And(\PF11,0x0008)){
      Notify(\_SB.PR11,0x80)    // Eval PR11 _PPC.
    }
    If(And(\PF12,0x0008)){
      Notify(\_SB.PR12,0x80)    // Eval PR12 _PPC.
    }
    If(And(\PF13,0x0008)){
      Notify(\_SB.PR13,0x80)    // Eval PR13 _PPC.
    }
    If(And(\PF14,0x0008)){
      Notify(\_SB.PR14,0x80)    // Eval PR14 _PPC.
    }
    If(And(\PF15,0x0008)){
      Notify(\_SB.PR15,0x80)    // Eval PR15 _PPC.
    }
  }Else{
    Notify(\_SB.PR00,0x80)      // Eval _PPC.
  }

  If(LGreater(TCNT, 1)){
    If(LAnd(And(\PF00,0x0008),And(\PF00,0x0010))){
      Notify(\_SB.PR00,0x81)  // Eval PR00 _CST.
    }
    If(LAnd(And(\PF01,0x0008),And(\PF01,0x0010))){
      Notify(\_SB.PR01,0x81)  // Eval PR01 _CST.
    }
    If(LAnd(And(\PF02,0x0008),And(\PF02,0x0010))){
      Notify(\_SB.PR02,0x81)  // Eval PR02 _CST.
    }
    If(LAnd(And(\PF03,0x0008),And(\PF03,0x0010))){
      Notify(\_SB.PR03,0x81)  // Eval PR03 _CST.
    }
    If(LAnd(And(\PF04,0x0008),And(\PF04,0x0010))){
      Notify(\_SB.PR04,0x81)  // Eval PR04 _CST.
    }
    If(LAnd(And(\PF05,0x0008),And(\PF05,0x0010))){
      Notify(\_SB.PR05,0x81)  // Eval PR05 _CST.
    }
    If(LAnd(And(\PF06,0x0008),And(\PF06,0x0010))){
      Notify(\_SB.PR06,0x81)  // Eval PR06 _CST.
    }
    If(LAnd(And(\PF07,0x0008),And(\PF07,0x0010))){
      Notify(\_SB.PR07,0x81)  // Eval PR07 _CST.
    }
    If(LAnd(And(\PF08,0x0008),And(\PF08,0x0010))){
      Notify(\_SB.PR08,0x81)  // Eval PR08 _CST.
    }
    If(LAnd(And(\PF09,0x0008),And(\PF09,0x0010))){
      Notify(\_SB.PR09,0x81)  // Eval PR09 _CST.
    }
    If(LAnd(And(\PF10,0x0008),And(\PF10,0x0010))){
      Notify(\_SB.PR10,0x81)  // Eval PR10 _CST.
    }
    If(LAnd(And(\PF11,0x0008),And(\PF11,0x0010))){
      Notify(\_SB.PR11,0x81)  // Eval PR11 _CST.
    }
    If(LAnd(And(\PF12,0x0008),And(\PF12,0x0010))){
      Notify(\_SB.PR12,0x81)  // Eval PR12 _CST.
    }
    If(LAnd(And(\PF13,0x0008),And(\PF13,0x0010))){
      Notify(\_SB.PR13,0x81)  // Eval PR13 _CST.
    }
    If(LAnd(And(\PF14,0x0008),And(\PF14,0x0010))){
      Notify(\_SB.PR14,0x81)  // Eval PR14 _CST.
    }
    If(LAnd(And(\PF15,0x0008),And(\PF15,0x0010))){
      Notify(\_SB.PR15,0x81)  // Eval PR15 _CST.
    }
  } Else {
    Notify(\_SB.PR00,0x81)      // Eval _CST.
  }

  // Update the Battery 1 and 2 Stored Capacity and
  // Stored Status.  Battery 0 information is always accurrate.

  If(LEqual(\ECON,1))
  {

    // Perform update to all Batteries in the System.
    Notify(\_SB.PC00.LPCB.H_EC.BAT0,0x81)       // Eval BAT0 _BST.
    Notify(\_SB.PC00.LPCB.H_EC.BAT1,0x81)       // Eval BAT1 _BST.
    Notify(\_SB.PC00.LPCB.H_EC.BAT2,0x81)       // Eval BAT2 _BST.
  }

  If (LEqual(DPTF,1)){
    Notify(\_SB.IETM, 0x86) // Notification sent to Intel(R) Dynamic Tuning Technology driver (Policy) for PDRT reevaluation after AC/DC transtion has occurred.
    If (LAnd(LEqual(\ECON,1), LEqual(CHGE,1))){
      Notify(\_SB.PC00.LPCB.H_EC.CHRG, 0x80) // PPPC/PPDL reevaluation after AC/DC transtion has occurred.
    }
  }

} // end of Method(PNOT)

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

        //P8XH(0, 0xE1)
        //P8XH(1, 0xAB)

        If(PSCP){
          // if P-state Capping s enabled
          If (CondRefOf(\_SB.PR00._PPC))
          {
            Store(Zero, \_SB.CPPC)
            PNOT()
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

        //P8XH(0, 0xE0)
        //P8XH(1, 00)

        If(PSCP){
          // if P-state Capping is enabled
          If (LAnd(CondRefOf(\_SB.PR00._PSS), CondRefOf(\_SB.PR00._PPC)))
          {
            If(And(\PF00,0x0400))
            {
              Subtract(SizeOf(\_SB.PR00.TPSS), One, \_SB.CPPC)
            } Else {
              Subtract(SizeOf(\_SB.PR00.LPSS), One, \_SB.CPPC)
            }
            PNOT()
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
    // Adding back USB powergating (ONLY for Win8) until RTD3 walkup port setup implementation is complete */
    If(LEqual(OSYS,2012)){    // ONLY for Win8 OS
      If(CondRefOf(\_SB.PC00.XHCI.DUAM)){ // Notify USB port- RVP
        \_SB.PC00.XHCI.DUAM()
      }
    }
    // TODO: Add calls to UAMN methods for
    //    * USB controller(s)
    //    * Embedded Controller
    //    * Sensor devices
    //    * Audio DSP?
    //    * Any other devices dependent on User Absent mode for power controls
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

  If(LEqual(Arg0,TRAP_TYPE_DTS))  // Is DTS IO Trap?
  {
    Store(Arg1,\_SB.DTSF)      // Store the function number global NVS
    // Store(0,\_SB.TRPD)         // Generate IO Trap.
    Return(\_SB.DTSF)          // Return status from SMI handler
  }

  If(LEqual(Arg0,TRAP_TYPE_BGD))  // Is BIOS Guard TOOLS IO Trap?
  {
    // Store(0,\_SB.TRPF)         // Generate IO Trap
  }

  Return(SMIF)            // Return SMIF.  0 = Success.
}

// Note:  Only add the indicator device needed by the platform.

//
// System Bus
//
Scope(\_SB.PC00)
{

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
    //   OSYS = 2015 = Windows 10
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

    //
    // Set DTS NVS data means in OS ACPI mode enabled insteads of GlobalNvs OperatingSystem (OSYS)
    //
    If(CondRefOf(\_SB.DTSE)){
      If(LGreaterEqual(\_SB.DTSE, 0x01)){
        Store(0x01, \_SB.DSAE)
      }
    }
  }
}

Scope (\)
{
  Method(NHPG,0,Serialized)
  {
    Store(0,\_SB.PC00.RP01.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP02.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP03.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP04.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP05.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP06.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP07.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP08.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP09.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP10.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP11.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP12.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP13.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP14.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP15.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP16.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP17.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP18.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP19.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP20.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP21.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP22.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP23.HPEX) // clear the hot plug SCI enable bit
    Store(0,\_SB.PC00.RP24.HPEX) // clear the hot plug SCI enable bit
    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(0,\_SB.PC01.TRP0.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC01.TRP1.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC01.TRP2.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC01.TRP3.HPEX) // clear the hot plug SCI enable bit
      } Else {
        Store(0,\_SB.PC00.TRP0.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC00.TRP1.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC00.TRP2.HPEX) // clear the hot plug SCI enable bit
        Store(0,\_SB.PC00.TRP3.HPEX) // clear the hot plug SCI enable bit
      }
    }

    Store(1,\_SB.PC00.RP01.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP02.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP03.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP04.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP05.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP06.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP07.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP08.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP09.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP10.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP11.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP12.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP13.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP14.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP15.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP16.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP17.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP18.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP19.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP20.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP21.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP22.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP23.HPSX) // clear the hot plug SCI status bit
    Store(1,\_SB.PC00.RP24.HPSX) // clear the hot plug SCI status bit
    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(1,\_SB.PC01.TRP0.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP1.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP2.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC01.TRP3.HPSX) // clear the hot plug SCI status bit
      } Else {
        Store(1,\_SB.PC00.TRP0.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP1.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP2.HPSX) // clear the hot plug SCI status bit
        Store(1,\_SB.PC00.TRP3.HPSX) // clear the hot plug SCI status bit
      }
    }
  }

  Method(NPME,0,Serialized)
  {
    Store(0,\_SB.PC00.RP01.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP02.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP03.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP04.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP05.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP06.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP07.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP08.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP09.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP10.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP11.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP12.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP13.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP14.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP15.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP16.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP17.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP18.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP19.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP20.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP21.PMEX) // clear the PME SCI status bit
    Store(0,\_SB.PC00.RP22.PMEX) // clear the PME SCI status bit
    Store(0,\_SB.PC00.RP23.PMEX) // clear the PME SCI status bit
    Store(0,\_SB.PC00.RP24.PMEX) // clear the PME SCI status bit
    Store(0,\_SB.PC00.RP21.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP22.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP23.PMEX) // clear the PME SCI enable bit
    Store(0,\_SB.PC00.RP24.PMEX) // clear the PME SCI enable bit
    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(0,\_SB.PC01.TRP0.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC01.TRP1.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC01.TRP2.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC01.TRP3.PMEX) // clear the PME SCI enable bit
      } Else {
        Store(0,\_SB.PC00.TRP0.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC00.TRP1.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC00.TRP2.PMEX) // clear the PME SCI enable bit
        Store(0,\_SB.PC00.TRP3.PMEX) // clear the PME SCI enable bit
      }
    }

    Store(1,\_SB.PC00.RP01.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP02.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP03.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP04.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP05.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP06.PMSX) // clear the PME SCI enable bit
    Store(1,\_SB.PC00.RP07.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP08.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP09.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP10.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP11.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP12.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP13.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP14.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP15.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP16.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP17.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP18.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP19.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP20.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP21.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP22.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP23.PMSX) // clear the PME SCI status bit
    Store(1,\_SB.PC00.RP24.PMSX) // clear the PME SCI status bit
    If (CondRefOf(\_SB.PC00.TXHC)) {
      If (CondRefOf(\_SB.PC01)) {
        Store(1,\_SB.PC01.TRP0.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP1.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP2.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC01.TRP3.PMSX) // clear the PME SCI status bit
      } Else {
        Store(1,\_SB.PC00.TRP0.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP1.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP2.PMSX) // clear the PME SCI status bit
        Store(1,\_SB.PC00.TRP3.PMSX) // clear the PME SCI status bit
      }
    }
  }
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

//
// EPC Memory Device support
//
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
        }
      } Else{
        And(STS0,0xFFFFFF00,STS0)
        Or(STS0,0xA, STS0) // Unrecognised Revision and report OSC failure
      }
    } Else {
      And(STS0,0xFFFFFF00,STS0)
      Or (STS0,0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return(Arg3)
  } // End _OSC

   Device (EPC)
   {
     Name (_HID, EISAID ("INT0E0C"))
     Name (_STR, Unicode ("Enclave Page Cache 1.0"))
     Name (_MLS, Package () {
       Package (2) { "en", Unicode ("Enclave Page Cache 1.0") }
     })

     Name (RBUF, ResourceTemplate ()
     {
       QWordMemory (
         ResourceConsumer,    // ResourceUsage
         PosDecode,           // Decode               _DEC
         MinNotFixed,         // IsMinFixed           _MIF
         MaxNotFixed,         // IsMaxFixed           _MAF
         NonCacheable,        // Cacheable            _MEM
         ReadWrite,           // ReadAndWrite         _RW
         0,                   // AddressGranularity   _GRA
         0,                   // AddressMinimum       _MIN  <= To be patched in Runtime
         0,                   // AddressMaximum       _MAX  <= To be patched in Runtime
         0,                   // AddressTranslation   _TRA
         1,                   // RangeLength          _LEN  <= To be patched in Runtime
         ,                    // ResourceSourceIndex
         ,                    // ResourceSource
         BAR0                 // DescriptorName
       )
     })

     Method (_CRS, 0x0, NotSerialized)
     {
       CreateQwordField (RBUF, ^BAR0._MIN, EMIN)
       CreateQwordField (RBUF, ^BAR0._MAX, EMAX)
       CreateQwordField (RBUF, ^BAR0._LEN, ELEN)
       Store (\_SB.EMNA, EMIN)
       Store (\_SB.ELNG, ELEN)
       Subtract (Add (\_SB.EMNA, \_SB.ELNG), 1, EMAX)
       Return (RBUF)
     }

     Method (_STA, 0x0, NotSerialized)
     {
       If (LNotEqual (\_SB.EPCS, 0))
       {
         Return (0xF)
       }
       Return (0x0)
     }
   } // end EPC Device
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

// Power Meter module support
include("PowerMeterTglU.asl")





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
