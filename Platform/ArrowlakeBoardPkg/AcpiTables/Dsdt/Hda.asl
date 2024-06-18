/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (HIDW, MethodObj)
External (HIWC, MethodObj)

//
// Define a Memory Region that will allow access to the HDA PCI Configuration Space
//
OperationRegion (HDAR, PCI_Config, 0x00, 0x100)
Field (HDAR,WordAcc,NoLock,Preserve) {
  VDID,32       // 0x00, VID DID
}

Name (_S0W, 3) // Device can wake itself from D3 in S0

Method (_DSW, 3) {}

// NHLT Table memory descriptor, returned from _DSM
Name (NBU0, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT0, AddressRangeACPI,)
})

Method (_INI) {
  // Update resource according to NVS
  ADBG ("HDAS _INI")

  // Set NHLT base address and length
  CreateQWordField (NBU0, ^NHT0._MIN, NBA0)
  CreateQWordField (NBU0, ^NHT0._MAX, NMA0)
  CreateQWordField (NBU0, ^NHT0._LEN, NLE0)
  Store (NHLT_ADDRESS_NVS_PATH, NBA0)
  Add (NHLT_ADDRESS_NVS_PATH, Subtract (NHLT_LENGTH_NVS_PATH, 1), NMA0)
  Store (NHLT_LENGTH_NVS_PATH, NLE0)
}

Name (_DSD, Package () {
  ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  // Properties related to HDAS
  Package () {
    Package (2) {"nhlt-version","1.8-0"}
  }
})
