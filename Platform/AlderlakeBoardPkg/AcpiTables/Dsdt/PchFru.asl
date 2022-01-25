/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//
// Serial Io Block
//
#include <SerialIoAcpiDefinesAdl.h>

Scope(\_SB.PC00) {
Include ("SerialIoCommon.asl")
Include ("SerialIoI2cCommon.asl")
Include ("SerialIoI2cAdl.asl")
Include ("SerialIoSpiCommon.asl")
Include ("SerialIoSpiAdl.asl")
}
Include ("SerialIoUartCommon.asl")
Include ("SerialIoUartAdl.asl")
Include ("GpioAdl.asl")
//
// SCS UFS controllers
//
Include ("ScsUfsAdl.asl")
//
// Integrated Connectivity definition
//
//Include ("CnviCommon.asl")
//Include ("CnviAdl.asl")

//
// PCI Express definition
//
Include ("PcieAdl.asl")
Include ("IsClkAdl.asl")
Include ("SataAdl.asl")

Scope (\) {
  Include("PcrLib.asl")
}
