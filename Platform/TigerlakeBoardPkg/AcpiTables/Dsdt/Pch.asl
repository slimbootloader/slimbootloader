/**@file

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/PchInfoLib.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PmcRegs.h>

#define PCR_MMIO_SIZE_PER_PID       0x10000        ///< 64KB
#define PCR_MMIO_SIZE               0x01000000     ///< 16MB
#define PCR_MMIO_SIZE_EX            0x10000000     ///< 256MB, used by SBREG_BAR 20-bit address platforms

Name(PCHH,PCH_H) // PCH H
Name(PCHN,PCH_N) // PCH N
Name(PCHL,PCH_LP) // PCH LP

Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length
Include ("PchNvs.asl")
#ifdef PCH_TGL
Include ("PchImgClkOutVer2.asl")
#else
Include ("PchClockControl.asl")
#endif

//
// Include GPIO macros and access library
//
Include("HsioDefine.asl")

Include("GpioLib.asl")
Include("HsioLib.asl")

//
// Trace Hub debug library
// Include it earlier so the debug function can be used as soon as possible
//
Include ("TraceHubDebug.asl")

External(\_SB.PC00.RP01.PPRW, MethodObj)
External(\_SB.PC00.RP02.PPRW, MethodObj)
External(\_SB.PC00.RP03.PPRW, MethodObj)
External(\_SB.PC00.RP04.PPRW, MethodObj)
External(\_SB.PC00.RP05.PPRW, MethodObj)
External(\_SB.PC00.RP06.PPRW, MethodObj)
External(\_SB.PC00.RP07.PPRW, MethodObj)
External(\_SB.PC00.RP09.PPRW, MethodObj)
External(\_SB.PC00.RP10.PPRW, MethodObj)
External(\_SB.PC00.RP11.PPRW, MethodObj)
External(\_SB.PC00.RP12.PPRW, MethodObj)
External(\_SB.PC00.RP13.PPRW, MethodObj)
External(\_SB.PC00.RP14.PPRW, MethodObj)
External(\_SB.PC00.RP15.PPRW, MethodObj)
External(\_SB.PC00.RP16.PPRW, MethodObj)
External(\_SB.PC00.RP17.PPRW, MethodObj)
External(\_SB.PC00.RP18.PPRW, MethodObj)
External(\_SB.PC00.RP19.PPRW, MethodObj)
External(\_SB.PC00.RP20.PPRW, MethodObj)
External(\_SB.PC00.RP21.PPRW, MethodObj)
External(\_SB.PC00.RP22.PPRW, MethodObj)
External(\_SB.PC00.RP23.PPRW, MethodObj)
External(\_SB.PC00.RP24.PPRW, MethodObj)

External(VMDE)
External(VMR1)
External(VMR2)
External(VMR3)

//
// PME event (PCH's GPE 6Dh) is received when any PCH internal device with
// PCI Power Management capabilities on bus 0 asserts the equivalent of the PME# signal.
// Dedicated _L6D method is not needed if its only purpose is to notify the device.
// Instead to properly handle this GPE event below requirements need to be fulfilled
// - _PRW for GPE 6Dh in devices capable of generating PME (XHCI, XDCI, GbE, HDA)
// - _S0W(3) and _DSW is present within those devices

Scope (\_SB) {
  //
  // PCH RC _WAK handler
  // This method should be called from _WAK function
  // to perform necessary PCH actions upon wake
  //
  Method(PWAK, 1) {
    //
    // Arg0 - sleeping state (1 for S1, 2 for S2, etc.)
    //
    //
    // On S3 resume call S0i2/3 wake handler
    //
    If (LEqual (Arg0, 3)) {
      S23W()
    }
  }

  //
  // PCH S0i2/3 dynamic switching
  //
  // 0: Device inactive
  // 1: Device active
  // 2: Device statically disabled
  //
  Name (HDAA, 1)    // HD Audio Active state flag
  Name (DISA, 1)    // Display Active state flag
  Name (CIWF, 1)    // CNVi WiFi Active state flag
  Name (CIBT, 1)    // CNVi BT Active state flag
  Name (S23C, 0)    // Flag indicating if function was called and static disabling was taken into account

 //
 // This method should be called upon resume from S3 state
 // from _WAK method to reinitialize device state variables
 // that impact S0i2/S0i3 flow
 //
  Method(S23W, 0) {
    //
    // Upon wake from S3 reinitialize state variables to default
    // which is also safe config and one that is left after BIOS boot
    //
    Store (1, HDAA)
    Store (1, DISA)
    Store (1, CIWF)
    Store (1, CIBT)
  }

  //
  // Display ON event handler for S0i2/3 dynamic switching
  // To be called in 'GUAM - Global User Absent Mode' or similar screen/display on event handler
  //
  Method(DION, 0) {
    S023(1, 1) // CS Exit event - Display state ON
  }

  //
  // Display OFF event handler for S0i2/3 dynamic switching
  // To be called in 'GUAM - Global User Absent Mode' or similar screen/display off event handler
  //
  Method(DIOF, 0) {
    S023(1, 0) // CS Entry event - Display state OFF
  }

  //
  // S0i2/3 dynamic switching
  // Arg0 - Caller: 0 - HD Audio, 1 - Display, 2 - CNVi-WiFi, 3 - CNVi-BT
  // Arg1 - Device state: 0 - inactive, 1 - active
  //
  Method(S023, 2, Serialized) {
#ifdef PCH_ICL

    //
    // Check if any device is statically disabled in boot time
    // Do this only once
    //
    If (LNot(S23C)) {
      If (LEqual(\_SB.PC00.HDAS.VDID, 0xFFFFFFFF)) {
        Store (2, HDAA)
      }
      If (LNot(\_SB.PC00.CNIP ())) {
        Store (2, CIWF)
        //
        // CNVi-BT needs to be disabled if CNVi WiFi is disabled
        //
       Store (2, CIBT)
      }
      Store (1, S23C)
    }

    Switch(ToInteger(Arg0)) {
      Case(0) {
        If (LNotEqual(HDAA, 2)) { // Check if HDAudio is not statically disabled
          Store (Arg1, HDAA)
        }
      }
      Case(1) {
        Store (Arg1, DISA)
      }
      Case(2) {
        If (LNotEqual(CIWF, 2)) { // Check if CNVi WiFi is not statically disabled
          Store (Arg1, CIWF)
        }
      }
      Case(3) {
        If (LNotEqual(CIBT, 2)) { // Check if CNVi BT is not statically disabled
          Store (Arg1, CIBT)
        }
      }
      Default {
        Return // Invalid parameter
      }
    }


    If (LAnd(LAnd(LNotEqual(DISA, 1), LNotEqual(HDAA,1)),
             LAnd(LNotEqual(CIWF,1), LNotEqual(CIBT,1)))) {
      // Diplay OFF, Audio in D3, CNVi WiFi & BT in D3
      // CPPMVRIC (0x1B1Ch[22]) = 0, 24MHz Crystal Shutdown Qualification Disable (XTALSDQDIS)
      Store(0, XSQD)
    } Else {
      // CPPMVRIC (0x1B1Ch[22]) = 1, 24MHz Crystal Shutdown Qualification Disable (XTALSDQDIS)
      Store(1, XSQD)
    }
#endif
  }

  //
  // PCH reserved resources
  //
  Device(PRRE) {
    Name(_HID,EISAID("PNP0C02")) // motherboard resource
    Name(_UID,"PCHRESV")
    Name(_STA,0x3) // device present and decodes its resources, but not to be displayed in OSPM

    Method(_CRS,0,Serialized)
    {
      Name(BUF0,ResourceTemplate(){
        //
        // PCH RESERVED MMIO RANGE
        // Refer to PchReservedResources.h for ranges used by PCH
        //
        // skip over address range that might be claimed by the Intel Serial IO, and TraceHub
        // The SerialIO ranges will be handled by SIRC device.
        // skip 0xFC800000 - 0xFCFFFFFF for PCH Trace Hub : SW BAR space
        // skip 0xFD800000 - 0xFDFFFFFF for PCH Trace Hub : SW BAR space (ADL)
        Memory32Fixed (ReadWrite, 0xFE000000, 0x00020000) // 0xFE000000 - 0xFE01FFFF
        // Skip 0xFE020000 - 0xFE04BFFF for Serial IO
        Memory32Fixed (ReadWrite, 0xFE04C000, 0x00004000) // 0xFE04C000 - 0xFE04FFFF
        Memory32Fixed (ReadWrite, 0xFE050000, 0x00060000) // 0xFE050000 - 0xFE0AFFFF
        // Skip 0xFE0B0000 - 0xFE0CFFFF for eSPI
        Memory32Fixed (ReadWrite, 0xFE0D0000, 0x00030000) // 0xFE0D0000 - 0xFE0FFFFF
        // skip 0xFE100000 - 0xFE1FFFFF for PCH Trace Hub : MTB BAR space
        Memory32Fixed (ReadWrite, 0xFE200000, 0x00600000) // 0xFE200000 - 0xFE7FFFFF
        // BIOS decoding range. Assume it's always 16MB.
        Memory32Fixed (ReadOnly, 0xFF000000, 0x1000000)

        // ACPI Base
        IO (Decode16,0,0,0x1,0xFF,ABAR)
      })
      // Update ACPI IO region
      CreateWordField (BUF0, ABAR._MIN,AMIN)
      CreateWordField (BUF0, ABAR._MAX,AMAX)
      Store (PMBS, AMIN)
      Store (PMBS, AMAX)

      //
      // Claim SBREG_MMIO range except for some GPIO communities which are used by GPIO OS driver, refer to Gpio.asl
      //
      // PID_GPIOCOM0 = 0x6E
      // PID_GPIOCOM1 = 0x6D
      // PID_GPIOCOM2 = 0x6C
      // PID_GPIOCOM3 = 0x6B
      // PID_GPIOCOM4 = 0x6A
      // PID_GPIOCOM5 = 0x69
      //
      // claim region from SBREG_BASE to first GPIO community claimed by GPIO driver.
      //
      Name(SBR0, ResourceTemplate(){ Memory32Fixed (ReadWrite, 0x0, PCH_GPIO_COM5, SBRR) })
      CreateDWordField (SBR0, SBRR._BAS, BAS0)
      Store (SBRG, BAS0)
      ConcatenateResTemplate(BUF0, SBR0, Local0)
      //
      // claim GPIO communities that GPIO driver does not use.
      //


      If (LOr (LEqual (PCHS, PCHH), LEqual (PCHS, PCH_S))) {
        //
        // claim GPIO COM2
        //
        Name (SBRS, ResourceTemplate () {Memory32Fixed (ReadWrite, 0x0, 0x0, SBSA) })         // Community 2
        CreateDWordField (SBRS, SBSA._BAS, BAS1)
        CreateDWordField (SBRS, SBSA._LEN, LEN1)
        Store (Add (SBRG, PCH_GPIO_COM2), BAS1)
        Store (PCR_MMIO_SIZE_PER_PID, LEN1)
        ConcatenateResTemplate (Local0, SBRS, Local1)
      } Else {
        //
        // claim GPIO COM2 & COM3
        //
        Name (SBRL, ResourceTemplate () {Memory32Fixed (ReadWrite, 0x0, 0x0, SBSB) })       // Community 2, 3
        CreateDWordField (SBRL, SBSB._BAS, BAS2)
        CreateDWordField (SBRL, SBSB._LEN, LEN2)
        Store(Add (SBRG, PCH_GPIO_COM3), BAS2)
        Store(Multiply (PCR_MMIO_SIZE_PER_PID, 2), LEN2)
        ConcatenateResTemplate(Local0, SBRL, Local1)
      }
      //
      // claim the rest of SBREG range, from above GPIO COM0 to SBREG range upper limit
      //
      Name (SBRE, ResourceTemplate () {Memory32Fixed (ReadWrite, 0x0, 0x0, SEND) })
      CreateDWordField (SBRE, SEND._BAS, BAS3)
      CreateDWordField (SBRE, SEND._LEN, LEN3)
      Store (Add (SBRG, Add (PCH_GPIO_COM0, PCR_MMIO_SIZE_PER_PID)), BAS3)
      If (LEqual (PCHS, PCH_S)) {
        Store (Subtract (Add (SBRG, PCR_MMIO_SIZE_EX), BAS3), LEN3)
      } Else {
        Store (Subtract (Add (SBRG, PCR_MMIO_SIZE), BAS3), LEN3)
      }
      ConcatenateResTemplate (Local1, SBRE, Local2)
      return (Local2)
    } // Method (_CRS)
  } // Device (PRRE)

  Device(IOTR) {
    //
    // This device claims IO range reserved for IO traps
    // to prevent OS from reusing it for other purposes
    //
    Name(_HID,EISAID("PNP0C02"))
    Name(_UID,"IoTraps")
    Method(_CRS) {
      Name(BUF0,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG0) })
      Name(BUF1,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG1) })
      Name(BUF2,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG2) })
      Name(BUF3,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG3) })
      CreateWordField(BUF0,TAG0._MIN,AMI0);CreateWordField(BUF0,TAG0._MAX,AMA0)
      CreateWordField(BUF1,TAG1._MIN,AMI1);CreateWordField(BUF1,TAG1._MAX,AMA1)
      CreateWordField(BUF2,TAG2._MIN,AMI2);CreateWordField(BUF2,TAG2._MAX,AMA2)
      CreateWordField(BUF3,TAG3._MIN,AMI3);CreateWordField(BUF3,TAG3._MAX,AMA3)

      Store(0, Local2)
      //
      // Win7 can't store result of ConcatenateResTemplate directly into one of its input parameters
      // When ITS0, ITS1, ITS2 and ITS3 is 1. Update BUF0, BUF1, BUF2 and BUF3 then store to Local0
      //
      If(LEqual(ITS0,1)) {
        Store(ITA0,AMI0)
        Store(ITA0,AMA0)
        Store(BUF0, Local0)
        Store(1, Local2)
      }
      If(LEqual(ITS1,1)) {
        Store(ITA1,AMI1)
        Store(ITA1,AMA1)
        If (LEqual (Local2,1)){
          ConcatenateResTemplate(BUF1, Local0, Local1)
          Store(Local1, Local0)
        } Else {
          Store(BUF1, Local0)
          Store(1, Local2)
        }
      }
      If(LEqual(ITS2,1)) {
        Store(ITA2,AMI2)
        Store(ITA2,AMA2)
        If (LEqual(Local2, 1)){
          ConcatenateResTemplate(BUF2, Local0, Local1)
          Store(Local1, Local0)
        } Else {
          Store(BUF2, Local0)
          Store(1, Local2)
        }
      }
      If(LEqual(ITS3,1)) {
        Store(ITA3,AMI3)
        Store(ITA3,AMA3)
        If(LEqual(Local2, 1)){
          ConcatenateResTemplate(BUF3, Local0, Local1)
          Store(Local1, Local0)
        } Else {
          Store(BUF3, Local0)
          Store(1, Local2)
        }
      }
      If(LEqual(Local2, 1)){
        Return(Local0)
      } Else {
        Return(Buffer(){0x79, 0x00})
      }
    }
  }

}

Scope (\_SB.PC00) {
  Name(TEMP,0)                          // scratch variable that does not cause compiler warnings as writes to Localx variables do.

  //
  // LPC Bridge - Device 31, Function 0, this is only for PCH register Memory Region declare,
  // it's better to be declared as early as possible since it's widely used in whole ACPI name space.
  // Please add any code which needs to reference any register of it after this
  //
  Device(LPCB) {
    Name(_ADR, 0x001F0000)

    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}

    OperationRegion(LPC, PCI_Config, 0x00, 0x100)
    Field(LPC, AnyAcc, NoLock, Preserve)
    {
      Offset(0x02),
      CDID, 16,
      Offset(0x08),
      CRID,  8,
      Offset(R_LPC_CFG_IOD), // 0x80
      IOD0,  8,
      IOD1,  8,
      Offset(0xA0),
      , 9,
      PRBL,  1,
      Offset(R_LPC_CFG_BC), // 0xDC
      ,       2,
      ESPI,   1,
    }
  }

  //
  // SMBus Controller - Device 31, Function 4
  //
  Device(SBUS) {
    Name(_ADR,0x001F0004)
    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
  }

  //
  // SPI Flash Controller - Device 31, Function 5
  //
  Device(SPI) {
    Name(_ADR, 0x001F0005)
        Method(_STA, 0) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return(0)
    }
   Return(0x0F)
  }

   Method(SWPD)
  {
   if(WPDB)
   {
     Store (0, WPDB)
   }
   if(SYNC)
   {
     Store (1, SYNC)
        }
  }

  OperationRegion(SPIX,SystemMemory,\_SB.PC00.PC2M(_ADR),0xFF)
  Field(SPIX, AnyAcc, NoLock, Preserve)
  {
    Offset(0),
    VDID, 32,
        Offset(0xDC),
    WPDB, 1,
        ,     7,
        SYNC, 1,
  }
 }
}
Include("Pcr.asl")
Include("Pmc.asl")

Scope (\_SB.PC00) {
  Name(LTRZ, 0)
  Name(LMSL, 0)
  Name(LNSL, 0)
} //scope

If (LNotEqual(GBES,0)) {
  //
  // LAN Controller
  //
  Scope (\_SB.PC00) {
    Device(GLAN) { // GbE Controller
      Name(_ADR, 0x001F0006)
      Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}

      Name(_S0W, 3) // Device can wake itself from D3 in S0

      Method(_PRW, 0) {
        Return(GPRW(0x6D, 4)) // can wakeup from S4 state
      }

      Method(_DSW, 3) {}

    } // end "GbE Controller"
  } //scope
}

//
// xHCI Controller - Device 20, Function 0
//
Include("PchXhci.asl")

//
// xDCI (OTG) Controller - Device 20, Function 1
//
include("PchXdci.asl")

Scope(\_SB_.PC00) {
  //
  // High Definition Audio Controller - Device 31, Function 3
  //
  include("PchHda.asl")

  //
  // High Definition Audio - SoundWire Controller
  //
  include("PchHdaSoundWireCtrl.asl")

  //
  // High Definition Audio - USB Audio Offload
  //
  include("PchHdaUsbAudioOffload.asl")

  //
  // High Definition Audio - Intel Display Audio
  //
  include("PchHdaIntelDisplayAudio.asl")

  //
  // PCIE Root Port #01
  //
  Device(RP01) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA1,0)) {
        Return (RPA1)
      } Else {
        Return (0x001C0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,1)
    Method(_INI)
    {
      Store (LTR1, LTRZ)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      If (HBSL & 0x1) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x01),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP01.PPRW)) {
        Return (\_SB.PC00.RP01.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #01"

  //
  // PCIE Root Port #02
  //
  Device(RP02) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA2,0)) {
        Return (RPA2)
      } Else {
        Return (0x001C0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,2)
    Method(_INI)
    {
      Store (LTR2, LTRZ)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      If (HBSL & 0x1) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x02),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP02.PPRW)) {
        Return (\_SB.PC00.RP02.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #02"

  //
  // PCIE Root Port #03
  //
  Device(RP03) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA3,0)) {
        Return (RPA3)
      } Else {
        Return (0x001C0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,3)
    Method(_INI)
    {
      Store (LTR3, LTRZ)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      If (HBSL & 0x1) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x04),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP03.PPRW)) {
        Return (\_SB.PC00.RP03.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #03"

  //
  // PCIE Root Port #04
  //
  Device(RP04) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA4,0)) {
        Return (RPA4)
      } Else {
        Return (0x001C0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,4)
    Method(_INI)
    {
      Store (LTR4, LTRZ)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      If (HBSL & 0x1) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x08),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP04.PPRW)) {
        Return (\_SB.PC00.RP04.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #04"

  //
  // PCIE Root Port #05
  //
  Device(RP05) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA5,0)) {
        Return (RPA5)
      } Else {
        Return (0x001C0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,5)
    Method(_INI)
    {
      Store (LTR5, LTRZ)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      If (HBSL & 0x2) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x10),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP05.PPRW)) {
        Return (\_SB.PC00.RP05.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #05"

  //
  // PCIE Root Port #06
  //
  Device(RP06) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA6,0)) {
        Return (RPA6)
      } Else {
        Return (0x001C0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,6)
    Method(_INI)
    {
      Store (LTR6, LTRZ)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
      If (HBSL & 0x2) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x20),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP06.PPRW)) {
        Return (\_SB.PC00.RP06.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #06"

  //
  // PCIE Root Port #07
  //
  Device(RP07) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA7,0)) {
        Return (RPA7)
      } Else {
        Return (0x001C0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,7)
    Method(_INI)
    {
      Store (LTR7, LTRZ)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
      If (HBSL & 0x2) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x40),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP07.PPRW)) {
        Return (\_SB.PC00.RP07.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #07"

  //
  // PCIE Root Port #08
  //
  Device(RP08) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA8,0)) {
        Return (RPA8)
      } Else {
        Return (0x001C0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,8)
    Method(_INI)
    {
      Store (LTR8, LTRZ)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
      If (HBSL & 0x2) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR1,0x80),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP08.PPRW)) {
        Return (\_SB.PC00.RP08.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #08"

  //
  // PCIE Root Port #09
  //
  Device(RP09) {
    Method (_ADR, 0) {
      If (LNotEqual(RPA9,0)) {
        Return (RPA9)
      } Else {
        Return (0x001D0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,9)
    Method(_INI)
    {
      Store (LTR9, LTRZ)
      Store (PML9, LMSL)
      Store (PNL9, LNSL)
      If (HBSL & 0x4) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x01),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP09.PPRW)) {
        Return (\_SB.PC00.RP09.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #09"

  //
  // PCIE Root Port #10
  //
  Device(RP10) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAA,0)) {
        Return (RPAA)
      } Else {
        Return (0x001D0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,10)
    Method(_INI)
    {
      Store (LTRA, LTRZ)
      Store (PMLA, LMSL)
      Store (PNLA, LNSL)
      If (HBSL & 0x4) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x02),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP10.PPRW)) {
        Return (\_SB.PC00.RP10.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #10"

  //
  // PCIE Root Port #11
  //
  Device(RP11) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAB,0)) {
        Return (RPAB)
      } Else {
        Return (0x001D0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,11)
    Method(_INI)
    {
      Store (LTRB, LTRZ)
      Store (PMLB, LMSL)
      Store (PNLB, LNSL)
      If (HBSL & 0x4) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x04),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP11.PPRW)) {
        Return (\_SB.PC00.RP11.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #11"

  //
  // PCIE Root Port #12
  //
  Device(RP12) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAC,0)) {
        Return (RPAC)
      } Else {
        Return (0x001D0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,12)
    Method(_INI)
    {
      Store (LTRC, LTRZ)
      Store (PMLC, LMSL)
      Store (PNLC, LNSL)
      If (HBSL & 0x4) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x08),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP12.PPRW)) {
        Return (\_SB.PC00.RP12.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #12"

  //
  // PCIE Root Port #13
  //
  Device(RP13) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAD,0)) {
        Return (RPAD)
      } Else {
        Return (0x001D0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,13)
    Method(_INI)
    {
      Store (LTRD, LTRZ)
      Store (PMLD, LMSL)
      Store (PNLD, LNSL)
      If (HBSL & 0x8) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x10),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP13.PPRW)) {
        Return (\_SB.PC00.RP13.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #13"

  //
  // PCIE Root Port #14
  //
  Device(RP14) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAE,0)) {
        Return (RPAE)
      } Else {
        Return (0x001D0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,14)
    Method(_INI)
    {
      Store (LTRE, LTRZ)
      Store (PMLE, LMSL)
      Store (PNLE, LNSL)
      If (HBSL & 0x8) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x20),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP14.PPRW)) {
        Return (\_SB.PC00.RP14.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #14"

  //
  // PCIE Root Port #15
  //
  Device(RP15) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAF,0)) {
        Return (RPAF)
      } Else {
        Return (0x001D0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,15)
    Method(_INI)
    {
      Store (LTRF, LTRZ)
      Store (PMLF, LMSL)
      Store (PNLF, LNSL)
      If (HBSL & 0x8) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x40),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP15.PPRW)) {
        Return (\_SB.PC00.RP15.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #15"

  //
  // PCIE Root Port #16
  //
  Device(RP16) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAG,0)) {
        Return (RPAG)
      } Else {
        Return (0x001D0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,16)
    Method(_INI)
    {
      Store (LTRG, LTRZ)
      Store (PMLG, LMSL)
      Store (PNLG, LNSL)
      If (HBSL & 0x8) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR2,0x80),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP16.PPRW)) {
        Return (\_SB.PC00.RP16.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #16"

  //
  // PCIE Root Port #17
  //
  Device(RP17) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAH,0)) {
        Return (RPAH)
      } Else {
        Return (0x001B0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,17)
    Method(_INI)
    {
      Store (LTRH, LTRZ)
      Store (PMLH, LMSL)
      Store (PNLH, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x01),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP17.PPRW)) {
        Return (\_SB.PC00.RP17.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #17"

  //
  // PCIE Root Port #18
  //
  Device(RP18) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAI,0)) {
        Return (RPAI)
      } Else {
        Return (0x001B0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,18)
    Method(_INI)
    {
      Store (LTRI, LTRZ)
      Store (PMLI, LMSL)
      Store (PNLI, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x02),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP18.PPRW)) {
        Return (\_SB.PC00.RP18.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #18"

  //
  // PCIE Root Port #19
  //
  Device(RP19) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAJ,0)) {
        Return (RPAJ)
      } Else {
        Return (0x001B0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,19)
    Method(_INI)
    {
      Store (LTRJ, LTRZ)
      Store (PMLJ, LMSL)
      Store (PNLJ, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x04),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP19.PPRW)) {
        Return (\_SB.PC00.RP19.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #19"

  //
  // PCIE Root Port #20
  //
  Device(RP20) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAK,0)) {
        Return (RPAK)
      } Else {
        Return (0x001B0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,20)
    Method(_INI)
    {
      Store (LTRK, LTRZ)
      Store (PMLK, LMSL)
      Store (PNLK, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x08),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP20.PPRW)) {
        Return (\_SB.PC00.RP20.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #20"

  //
  // PCIE Root Port #21
  //
  Device(RP21) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAL,0)) {
        Return (RPAL)
      } Else {
        Return (0x001B0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,21)
    Method(_INI)
    {
      Store (LTRL, LTRZ)
      Store (PMLL, LMSL)
      Store (PNLL, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x10),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP21.PPRW)) {
        Return (\_SB.PC00.RP21.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #21"

  //
  // PCIE Root Port #21
  //
  Device(RP22) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAM,0)) {
        Return (RPAM)
      } Else {
        Return (0x001B0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,22)
    Method(_INI)
    {
      Store (LTRM, LTRZ)
      Store (PMLM, LMSL)
      Store (PNLM, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x20),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP22.PPRW)) {
        Return (\_SB.PC00.RP22.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #22"

  //
  // PCIE Root Port #22
  //
  Device(RP23) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAN,0)) {
        Return (RPAN)
      } Else {
        Return (0x001B0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,23)
    Method(_INI)
    {
      Store (LTRN, LTRZ)
      Store (PMLN, LMSL)
      Store (PNLN, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x40),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP23.PPRW)) {
        Return (\_SB.PC00.RP23.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #23"

  //
  // PCIE Root Port #21
  //
  Device(RP24) {
    Method (_ADR, 0) {
      If (LNotEqual(RPAO,0)) {
        Return (RPAO)
      } Else {
        Return (0x001B0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,24)
    Method(_INI)
    {
      Store (LTRO, LTRZ)
      Store (PMLO, LMSL)
      Store (PNLO, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If(LAnd(LEqual(VMDE,1),LNotEqual(And(VMR3,0x80),0))) {
          Store (1, PRMV)
        }
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PC00.RP24.PPRW)) {
        Return (\_SB.PC00.RP24.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #24"

  //
  // Serial ATA Host Controller - Device 31, Function 2
  //
  Include ("PchSata.asl")
}


Include ("Gpio.asl")


//
//  Serial IO Controllers definitions
//
Include ("PchSerialIo.asl")

//
// Integrated Sensor Hub definition
//
Include ("PchIsh.asl")

//
// MEI 1 definition
//
Include ("PchHeci.asl")

//
// Integrated Connectivity definition
//
Include ("Cnvi.asl")

Include ("PchTraceHub.asl")


#ifdef PCH_TGL
//
// Timed GPIO
//
Include ("TimedGpio.asl")
//
// GBE TSN
//
Include ("PchTsn.asl")
#endif

