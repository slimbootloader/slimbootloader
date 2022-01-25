/** @file
  ACPI Support for PCIe SSD

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  // Include PciEpSel.asl for PCIe SSD support
  // Input parameters:
  OperationRegion(PCCX,PCI_Config,0x0,16) // PCI Config Space Class Code
  Field(PCCX, ByteAcc, NoLock, Preserve) {
    DVID, 32, // Vendor&Device ID,
    Offset(9),
    PIXX, 8, // Programming Interface
    SCCX, 8, // Sub Class Code
    BCCX, 8, // Base Class Code
  }

  Method(PAHC, Zero, Serialized) // Check if PCIe AHCI Controller
  {
    If(LEqual(BCCX, 0x01)){ // Check Sub Class Code and Base Class Code
      If(LEqual(SCCX, 0x06)){
        If(LEqual(PIXX, 0x01)){
          Return(0x01)
        }
      }
    }
    Return(0x00)
  }

  Method(PNVM, Zero, Serialized) // Check if PCIe NVMe
  {
    If(LEqual(BCCX, 0x01)){ // Check Sub Class Code and Base Class Code
      If(LEqual(SCCX, 0x08)){
        If(LEqual(PIXX, 0x02)){
          Return(0x01)
        }
      }
    }
    Return(0x00)
  }

  //
  // Check if  EP(End Point) is present.
  // Arguments: (0)
  // Return: EP presence status
  //     0->EP is absent; 1->EP is present
  //
  Method(PRES, Zero, Serialized) {
    If(LEqual (DVID, 0xFFFFFFFF)) {
      Return(0)
    } Else {
      Return(1)
    }
  }

  //
  // Check if EP (End Point) is GFX.
  // Arguments: (0)
  // Return:
  //     0->EP is not Gfx; 1->EP is GFX
  //
  Method (ISGX, Zero, Serialized) // Check if PCIe GFX device
  {
    If (LEqual (BCCX, 0x03)){ // Check Base Class Code
      Return (0x01)
    }
    Return (0x00)
  }

  Method (_DSD, 0)
  {
    If (LOr (PAHC (), PNVM ())) {
      Return (
        Package () {
          ToUUID ("5025030F-842F-4AB4-A561-99A5189762D0"),
          // Enable D3 Support for NVMe Storage
          Package () {
            Package (2) {"StorageD3Enable", 1}  // 1 - Enable; 0 - Disable
          }
        }
      )
    } Else {
      Return (
        Package () {
          ToUUID ("5025030F-842F-4AB4-A561-99A5189762D0"),
          Package () {
            Package (2) {"StorageD3Enable", 0}  // 1 - Enable; 0 - Disable
          }
        }
      )
    }
  }

