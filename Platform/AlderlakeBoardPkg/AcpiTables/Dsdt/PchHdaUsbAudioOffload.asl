/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// USB Audio Offload SIO Controllers definition
//
Scope(HDAS)
{
  //
  // Address (_ADR) encoding:
  // Bits 28-31 - Link Type
  // 0 = HD-Audio, 1 = DSP, 2 = PDM, 3 = SSP, 4 = SoundWire, 5 = SIO / USB Audio Offload
  // Bits 0-3 - Device Instance ID (unique to virtual bus).

  //
  // USB Audio Offload Controller
  //
  Device(UAOL) {
    Name(_ADR, 0x50000000)

    Name(_CID, Package() {
      // Precedence order does matter for evaluation of list
      "PRP00001",  // to indicate that we want to use DeviceTree-like "compatible" matching, Linux only
      "PNP0A05"    // generic container device always placed last, makes sure entry is ignored by Windows with no yellow bangs if there is no matching driver
    })

    Method(_STA, 0, NotSerialized) { // _STA: Status
      Return (0x0B)
    }

    //
    // UAOL _DSM method
    //
    Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      // Arg0 - UUID: 2E60AEFC-1BA8-467A-B8CC-5727B98CECB7 (Buffer)
      // Arg1 - Revision ID: 0x01 (Integer)
      // Arg2 - Function Index: 0x0 - 0x3 (Integer) - See below for details.
      // Arg3 - Depends on Function Index - See below for details.
      // Return - Depends on Function Index - See below for details.

      ADBG("UAOL _DSM")

      // Verify UUID
      If (LEqual(Arg0, ToUUID ("2E60AEFC-1BA8-467A-B8CC-5727B98CECB7"))) {
        Switch(ToInteger(Arg2)) {
          Case(0) {
            // Supports function 0 - 1
            Return(Buffer(One) { 0x03 })
          }
          Case(1) {
            Return(CFAE(Arg3))
          }
        }
      }
      Return(Buffer() {0})
    }

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"uaol-ctrl-count", 2}
      },
      // Properties for the USB Audio Offload link instances
      ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
      Package () {
        Package (2) {"uaol-descriptor-0", "UAO0"}, // Link Descriptor 0
        Package (2) {"uaol-descriptor-1", "UAO1"}, // Link Descriptor 1
        // Package (2) {"uaol-descriptor-N", "UAO[N]"}, // Link Descriptor N
      }
    }) // END Name(_DSD)

    Name(UAO0, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"uaol-instance-number", 0}, // Integer
        Package (2) {"bdf-routing-enabled", 0}, // Integer/Boolean
        // When the bdf-routing-enabled is set to true, peer-integrated-controller-identifier field is invalid.
        Package (2) {"peer-integrated-controller-identifier", 0x98ED}, // Type/Value: TBD
      }
    }) // END Name(UAO0)

    Name(UAO1, Package() {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"uaol-instance-number", 1}, // Integer
        Package (2) {"bdf-routing-enabled", 0}, // Integer/Boolean
        // When the bdf-routing-enabled is set to true, peer-integrated-controller-identifier field is invalid.
        Package (2) {"peer-integrated-controller-identifier", 0xABCD}, // Type/Value: TBD
      }
    }) // END Name(UAO1)
  } // END Device(UAOL)

} // END Scope(HDAS)
