/**@file
  ACPI DSDT table for PSE

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00) {
  //
  // GBE TSN (PCI Mode)
  //
  Device(GTSN) {
    Name(_ADR, 0x001E0004)
    OperationRegion(TSRT,PCI_Config,0x00,0x100)
    Field(TSRT,AnyAcc,NoLock,Preserve)
    {
      DVID, 16,
      Offset(0x10),
      TADL,  32,
      TADH,  32,
    }
  } // Device(GTSN)
}
