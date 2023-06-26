/** @file
  VMD Logical Volume RTD3 support in ACPI

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Include this file in VMD scope to enable VMD Logical volume support
//

//
// NOTE: For platform implementers.
// VR3D method checks if RTD3 flow is allowed for given device
// RTD3 flow is enabled if it hasn't been globally locked or device isn't part of the RAID volume
// VMD driver will globally lock RTD3 flows in following scenarios:
// 1. Volume creation
// 2. System shutdown
// 3. System transition to S3/S4
//
// To enable Logical volume functionality platform code should check in _ON/_OFF methods of the
// power resource associated with the device that can be a part of the VMD logical volume if RTD3 flow has not been blocked.
// If RTD3 flow has been blocked platform code should not execute D3Cold entry logic.
//
// To allow VMD volume to manage PCIe and SATA port power implement VPON/VPOF/VSON/VSOF methods.
// VPON - Method to turn PCIe port on.
// VPOF - Method to turn PCIe port off.
// VSON - Method to turn SATA port on.
// VSOF - Method to turn SATA port off.
//

Name (VLVN, 0) // VMD Logical Volume Number
Name (VV0D, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}) // List of ACPI address for PCIe device or ACPI address for SATA device for Volume 0
Name (VV1D, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}) // List of ACPI address for PCIe device or ACPI address for SATA device for Volume 1
Name (VV2D, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}) // List of ACPI address for PCIe device or ACPI address for SATA device for Volume 2
Name (VV3D, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}) // List of ACPI address for PCIe device or ACPI address for SATA device for Volume 3
Name (VD3A, 0) //VMD Global RTD3 Allowed Flag
Name (VV0S, 0) // Volume 0 Size
Name (VV1S, 0) // Volume 1 Size
Name (VV2S, 0) // Volume 2 Size
Name (VV3S, 0) // Volume 3 Size

External (CBAR, MethodObj)
External (\_SB.PC00.VMD0.VLON, MethodObj)
External (\_SB.PC00.VMD0.VLOF, MethodObj)
External (D3CV, MethodObj)

//
// Store the volume package sent by the RST driver
//
// Arg0: Reference to volume package passed from the RST driver
// Arg1: Reference to volume package to which we should store the data
// Arg2: Reference to volume size object
//
Method (SVRP, 3) {
  Subtract (SizeOf (Arg0), 1, Local0)

  If (LGreater (Local0, 32)) {
    Return ()
  }

  Store (Local0, Arg2)
  Store (0, Local1) // Iterator
  While (LLess (Local1, Local0)) {
    Store (DeRefOf (Index (Arg0, Add (Local1, 1))), Index (Arg1, Local1))
    Increment (Local1)
  }
}

//
// _DSM- _DSM method for VMD Logical Volume
// Arg0 - UUID: 9CD9CDDD-8845-4AFD-8392-31C4EB87CABD
// Arg1 - Revision ID: 0x01 (Integer)
// Arg2 - Function Index: 0x01 (Integer)
// Arg3 - Rtd3AcpiVmdDevList : ACPI address for PCIe device or ACPI address for SATA device is stored
//
Method (_DSM, 4, Serialized,,, {BuffObj, IntObj, IntObj, PkgObj}) {
  If (LEqual (Arg0, ToUUID ("9CD9CDDD-8845-4AFD-8392-31C4EB87CABD"))) {
    Switch (ToInteger (Arg2)) {
      Case (0) {
        // Standard query - A bitmask of functions supported
        // Supports function 0-5
        Name (RBUF, Buffer (1) {0x00})
        CreateByteField (RBUF, 0, SFUN)
        Store (0x3F, SFUN)
        Return (RBUF)
      }
      Case (1) {
        // Assign ports to this VMD RAID volume - _SAVE_RTD3_PACKAGE
        // Arg3 - Rtd3AcpiVmdDevList : Volume Number and ACPI address for PCIe device or ACPI address for SATA device is stored
        Store (ToInteger (DerefOf (Index (Arg3,0))), VLVN)  // Volume Number (0-3)

        If (LEqual (VLVN,0)) { // if Volume number is  0 store address package to VV0D
          SVRP (Arg3, VV0D, RefOf (VV0S))
          Return (0)
        }
        ElseIf (LEqual (VLVN,1)) {
          SVRP (Arg3, VV1D, RefOf (VV1S))
          Return (0)
        }
        ElseIf (LEqual (VLVN,2)) {
          SVRP (Arg3, VV2D, RefOf (VV2S))
          Return (0)
        }
        ElseIf (LEqual (VLVN,3)) {
          SVRP (Arg3, VV3D, RefOf (VV3S))
          Return (0)
        }
        Else { Return (1) }
      }
      Case (2) {
      // VMD Driver access to turn ON ports.
      // Arg3 - List of disk adress that are part of logical volume
        // List of ACPI address for PCIe device or ACPI address for SATA device for Logical Volume
        Name (VVON, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0})
        Store (SizeOf (Arg3), Local0)
        Store (0, Local1)
        If (LLessEqual (Local0, 32)) { // Logical Volume can contain maximum of 32 disks
          While (LLess (Local1, Local0)) {
            Store (ToInteger (DerefOf (Index (Arg3, Local1))), Index (VVON, Local1))
            Increment (Local1)
          }
          \_SB.PC00.VMD0.VLON (Local0, VVON);
        }
        Return (0)
      }
      Case (3) {
      // VMD Driver access to turn OFF a port.
      // Arg3 - List of disk adress that are part of logical volume
        Name (VVOF, Package (32) {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}) // List of ACPI address for PCIe device or ACPI address for SATA device for Logical Volume
        Store (SizeOf (Arg3), Local0)
        Store (0, Local1)
        If (LLessEqual (Local0, 32)) { // Logical Volume can contain maximum of 32 disks
          While (LLess (Local1, Local0)) {
            Store (ToInteger (DerefOf (Index (Arg3, Local1))), Index (VVOF, Local1))
            Increment (Local1)
          }
          \_SB.PC00.VMD0.VLOF (Local0, VVOF);
        }
        Return (0)
      }
      Case (4) {
      // RST Driver read VMD Volume mask - Unsupported in the driver
        Return (0)
      }
      Case (5) {
      // Global Lock to allow/disallow RTD3 for VMD Raid Volumes
      // Arg3 - Lock bit to enable or disable RTD3 flow for VMD logical volume
        Store (DerefOf (Index (Arg3,0)), VD3A)
        Return (0)
      }
      Default {
        Return (2)
      }
    }
  }
  Return (0)
}

//
// Is VMD RAID member method
// Arg0 - ACPI address for PCIe device or ACPI address for SATA device
//
Method (IVRM, 1) {
  // Check in volume 0
  Store (0, Local0) // use this as the index value
  While (LGreater (VV0S, Local0)) {
    If (LEqual (Arg0,ToInteger (DerefOf (Index (VV0D, Local0))))) { Return (1) }
    Increment (Local0)
  }

  // Check in volume 1
  Store (0, Local0) // use this as the index value
  While (LGreater (VV1S, Local0)) {
    If (LEqual (Arg0,ToInteger (DerefOf (Index (VV1D, Local0))))) { Return (1) }
    Increment (Local0)
  }

  // Check in volume 2
  Store (0, Local0) // use this as the index value
  While (LGreater (VV2S, Local0)) {
    If (LEqual (Arg0,ToInteger (DerefOf (Index (VV2D, Local0))))) { Return (1) }
    Increment (Local0)
  }

  // Check in volume 3
  Store (0, Local0) // use this as the index value
  While (LGreater (VV3S, Local0)) {
    If (LEqual (Arg0,ToInteger (DerefOf (Index (VV3D, Local0))))) { Return (1) }
    Increment (Local0)
  }

  Return (0)
}

// Is D3Cold blocked on the port.
// Arg0 - PCIe / SATA port address
Method (VD3C, 1) {
  Return (LAnd (VD3A, IVRM (Arg0)))
}
