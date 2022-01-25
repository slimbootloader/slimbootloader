/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/SerialIoRegs.h>

#define SERIAL_IO_BAR_RESET 1
#define SERIAL_IO_D3        1

//
// Include PchSerialIo ACPI macros
//

Scope(\_SB.PC00) {


  //
  //  Serial IO D3
  //  Common code for D3 entry for all SerialIo devices including PCI mode
  //
  //  @param[in]  Arg0      PCI CFG address
  //  @param[in]  Arg1      Boolean - Place Device in D3 (SERIAL_IO_D3)
  //  @param[in]  Arg2      Boolean - Reset BAR0 (SERIAL_IO_BAR_RESET) or leave BAR0 unchanged (0)
  //                                  depends on Arg1 equal to SERIAL_IO_D3 - device placed in D3
  //
  Method (SOD3, 3, Serialized) {
    OperationRegion(ICB1, SystemMemory, Add(\_SB.PC00.GPCB(), Arg0), Add (R_SERIAL_IO_CFG_PME_CTRL_STS, 4))
    If (Arg1) { // SERIAL_IO_D3
      Field(ICB1, ByteAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_PME_CTRL_STS),
        PMEC, 8 //Note: SerialIO Does Not Support PME
      }
      Store (0x3, PMEC) // Set BIT[1:0] = 11b - Power State D3
      Or (PMEC, 0, PMEC) // perform a read to avoid ordering and noncoherency problems
    }
    If (LAnd(Arg1, Arg2)) { //SERIAL_IO_BAR_RESET only if device is placed in D3
      Field(ICB1, AnyAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_BAR0_LOW),
        BAR0, 64
      }
      Store(0, BAR0)
    }
  }
} //End Scope(\_SB.PC00)

