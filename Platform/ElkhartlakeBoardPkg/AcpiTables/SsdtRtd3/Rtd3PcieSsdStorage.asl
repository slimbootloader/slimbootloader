/** @file
  ACPI RTD3 SSDT table functions for M.2 SSD support

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// This code should be included in the scope of the PCIe root port that is connected to the M.2 slot
//

  External(PAHC, MethodObj) // Checks if the connected device is AHCI drive
  External(PNVM, MethodObj) // Checks if the connected device is NVME drive

  Name(_S0W,0x04)

  //
  // PCIe D3Cold support hacks ACPI specification in order to work around bug in Windows PCIe driver.
  // For storage we want to return power resources from root port's _PR0 so that we won't get unexpected D3Cold entry.
  //
  If(LOr(PAHC(), PNVM())){
    Method(_PR0, Zero, NotSerialized, 0, PkgObj){
      Return(^^_PR0())
    }
    Method(_PR3, Zero, NotSerialized, 0, PkgObj){
      Return(^^_PR0())
    }
  }

  Method(_PS0,0,Serialized){
  }

  Method(_PS3,0,Serialized){
  }

  Device(MINI){
    Method(_STA,0,Serialized){
      If(PAHC()){
        Return(0x0F)
      }Else {
        Return(0x00)
      }
    }

    //
    // Declare MINI device as a SATA drive connected to the port 0 of the SATA controller
    //
    Name(_ADR,0x0000FFFF)
    Name(_S0W,0x04)

    Method(_PS0,0,Serialized){
    }

    Method(_PS3,0,Serialized){
    }

    //
    // PCIe D3Cold support hacks ACPI specification in order to work around bug in Windows PCIe driver.
    // For storage we want to return power resources from root port's _PR0 so that we won't get unexpected D3Cold entry.
    //
    If(PAHC()){
      Method(_PR0, Zero, NotSerialized, 0, PkgObj){
        Return(^^^_PR0())
      }
      Method(_PR3, Zero, NotSerialized, 0, PkgObj){
        Return(^^^_PR0())
      }
    }
  }

