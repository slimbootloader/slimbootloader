/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Serial Io Block
//
#include <SerialIoAcpiDefinesMtl.h>
//
//
If (LNotEqual (PCHS, PCH_S)) {
  Scope (\_SB.PC00) {
    Include ("SerialIoCommon.asl")
    Include ("SerialIoI2cCommon.asl")
    Include ("SerialIoI2cMtl.asl")
    Include ("SerialIoI3cMtl.asl")
    Include ("SerialIoSpiCommon.asl")
    Include ("SerialIoSpiMtl.asl")
    Include ("SerialIoUartCommon.asl")
  }
  Include ("SerialIoUartCommonHidden.asl")
  Include ("SerialIoUartMtl.asl")
  Include ("MtlSocCnvi.asl")
  Include ("MtlSocSata.asl")

  //
  // Touch Host Controllers definition
  //
  Include ("ThcMtl.asl")
}

Scope (\_SB_.PC00) {
   include ("ScsUfsMtl.asl")
}

Include ("IsClkMtl.asl")

Include ("GpioMtl.asl")

Include ("MtlSocP2Sb.asl")

//
// Legacy support which redirects to MTL SoC P2SB
//
Scope (\) {
  Include ("PcrLib.asl")
}
