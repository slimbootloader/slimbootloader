/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//
// Serial Io Block
//
#include <SerialIoAcpiDefinesPtl.h>

Scope (\_SB.PC00) {
  Include ("SerialIoCommon.asl")
  Include ("SerialIoI2cCommon.asl")
  Include ("SerialIoI2cPtlPcd.asl")
  Include ("SerialIoI3cPtlPcd.asl")
  Include ("SerialIoSpiCommon.asl")
  Include ("SerialIoSpiPtlPcd.asl")
  Include ("SerialIoUartCommon.asl")
}
Include ("SerialIoUartCommonHidden.asl")
Include ("SerialIoUartPtlPcd.asl")
Include ("PtlPcdCnvi.asl")
Include ("IsClkPtlPcd.asl")
Include ("GpioPtlPcd.asl")

Include ("PtlPcdP2Sb.asl")

//
// Legacy support which redirects to PTL SoC P2SB
//
Scope (\) {
  Include ("PcrLib.asl")
}
