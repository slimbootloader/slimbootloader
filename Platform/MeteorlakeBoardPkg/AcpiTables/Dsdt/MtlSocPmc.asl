/**@file
  Intel ACPI Reference Code for MTL SOC Power Management Control

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/TcoRegs.h>

Scope (\) {

  //
  // Define PCH ACPIBASE I/O as an ACPI operating region.  The base address
  // can be found in Device 31, Function 2, Offset 40h.
  //
  OperationRegion (PMIO, SystemIo, PMBS, 0x80)
  Field (PMIO, ByteAcc, NoLock, Preserve) {
          ,  8,
      PBSS,  1,       // Power Button Status
          ,  15,
      PBEN,  1,       // Power Button Enable
    Offset (R_ACPI_IO_GPE_CTRL),            // 0x40, General Purpose Event Control
          ,  17,
      GPEC,  1,       // Software GPE Control
    Offset (R_ACPI_IO_GPE0_STS_127_96),     // 0x6C, General Purpose Event 0 Status [127:96]
    Offset (R_ACPI_IO_GPE0_EN_127_96),      // 0x7C, General Purpose Event 0 Enable [127:96]
          ,  18,
      WAEN,  1,       // WADT EN
  }

  //
  // Is MODPHY power gating supported
  // Checks if MODPHY power gating is supported on given SKU
  //
  // @retval 0 MODPHY power gating is not supported
  // @retval 1 MODPHY power gating is supported
  //
  Method (IMPS) {
    If (LEqual (PCHS, PCH_S)) {
      Return (0)
    } Else {
      Return (1)
    }
  }

  Include ("Pmc.asl")
}
