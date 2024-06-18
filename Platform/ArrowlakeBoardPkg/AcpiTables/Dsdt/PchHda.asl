/** @file

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

// NHLT Table memory descriptor, returned from _DSM
Name (NBU1, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT1, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU2, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT2, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU3, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT3, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU4, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT4, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU5, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT5, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU6, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT6, AddressRangeACPI,)
})

// NHLT Table memory descriptor, returned from _DSM
Name (NBU7, ResourceTemplate () {
  // NHLT table address (_MIN = NHLT 64bit pointer, _MAX = _MIN + _LEN - 1) and length (_LEN)
  QWordMemory (ResourceConsumer, , MinNotFixed, MaxNotFixed, NonCacheable, ReadOnly,
              0x1,                // AddressGranularity
              0x0000000000000000, // AddressMinimum _MIN
              0x0000000000000000, // AddressMaximum _MAX
              0x0,
              0x0,                // RangeLength _LEN
              , , NHT7, AddressRangeACPI,)
})

Method (_INI) {
  // Update resource according to NVS

  // Set NHLT base address and length
  CreateQWordField (NBU0, ^NHT0._MIN, NBA0)
  CreateQWordField (NBU0, ^NHT0._MAX, NMA0)
  CreateQWordField (NBU0, ^NHT0._LEN, NLE0)
  Store (NHAA, NBA0)
  Add (NHAA, Subtract (NHLL, 1), NMA0)
  Store (NHLL, NLE0)

  // Set NHLT base address and length
  CreateQWordField (NBU1, ^NHT1._MIN, NBA1)
  CreateQWordField (NBU1, ^NHT1._MAX, NMA1)
  CreateQWordField (NBU1, ^NHT1._LEN, NLE1)
  Store (NHA1, NBA1)
  Add (NHA1, Subtract (NHL1, 1), NMA1)
  Store (NHL1, NLE1)

  // Set NHLT base address and length
  CreateQWordField (NBU2, ^NHT2._MIN, NBA2)
  CreateQWordField (NBU2, ^NHT2._MAX, NMA2)
  CreateQWordField (NBU2, ^NHT2._LEN, NLE2)
  Store (NHA2, NBA2)
  Add (NHA2, Subtract (NHL2, 1), NMA2)
  Store (NHL2, NLE2)

  // Set NHLT base address and length
  CreateQWordField (NBU3, ^NHT3._MIN, NBA3)
  CreateQWordField (NBU3, ^NHT3._MAX, NMA3)
  CreateQWordField (NBU3, ^NHT3._LEN, NLE3)
  Store (NHA3, NBA3)
  Add (NHA3, Subtract (NHL3, 1), NMA3)
  Store (NHL3, NLE3)

  // Set NHLT base address and length
  CreateQWordField (NBU4, ^NHT4._MIN, NBA4)
  CreateQWordField (NBU4, ^NHT4._MAX, NMA4)
  CreateQWordField (NBU4, ^NHT4._LEN, NLE4)
  Store (NHA4, NBA4)
  Add (NHA4, Subtract (NHL4, 1), NMA4)
  Store (NHL4, NLE4)

  // Set NHLT base address and length
  CreateQWordField (NBU5, ^NHT5._MIN, NBA5)
  CreateQWordField (NBU5, ^NHT5._MAX, NMA5)
  CreateQWordField (NBU5, ^NHT5._LEN, NLE5)
  Store (NHA5, NBA5)
  Add (NHA5, Subtract (NHL5, 1), NMA5)
  Store (NHL5, NLE5)

  // Set NHLT base address and length
  CreateQWordField (NBU6, ^NHT6._MIN, NBA6)
  CreateQWordField (NBU6, ^NHT6._MAX, NMA6)
  CreateQWordField (NBU6, ^NHT6._LEN, NLE6)
  Store (NHA6, NBA6)
  Add (NHA6, Subtract (NHL6, 1), NMA6)
  Store (NHL6, NLE6)

  // Set NHLT base address and length
  CreateQWordField (NBU7, ^NHT7._MIN, NBA7)
  CreateQWordField (NBU7, ^NHT7._MAX, NMA7)
  CreateQWordField (NBU7, ^NHT7._LEN, NLE7)
  Store (NHA7, NBA7)
  Add (NHA7, Subtract (NHL7, 1), NMA7)
  Store (NHL7, NLE7)
}

Name (_DSD, Package () {
  ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
  // Properties related to HDAS
  Package () {
    Package (2) {"nhlt-version","1.8-0"}
  }
})
