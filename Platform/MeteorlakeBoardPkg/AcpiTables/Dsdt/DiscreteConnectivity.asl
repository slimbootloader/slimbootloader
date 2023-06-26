/** @file
  Intel ACPI Sample Code for Discrete Connectivity module

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
OperationRegion (RPXX, SystemMemory, GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), 0x30)
Field (RPXX, AnyAcc, NoLock, Preserve)
{
  Offset (0),     // Vendor-Device ID
  VDID, 32,
  Offset (0x2C),  // SVID
  SVID, 16,
}

OperationRegion (FLDR, SystemMemory, Add (GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), 0x44), 0x06)
Field (FLDR, ByteAcc, NoLock, Preserve)
{
  DCAP, 32,
  DCTR, 16,
}

OperationRegion (VSEC, SystemMemory, Add (GMIO (PCIE_ROOT_PORT.PBNU, PCIE_ROOT_PORT._ADR, PCIE_ROOT_PORT.PXSX._ADR), WVHO), 0x10)
Field (VSEC, AnyAcc, NoLock, Preserve)
{
  Offset (0xC),     // VSEC Control Reg
      , 1,
  BTIE, 1,         // BT Interface Enable bit
      , 1,
      , 1,
      , 1,
      , 1,
      , 1,
      , 1,
  PDRC, 1,         // PLDR Control bit
}

// WIST (WiFi Device Presence Status)
//
// Check if a WiFi Device is present on the RootPort.
//
// Arguments: (0)
//   None
// Return Value:
//    0 - if a device is not present.
//    1 - if a device is present.
//
Method (WIST,0,Serialized)
{
  // check Vendor-Device ID for supported devices
  If (CondRefOf (VDID)) {
    Switch (ToInteger (VDID)) {
      Case (0x25268086) {Return (1)} // Intel Wireless-AC 9260 (Thunder Peak 2)
      Case (0x271B8086) {Return (1)} // Intel Wireless-AC 9260 (Thunder Peak 1)
      Case (0x27238086) {Return (1)} // Cyclone Peak
      Case (0x27258086) {Return (1)} // Typhoon Peak
      Case (0x272B8086) {Return (1)} // Gale Peak (GaP)
      Default {
        If (LEqual (SVDC, 1)) {      // Skip VDID check
          Return (1)
        }
        Return (0)                 // no supported device
      }
    }
  }Else {
    Return (0)
  }
}

// WWST (WWAN Device Presence Status)
//
// Check if a WWAN Device is present on the RootPort.
//
// Arguments: (0)
//   None
// Return Value:
//    0 - if a device is not present.
//    1 - if a device is present.
//
Method (WWST,0,Serialized)
{
  // check Vendor-Device ID for supported devices
  If (CondRefOf (VDID)) {
    Switch (ToInteger (VDID)) {
      Case (0x73608086) {Return (1)} // XMM7360
      Case (0x75608086) {Return (1)} // XMM7560
      Case (0x4D7514C3) {Return (1)} // M80
      Case (0x080014C3) {Return (1)} // MaxSpring
      Default {Return (0)}           // no supported device
    }
  }Else {
    Return (0)
  }
}

//
// Load Wifi/BT/WiGig tables only the Vendor ID, Device ID matches
//
If (WIST ())
{
  Include ("Wifi.asl")
}

//
// Load WWAN tables only the Vendor ID, Device ID matches
//
If (LAnd (LNotEqual (WWEN, 0), LEqual (WWRP, SLOT)))
{
  Include ("Wwan.asl")
}

//
// Load Platform-level device reset tables only the Vendor ID, Device ID matches
//
If (LOr (WIST (), LAnd (LNotEqual (WWEN, 0), LEqual (WWRP, SLOT))))
{
  Include ("DiscreteConnectivityReset.asl")
  Include ("DiscreteConnectivityDsm.asl")
}