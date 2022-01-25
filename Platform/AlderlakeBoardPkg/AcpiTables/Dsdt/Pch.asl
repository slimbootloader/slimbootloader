/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PlatformBoardId.h>
//
// Since asl compiler update to 2018 version
// Empty template will casuse warrning
// using this Macro to replace empty ResourceTemplate with End Tag
//
#ifndef EMPTY_RESOURCE_TEMPLATE
#define EMPTY_RESOURCE_TEMPLATE Buffer(){0x79, 0x00}
#endif

#include <Library/PchInfoLib.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PmcRegs.h>
#include <GpioAcpiDefines.h>
#include <PcieRpMacroDefinition.h>

#define PCR_MMIO_SIZE_PER_PID       0x10000        ///< 64KB
#define PCR_MMIO_SIZE               0x01000000     ///< 16MB
#define PCR_MMIO_SIZE_EX            0x10000000     ///< 256MB, used by SBREG_BAR 20-bit address platforms

Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length
Include ("PchNvs.asl")

//
// Include GPIO macros and access library
//
Include("HsioDefine.asl")
Include("HsioLib.asl")

//
// Trace Hub debug library
// Include it earlier so the debug function can be used as soon as possible
//
Include ("TraceHubDebug.asl")

Name(PCHH,PCH_H)  // PCH H
Name(PCHN,PCH_N)  // PCH N
Name(PCHL,PCH_LP) // PCH LP
Name(PCHP,PCH_P)  // PCH P
Name(PCHM,PCH_M)  // PCH M
Name(PCHX,PCH_S)  // PCH S


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
  // PCH reserved resources
  //
  Device(PRRE) {
    Name(_HID,EISAID("PNP0C02")) // motherboard resource
    Name(_UID,"PCHRESV")
    Name(_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM

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
        Return(EMPTY_RESOURCE_TEMPLATE)
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
Include("Pmc.asl")

Scope (\_SB.PC00) {
  Name(LTEN, 0)
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

      //
      // Define a Memory Region that will allow access to the GLAN PCI Configuration Space
      //
      OperationRegion(LANR, PCI_Config, 0x00, 0x100)
      Field(LANR,ByteAcc,NoLock,Preserve) {
        DVID, 16,
        Offset(0xCC), // R_GBE_CFG_PMCS Power management control and status PMCS_DR
        ,8,
        PMEE,1,
        ,6,
        PMES,1
      }

      Method(GPEH) {
        If(LEqual(^DVID,0xFFFF)) {
          Return
        }
        If(LEqual(PMES,1)) {
          Notify(\_SB.PC00.GLAN, 0x02)
        }
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

  If (LNotEqual(UAOE,0)) {
    //
    // High Definition Audio - USB Audio Offload
    //
    include("PchHdaUsbAudioOffload.asl")
  }

  //
  // High Definition Audio - Intel Display Audio
  //
  include("PchHdaIntelDisplayAudio.asl")
}

Include ("PchFru.asl")

//
// MEI 1 definition
//
Include ("PchHeci.asl")

Include ("PchTraceHub.asl")

//
// Timed GPIO
//
Include ("TimedGpio.asl")
//
// GBE TSN
//
Include ("PchTsn.asl")
