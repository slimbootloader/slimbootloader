/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PchInfoLib.h>
#include <Register/EspiRegs.h>
#include <Acpi/Register/PmcRegs.h>

#define ACPI_PCI_ADDR(Device, Function)  Or(ShiftLeft(Device, 16), Function)

#include "PchNvs.asl"

//
// Include GPIO macros and access library
//
#include "IblUart.asl"
#include "Acpi/GpioAcpiDefinesS3m2.h"
#include "Gpio.asl"

//
// PME event (PCH's GPE 6Dh) is received when any PCH internal device with
// PCI Power Management capabilities on bus 0 asserts the equivalent of the PME# signal.
// Dedicated _L6D method is not needed if its only purpose is to notify the device.
// Instead to properly handle this GPE event below requirements need to be fulfilled
// - _PRW for GPE 6Dh in devices capable of generating PME (XHCI, XDCI, GbE, HDA)
// - _S0W(3) and _DSW is present within those devices
//
//

Scope (\_SB.PC00) {
  Name(TEMP,0)                          // scratch variable that does not cause compiler warnings as writes to Localx variables do.

  //
  // PCH reserved resource
  //
  Device(PRRE) {
    Name(_HID,EISAID("PNP0C02")) // motherboard resource
    Name(_UID,"PCHRESV")
    Name(_STA,0x3) // device present and decodes its resources, but not to be displayed in OSPM

    Method(_CRS,0,Serialized)
    {
      Name(BUF0,ResourceTemplate(){
        //
        // PCH/IBL RESERVED MMIO RANGE
        // Refer to PchReservedResources.h for ranges used by PCH/IBL
        //
        // 0xF6800000 to 0xF8000000
        //
        Memory32Fixed(ReadWrite,0xF6800000,0x00010000) // 0xF6800000 - 0xF680FFFF
        // skip 0xF6810000 - 0xF682FFFF for eSPI
        Memory32Fixed(ReadWrite,0xF6830000,0x017D0000) // 0xF6830000 - 0xF7FFFFFF

        // SBL region decoding range. Assume it's always 16MB.
        Memory32Fixed(ReadOnly,0xFF000000,0x1000000)
        // ACPI Base
        IO(Decode16,0,0,0x1,0xFF,ABAR)
      })
      // Update ACPI IO region
      CreateWordField(BUF0,ABAR._MIN,AMIN)
      CreateWordField(BUF0,ABAR._MAX,AMAX)
      Store(PMBS, AMIN)
      Store(PMBS, AMAX)

      Return(BUF0)
    }
  }
  Device(IOTR) {
    //
    // This device claims IO range reserved for IO traps
    // to prevent OS from reusing it for other purposes
    //
    Name(_HID,EISAID("PNP0C02"))
    Name(_UID,"IoTraps")
    Name(BUF0,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG0) })
    Name(BUF1,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG1) })
    Name(BUF2,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG2) })
    Name(BUF3,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG3) })
    CreateWordField(BUF0,TAG0._MIN,AMI0)
    CreateWordField(BUF0,TAG0._MAX,AMA0)
    CreateWordField(BUF1,TAG1._MIN,AMI1)
    CreateWordField(BUF1,TAG1._MAX,AMA1)
    CreateWordField(BUF2,TAG2._MIN,AMI2)
    CreateWordField(BUF2,TAG2._MAX,AMA2)
    CreateWordField(BUF3,TAG3._MIN,AMI3)
    CreateWordField(BUF3,TAG3._MAX,AMA3)
    Method(_CRS) {
      Store(Zero, Local0)
      Store(ITA0,AMI0);Store(ITA0,AMA0)
      Store(ITA1,AMI1);Store(ITA1,AMA1)
      Store(ITA2,AMI2);Store(ITA2,AMA2)
      Store(ITA3,AMI3);Store(ITA3,AMA3)
      // Win7 can't store result of ConcatenateResTemplate directly into one of its input parameters
      if(LEqual(ITS0,1)) { ConcatenateResTemplate(Local0, BUF0, Local1); Store(Local1, Local0) }
      if(LEqual(ITS1,1)) { ConcatenateResTemplate(Local0, BUF1, Local1); Store(Local1, Local0) }
      if(LEqual(ITS2,1)) { ConcatenateResTemplate(Local0, BUF2, Local1); Store(Local1, Local0) }
      if(LEqual(ITS3,1)) { ConcatenateResTemplate(Local0, BUF3, Local1); Store(Local1, Local0) }
      return (Local0)
    }
  }


  //
  // LPC Bridge - Device 31, Function 0, this is only for PCH register Memory Region declare,
  // it's better to be declared as early as possible since it's widely used in whole ACPI name space.
  // Please add any code which needs to reference any register of it after this
  //
  Device(LPC0) {
    Name(_ADR, 0x001F0000)

    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}

    OperationRegion(LPC, PCI_Config, 0x00, 0x100)
    Field(LPC, AnyAcc, NoLock, Preserve)
    {
      Offset(0x02),
      CDID, 16,
      Offset(0x08),
      CRID,  8,
      Offset(R_ESPI_CFG_ESPI_IOD_IOE), // 0x80
      IOD0,  8,
      IOD1,  8,
      Offset(0xA0),
      , 9,
      PRBL,  1,
      Offset(R_ESPI_CFG_ESPI_BC),  // 0xDC
      ,      2,
      ESPI,  1,
    }
  }

  //
  // SMBus Controller - Device 31, Function 4
  //
  Device(SBUS) {
    Name(_ADR,0x001F0004)
    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
  }
}
