/** @file
  ACPI uPEP Support for PCIe SSD

  Copyright (c) 2013 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  // Include PepPcieSsd.asl for PCIe SSD PEP support
  // Input parameters:

  Method(_DEP){
    If(LOr(PAHC(), PNVM())){ // Check if PCIe AHCI controller or NVMe
      If(LAnd(LEqual(S0ID, 1), LEqual(OSYS, 2012))) {  // If OS is Win8 and S0ix enabled, associate PCIe storage PEP with SATA PEP due to OS limitation
        If(LEqual(And(PEPC, BIT0), 1)) {  // BIT0 set means Adapter D0/F1 or D3
          Return(Package() {\_SB.PEPD})
        }
      }
      If(LAnd(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))) {  // If OS is Win10 onward and S0ix enabled, PCIe storage PEP would be reported
        Return(Package() {\_SB.PEPD})
      }
    }
    Return(Package(){})
  }

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

