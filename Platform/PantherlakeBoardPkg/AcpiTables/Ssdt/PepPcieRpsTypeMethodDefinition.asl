/** @file
  ACPI Pep Pcie RP devices type method defintion.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// CondRefOf check for the device is safe because the SSDTs are
// always initialized after DSDT. And all the devices is defined
// in DSDT.
//
#define PEP_PCIE_RP_DEVICE_TYPE(Device) \
        If (CondRefOf (ROOT_COMPLEX.Device)) { \
          Scope (ROOT_COMPLEX.Device.PXSX) { \
            Method (ISLN, 0, Serialized) { \
              If (LEqual (BCCX, 0x02)) { \
                If (LEqual (SCCX, 0x00)) { \
                  If (LEqual (PIXX, 0x00)) { \
                    ADBG ("PCIe LAN!") \
                    Return (0x01) \
                  } \
                } \
              } \
              Return (0x00) \
            } \
            Method (GRPT, 0, Serialized) { \
              ADBG (Concatenate ("Check Root RP: ", Concatenate (Concatenate (ROOT_COMPLEX_STR, "."), #Device))) \
              If (PRES ()) { \
              If (CondRefOf (PRMV)) { \
                  If (LEqual (PRMV, 1)) { \
                    Return (99) \
                  } \
                } \
                If (LOr (PAHC (), PNVM ())) { \
                  Return (1) \
                } \
                If (ISGX ()) { \
                  Return (4) \
                } \
                If (ISLN ()) { \
                  Return (2) \
                } \
                If (CondRefOf (WIST)) { \
                  If (WIST ()) { \
                    Return (3) \
                  } \
                } \
                Return (0) \
              } \
              Return (99) \
            } \
          } \
        }

#define PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL(Device) \
        External (ROOT_COMPLEX.Device, DeviceObj) \
        External (ROOT_COMPLEX.Device.PXSX, DeviceObj)

#ifndef _PCIE_RP_PXSX_OBJECTS_
#define _PCIE_RP_PXSX_OBJECTS_
External (BCCX)
External (SCCX)
External (PIXX)

External (PRMV, IntObj)

// PXSX PRES method to check if the end point device present or not.
External (PRES, MethodObj)

External (PAHC, MethodObj)
External (PNVM, MethodObj)

External (ISGX, MethodObj)

External (WIST, MethodObj)
#endif

PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP01)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP02)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP03)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP04)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP05)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP06)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP07)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP08)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP09)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP10)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP11)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP12)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP13)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP14)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP15)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP16)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP17)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP18)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP19)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP20)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP21)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP22)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP23)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP24)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP25)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP26)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP27)
PCIE_RP_OVERRIDE_REQUIRED_EXTERNAL (RP28)

//
// PCIe RootPort PEP override methods definitions.
// It would define two methods:
//  ISLN  - check if the endpoint device under specific RP
//          is PCIe Lan or not.
//  GRPT  - Return the endpoint device type.
//          0 - Others.
//          1 - Storage
//          2 - LAN
//          3 - WLAN
//          4 - GFX
//
PEP_PCIE_RP_DEVICE_TYPE (RP01)
PEP_PCIE_RP_DEVICE_TYPE (RP02)
PEP_PCIE_RP_DEVICE_TYPE (RP03)
PEP_PCIE_RP_DEVICE_TYPE (RP04)
PEP_PCIE_RP_DEVICE_TYPE (RP05)
PEP_PCIE_RP_DEVICE_TYPE (RP06)
PEP_PCIE_RP_DEVICE_TYPE (RP07)
PEP_PCIE_RP_DEVICE_TYPE (RP08)
PEP_PCIE_RP_DEVICE_TYPE (RP09)
PEP_PCIE_RP_DEVICE_TYPE (RP10)
PEP_PCIE_RP_DEVICE_TYPE (RP11)
PEP_PCIE_RP_DEVICE_TYPE (RP12)
PEP_PCIE_RP_DEVICE_TYPE (RP13)
PEP_PCIE_RP_DEVICE_TYPE (RP14)
PEP_PCIE_RP_DEVICE_TYPE (RP15)
PEP_PCIE_RP_DEVICE_TYPE (RP16)
PEP_PCIE_RP_DEVICE_TYPE (RP17)
PEP_PCIE_RP_DEVICE_TYPE (RP18)
PEP_PCIE_RP_DEVICE_TYPE (RP19)
PEP_PCIE_RP_DEVICE_TYPE (RP20)
PEP_PCIE_RP_DEVICE_TYPE (RP21)
PEP_PCIE_RP_DEVICE_TYPE (RP22)
PEP_PCIE_RP_DEVICE_TYPE (RP23)
PEP_PCIE_RP_DEVICE_TYPE (RP24)
PEP_PCIE_RP_DEVICE_TYPE (RP25)
PEP_PCIE_RP_DEVICE_TYPE (RP26)
PEP_PCIE_RP_DEVICE_TYPE (RP27)
PEP_PCIE_RP_DEVICE_TYPE (RP28)
