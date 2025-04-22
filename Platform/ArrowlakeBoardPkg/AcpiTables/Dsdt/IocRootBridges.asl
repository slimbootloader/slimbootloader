/** @file
  IOC host bridge devices declaration.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/MtlSocInfoLib.h>

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

  // In SSDT - PC01 TCSS root bridge

  //
  // PCH root bridge
  //
  If (LNotEqual (PCHA, 0)) {
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
    }
  }
}