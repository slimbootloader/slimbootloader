/**@file


 Copyright (c) 1999 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include <Register/PchRegsPcie.h>
//#include <Register/PmcRegs.h>
#include <Register/PchDmiRegs.h>

Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length
Include ("PchNvs.asl")
Include ("PchClockControl.asl")

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

Name(PCHH,PCH_H) // PCH H
Name(PCHN,PCH_N) // PCH N
Name(PCHL,PCH_LP) // PCH LP

External(\_SB.PC00.RP01.PPRW, MethodObj)
External(\_SB.PC00.RP02.PPRW, MethodObj)
External(\_SB.PC00.RP03.PPRW, MethodObj)
External(\_SB.PC00.RP04.PPRW, MethodObj)
External(\_SB.PC00.RP05.PPRW, MethodObj)
External(\_SB.PC00.RP06.PPRW, MethodObj)
External(\_SB.PC00.RP07.PPRW, MethodObj)
//
// PME event (PCH's GPE 6Dh) is received when any PCH internal device with
// PCI Power Management capabilities on bus 0 asserts the equivalent of the PME# signal.
// Dedicated _L6D method is not needed if its only purpose is to notify the device.
// Instead to properly handle this GPE event below requirements need to be fulfilled
// - _PRW for GPE 6Dh in devices capable of generating PME (XHCI, XDCI, GbE, HDA)
// - _S0W(3) and _DSW is present within those devices
//

Scope (\_SB) {

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
        // 0xFC800000 to 0xFE7FFFFF
        // skip over address range that might be claimed by the Intel Serial IO, and TraceHub
        // The SerialIO ranges will be handled by SIRC device.
        //
        // skip 0xFC800000 - 0xFCFFFFFF for PCH Trace Hub : SW BAR space
        Memory32Fixed(ReadWrite,0xFD000000,0x00690000) // 0xFD000000 - 0xFD68FFFF
        Memory32Fixed(ReadWrite,0xFD6F0000,0x00910000) // 0xFD6F0000 - 0xFDFFFFFF
        Memory32Fixed(ReadWrite,0xFE000000,0x00020000) // 0xFE000000 - 0xFE01FFFF
        // Skip 0xFE020000 - 0xFE035FFF for Serial IO
        // skip 0xFE100000 - 0xFE1FFFFF for PCH Trace Hub : MTB BAR space
        Memory32Fixed(ReadWrite,0xFE200000,0x00600000) // 0xFE200000 - 0xFE7FFFFF

        // BIOS decoding range. Assume it's always 16MB.
        Memory32Fixed(ReadOnly,0xFF000000,0x1000000)
        // ACPI Base
        IO(Decode16,0,0,0x1,0xFF,ABAR)
      })
      // Update ACPI IO region
      CreateWordField(BUF0,ABAR._MIN,AMIN)
      CreateWordField(BUF0,ABAR._MAX,AMAX)
      Store(PMBS, AMIN)
      Store(PMBS, AMAX)

      //
      // Handle resources assigned to GPIO communites that are not claimed
      // by GPIO OS driver
      //
      If(LEqual(PCHS, PCHH)){
        Name(BUFH,ResourceTemplate(){
          Memory32Fixed(ReadWrite,0xFD6C0000,0x00010000) // 0xFD6C0000 - 0xFD6CFFFF - Community 2
        })
        Return(ConcatenateResTemplate(BUF0, BUFH))
      } Else {
      Name(BUFL,ResourceTemplate(){
        Memory32Fixed(ReadWrite,0xFD6B0000,0x00020000) // 0xFD6B0000 - 0xFD6CFFFF - Community 2,3
      })

      Return(ConcatenateResTemplate(BUF0, BUFL))
      }
    }
  }

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
  // SPI Bridge - Device 31, Function 5
  //
  Device(SPIF) {
    Name(_ADR, 0x001F0005)

    // It is expected for bootloader to check and clear SPI Synchronous SMI Status bit prior to S0ix entry,
    // as this status bit might cause SPI driver unable to turn off clock source properly.
    // This method will check if Synchronous SMI Status bit is on, then reenable Write Protect Disable bit
    // and clear Synchronous SMI Status bit.
    Method(SPIS)
    {
      If (LEqual (SYNS, 0x1)) {
        store(0, WPDL)
        store(1, SYNS)
      }
    }

    OperationRegion(SPIC, PCI_Config, 0x00, 0x100)
    Field(SPIC, AnyAcc, NoLock, Preserve)
    {
      Offset(R_LPC_CFG_BC), // 0xDC
      WPDL,   1,
      ,       7,
      SYNS,   1,
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
  // Serial ATA Host Controller - Device 31, Function 2
  //
  Include ("PchSata.asl")
}

//
//  GPIO ACPI device
//
Include ("Gpio.asl")

//
//  Serial IO Controllers definitions
//
Include ("PchSerialIo.asl")

//
// Storage and Communication Subsystems definitions
//
Include ("PchScs.asl")

//
// Programmable Service Engine definition
//
Include ("PchPse.asl")

//
// GBE TSN
//
Include ("PchTsn.asl")

//
// Intel Safety Island (ISI)
//
Include ("PchIsi.asl")

//
// Touch Host Controllers definition
//
Include ("Thc.asl")

//
// MEI 1 definition
//
Include ("PchHeci.asl")

//
// Integrated Connectivity definition
//
Include ("Cnvi.asl")

//
// PCH Trace Hub, report ACPI deivce for host debugger mode.
//
If (LEqual(PTHM, 2)) {
  Include ("PchTraceHub.asl")
}

//
// Timed GPIO
//
Include ("TimedGpio.asl")

