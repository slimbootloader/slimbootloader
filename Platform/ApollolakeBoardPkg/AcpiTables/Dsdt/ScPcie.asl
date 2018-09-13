/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  OperationRegion(PXCS,PCI_Config,0x00,0x380)
  Field(PXCS,AnyAcc, NoLock, Preserve)
  {      
    Offset(0),
    VDID, 32,
    Offset(0x5A), // SLSTS[7:0] - Slot Status Register
    ABPX, 1,      // 0, Attention Button Pressed
    , 2,
    PDCX, 1,      // 3, Presence Detect Changed
    , 2,
    PDSX, 1,      // 6, Presence Detect State       
    , 1,
    Offset(0x60), // RSTS - Root Status Register
    , 16,
    PSPX, 1,      // 16,  PME Status
    Offset(0xA4),
    D3HT, 2,      // Power State
    Offset(0xD8), // MPC - Miscellaneous Port Configuration Register
    , 30,
    HPEX, 1,      // 30,  Hot Plug SCI Enable
    PMEX, 1,      // 31,  Power Management SCI Enable 
  }

  Field(PXCS,AnyAcc, NoLock, WriteAsZeros)
  {      
    Offset(0xDC), // SMSCS - SMI/SCI Status Register
    , 30,
    HPSX, 1,      // 30,	Hot Plug SCI Status
    PMSX, 1       // 31,	Power Management SCI Status 
  }

  Device(PXSX)
  {
    Name(_ADR, 0x00000000)

    // NOTE:  Any PCIE Hot-Plug dependency for this port is
    // specific to the CRB.  Please modify the code based on
    // your platform requirements.
	
    Name(_PRW, Package(){8,4}) //Wake bit for WiFi is 8
  }

  //
  // PCI_EXP_STS Handler for PCIE Root Port
  //
  Method(HPME,0,Serialized)
  {
  
    If(PMSX) {
      //
      // Clear the PME SCI status bit with timeout
      //
      Store(200,Local0)
      While(Local0) {
        //
        // Clear PME SCI Status
        //
        Store(1, PMSX)
        //
        // If PME SCI Status is still set, keep clearing it.
        // Otherwise, break the while loop.
        //
        If(PMSX) {
          Decrement(Local0)
        } else {
          Store(0,Local0)
        }
      }
      
      //
      // Notify PCIE Endpoint Devices
      //
      Notify(PXSX, 0x02)
    }
  }
