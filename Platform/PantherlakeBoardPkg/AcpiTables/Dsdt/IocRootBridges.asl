/** @file
  IOC host bridge devices declaration.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/PtlPcdInfoLib.h>

Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length

Include ("PchNvs.asl")

Scope (\_SB) {
  //
  // SOC root bridge
  //
  Device (PC00) {
    Name (_HID, EISAID ("PNP0A08")) // Indicates PCI Express/PCI-X Mode2 host hierarchy
    Name (_CID, EISAID ("PNP0A03")) // To support legacy OS that doesn't understand the new HID
    Name (_SEG, 0)
    Name (_BBN, 0)
    Name (_UID, 0x0)

    Include ("HostBus.asl")
  }

  //
  // PCH root bridge
  //
  If (LEqual (PCHA, 1)) {
    Device (PC02) {
      Name (_HID, EISAID ("PNP0A08")) // Indicates PCI Express/PCI-X Mode2 host hierarchy
      Name (_CID, EISAID ("PNP0A03")) // To support legacy OS that doesn't understand the new HID
      Name (_SEG, 0)
      Method (_BBN) {
        Return (PBSB)
      }
      Name (_UID, 0x2)
      //
      // Note that the memory range values given below are only an example and might change during boot
      //
      Name (BUF0, ResourceTemplate ()
      {
        WORDBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode, 0x00, 0x81, 0xFF, 0x00, 0x007F,,,PB01)
        DWordIo (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x00, 0x8000, 0xFFFF, 0x00, 0x8000,,,PI01)
        DWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x00, 0xB0000000, 0xBFFFFFFF, 0x00, 0x10000000,,,PM01)
        QWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x00, 0x4000000000, 0x5FFFFFFFFF, 0x00, 0x2000000000,,,PQ01)
      }
      )

      Method (_CRS, 0, Serialized) {
        //
        // PCH BUS
        //
        CreateWordField (BUF0, ^PB01._MAX, PBMX)
        CreateWordField (BUF0, ^PB01._MIN, PBMN)
        CreateWordField (BUF0, ^PB01._LEN, PBLN)

        Store (PBSB, PBMN)
        Store (PBSL, PBMX)
        Subtract (PBMX, PBMN, PBLN)
        Add (PBLN, 1, PBLN)

        //
        // PCH IO
        //
        CreateDwordField (BUF0, ^PI01._MIN, I1MN)
        CreateDwordField (BUF0, ^PI01._MAX, I1MX)
        CreateDwordField (BUF0, ^PI01._LEN, I1LN)

        Store (PIOB, I1MN)
        Store (PIOL, I1MX)
        Subtract (I1MX, I1MN, I1LN)
        Add (I1LN, 1, I1LN)

        //
        // PCH MEM32
        //
        CreateDwordField (BUF0, ^PM01._MIN, M1MN)
        CreateDwordField (BUF0, ^PM01._MAX, M1MX)
        CreateDwordField (BUF0, ^PM01._LEN, M1LN)

        Store (PMEB, M1MN)
        Store (PMEL, M1MX)
        Subtract (M1MX, M1MN, M1LN)
        Add (M1LN, 1, M1LN)

        //
        // PCH MEM64
        //
        CreateQwordField (BUF0, ^PQ01._LEN, M2LN)
        CreateQwordField (BUF0, ^PQ01._MIN, M2MN)
        CreateQwordField (BUF0, ^PQ01._MAX, M2MX)

        Store (PPMB, M2MN)
        Store (PPML, M2MX)
        Subtract (M2MX, M2MN, M2LN)
        Add (M2LN, 1, M2LN)

        Return (BUF0)
      }

      //
      //Name(GUID,UUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
      //
      Name (GUID,Buffer () {0x5b, 0x4d, 0xdb, 0x33,
        0xf7, 0x1f,
        0x1c, 0x40,
        0x96, 0x57,
        0x74, 0x41, 0xc0, 0x3d, 0xd7, 0x66})


      Name (SUPP,0)  // PCI _OSC Support Field value
      Name (CTRL,0)  // PCI _OSC Control Field value

      Method (_OSC,4,Serialized) {
        //
        // Check for proper UUID
        // Save the capabilities buffer
        //
        Store (Arg3,Local0)

        //
        // Create DWord-adressable fields from the Capabilties Buffer
        //
        CreateDWordField (Local0,0,CDW1)
        CreateDWordField (Local0,4,CDW2)
        CreateDWordField (Local0,8,CDW3)

        If (LNotEqual (Arg0, GUID)) {
          Or (CDW1, 0x4, CDW1)
          Return (Local0)
        }

        If (LNotEqual (Arg1, One)) {
          Or (CDW1, 0x8, CDW1)
          Return (Local0)
        }

        // Save Capabilities DWord2 & 3
        Store (CDW2,SUPP)
        Store (CDW3,CTRL)

        ADBG ("PC02 Support")
        ADBG (ToHexString (SUPP))
        ADBG ("PC02 Control")
        ADBG (ToHexString (CTRL))

        If (LNotEqual (CDW3,CTRL)) {
          ADBG ("Rejecting OS request new control")
          ADBG (ToHexString (CTRL))
          Or (CDW1,0x10,CDW1)
          Store (CTRL,CDW3)
        }
        Return (Local0)
      }
    }
  }
}
