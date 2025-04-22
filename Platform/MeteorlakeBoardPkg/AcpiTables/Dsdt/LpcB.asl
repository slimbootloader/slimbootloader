/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


// LPC Bridge - Device 31, Function 0
scope (\_SB.PC00.LPCB) {
      Include ("LpcDev.asl")
      Include ("H8S2113Sio.asl")
      Include ("PlatformEcMethods.asl")
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
        If (SCMK) //Scan Matrix Keyboard Support Enable
        {
          Return (0x000F)
        }
      }
      Return (0x0000)
    }

    Name (_CRS,ResourceTemplate ()
    {
      IRQ (Edge,ActiveHigh,Exclusive) {0x0C}
    })
  }
}
