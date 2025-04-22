/** @file
  ACPI DSDT table

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <CpuRegs.h>
#include <CpuGenInfo.h>
External(\_SB.CPID)

// LPC Bridge - Device 31, Function 0
Scope (\_SB.PC00.LPCB) {
  Include ("LpcDev.asl")
  //Include ("NatSio.asl")
  Include ("H8S2113Sio.asl")
  //Include ("Nct6776FSio.asl")
  //Include ("Sch3223Sio.asl")
  Include ("PlatformEcMethods.asl")
  Include ("IteSio.asl")

  // Define the KBC_COMMAND_REG-64, KBC_DATA_REG-60 Registers as an ACPI Operating
  // Region.  These registers will be used to skip kbd mouse
  // resource settings if not present.
  OperationRegion (PKBS, SystemIO, 0x60, 0x05)
  Field (PKBS, ByteAcc, Lock, Preserve)
  {
    PKBD, 8,
    Offset(4),
    PKBC, 8
  }
  Device (PS2K)    // PS2 Keyboard
  {
    Name (_HID,"MSFT0001")
    Name (_CID,EISAID ("PNP0303"))

    Method (_STA)
    {
      If (SCMK) //Scan matrix Keyboard Support Enable
      {
        Return (0x000F)
      }
      Return (0x0000)
    }

    Name (_CRS,ResourceTemplate ()
    {
      IO (Decode16,0x60,0x60,0x01,0x01)
      IO (Decode16,0x64,0x64,0x01,0x01)
      IRQ (Edge,ActiveHigh,Exclusive) {0x01}
    })
  }

  Device (PS2M)    // PS/2 Mouse
  {
    Name (_HID,"MSFT0003")
    Name (_CID,EISAID ("PNP0F03"))

    Method (_STA)
    {
      If (P2ME) //Ps2 Mouse Enable
      {
        Return (0x000F)
      }
      Return (0x0000)
    }

    Name (_CRS,ResourceTemplate ()
    {
      IRQ (Edge,ActiveHigh,Exclusive) {0x0C}
    })
  }
}

#if (FixedPcdGet8(PcdMtlSSupport) == 1)
Scope (\_SB) {
  Device (CWDT)
  {
    Method (_HID) {
      If (LOr (LEqual (And (\_SB.CPID, CPUID_FULL_FAMILY_MODEL), CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H),
               LEqual (And (\_SB.CPID, CPUID_FULL_FAMILY_MODEL), CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_U))) {
        Return ("INTC109F")
      } ElseIf (LEqual (And (\_SB.CPID, CPUID_FULL_FAMILY_MODEL), CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO)) {
        Return ("INTC109F")
      } Else {
        Return ("INTC109D")
      }
    }

    Name (_CID,EISAID ("PNP0C02"))

    Method (_STA,0,Serialized)
    {
      Return (0x0F)
    }

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBUF, ResourceTemplate () {
        IO (Decode16, 0, 0, 0x4, 0x4,OCWD)  // ACPIBASE + OcWdtCtl offset
      })

      CreateWordField (RBUF,OCWD._MIN,OMIN)
      CreateWordField (RBUF,OCWD._MAX,OMAX)

      Add (PMBS,R_ACPI_IO_OC_WDT_CTL,OMIN)
      Add (PMBS,R_ACPI_IO_OC_WDT_CTL,OMAX)

      Return (RBUF)
    }
  }
}
#endif
