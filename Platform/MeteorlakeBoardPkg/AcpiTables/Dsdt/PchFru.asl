/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//
// Serial Io Block
//
#include <SerialIoAcpiDefinesMtl.h>

If (LNotEqual (PCHS, PCH_S)) {
  Scope(\_SB.PC00) {
  Include ("SerialIoCommon.asl")
  Include ("SerialIoI2cCommon.asl")
  Include ("SerialIoI2cMtl.asl")
  Include ("SerialIoSpiCommon.asl")
  Include ("SerialIoI3cMtl.asl")
  Include ("SerialIoSpiMtl.asl")
  Include ("SerialIoUartCommon.asl")
  }

  Include ("SerialIoUartMtl.asl")
  Include ("MtlSocSata.asl")
  Include ("MtlSocCnvi.asl")
  //
  // Touch Host Controllers definition
  //
  Include ("ThcMtl.asl")
}


//
// SCS UFS controllers
//
Scope (\_SB_.PC00) {
   include ("ScsUfsMtl.asl")
}

Include ("IsClkMtl.asl")
Include ("GpioMtl.asl")
Include ("MtlSocP2Sb.asl")

Scope (\) {
  Include("PcrLib.asl")
}
