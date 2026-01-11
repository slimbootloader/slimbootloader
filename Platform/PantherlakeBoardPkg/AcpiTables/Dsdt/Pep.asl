/** @file
  ACPI uPEP Support

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PEP_CONSTRAINT_
#define _PEP_CONSTRAINT_

External (\_SB.PC00.IMNG, MethodObj)
External (S0ID, FieldUnitObj)
External (PSON, FieldUnitObj)

Scope (\_SB)
{
  Device (PEPD) {
    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)

    Name (DEVY, Package () {
      Include ("DummyPepDevices.asl")
    })

    Name (BCCD, Package () // Bugcheck Critical Device (s)
    {  })

    //
    // _DSD to notify OSPM about CEC PS_ON support.
    //
    Name (_DSD, Package (0x02) { // Using Name object to be compatible for different OSPM
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package (0x01) {
        Package (0x02) {
          "intel-cec-pson-switching-enabled-in-s0",
          0 // to be updated in PSOP
        }
      }
    })

    Method (PSOP, 0, NotSerialized) { // PSON Pep Check
      //
      // Loop through _DSD object to find index for UUID of interest.
      // Stride length is 2 because list of properties comes right after the corresponding UUID.
      //
      For (Local0 = Zero, Local0 < SizeOf (_DSD), Local0 += 2) {
        If (LNotEqual (DerefOf (_DSD[Local0]), ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"))) {
          // skip if not the uuid of interest
          Continue
        }
        // get the package associated with this UUID
        Local1 = DerefOf (_DSD[Add (Local0, 1)])
        For (Local2 = Zero, Local2 < SizeOf (Local1), Increment (Local2)) {
          // get the key/value pair at the index
          Local3 = DerefOf (Local1[Local2])
          If (LAnd (LEqual ("intel-cec-pson-switching-enabled-in-s0", DerefOf (Local3[Zero])), PSON)) {
            ADBG ("PEPD: Storing 1 in _DSD Object")
            DerefOf (DerefOf (_DSD[Local0 + 1])[Local2])[One] = One
          }
        }
      }
      Return
    }

    Method (_STA, 0x0, NotSerialized)
    {
      If (LEqual (S0ID, 1))
      {
        PSOP () // Update _DSD
        Return (0xF)
      }
      Return (0)
    }

    Name (IPDI, 0x0) // Is Pep Device List Initialized

    Include ("PepCsMethods.asl")

    Include ("PepDevicesInit.asl")

    Method (_DSM, 0x4, Serialized)
    {
      ADBG ("Pep _DSM!")

      If (LEqual (Arg0,ToUUID ("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      {
        // Number of Functions (including this one)
        If (LEqual (Arg2, Zero))
        {
          Return (Buffer (One) {0x7F})
        }
        // Device Constraints Enumeration
        If (LEqual (Arg2, One))
        {
          If (LEqual (S0ID, 0)) {
            Return (Package () {})
          }

          If (LEqual (IPDI, 0)) { // Check if DEVY is already initialzed or not.
             UPDV ()
             Store(1, IPDI)
          } Else {
            ADBG ("PEP DEVY is already initialized !!! ")
          }
          Return (DEVY)
        }

        // BCCD
        If (LEqual (Arg2, 2))
        {
          Return (BCCD)
        }

        // Screen off notification
        If (LEqual (Arg2, 0x3))
        {
          // no action for screen off
          If (CondRefOf (\_SB.PC00.IMNG)) {
            \_SB.PC00.IMNG (Arg2)
          }
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0)
          }
        }

        // Screen on notification
        If (LEqual (Arg2, 0x4))
        {
          // no action for screen on
          If (CondRefOf (\_SB.PC00.IMNG)) {
            \_SB.PC00.IMNG (Arg2)
          }
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0xAB)
          }
        }

        // resiliency phase entry (deep standby entry)
        If (LEqual (Arg2, 0x5))
        {
          ADBG ("[PEP] deep standby entry")
          PESB (1)
        }

        // resiliency phase exit (deep standby exit)
        If (LEqual (Arg2, 0x6))
        {
          ADBG ("[PEP] deep standby exit")
          PESB (0)
        }
      }// If (LEqual (Arg0,ToUUID ("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))

      Return (Buffer () {0})
    } // Method (_DSM)
  }
}

#endif
