/** @file
  ACPI uPEP Support for PCIe SSD

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  // Include PepPcieSsd.asl for PCIe SSD PEP support
  // Input parameters:


  OperationRegion(PCCX,PCI_Config,0x09,4) // PCI Config Space Class Code
  Field(PCCX,ByteAcc, NoLock, Preserve) {
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

