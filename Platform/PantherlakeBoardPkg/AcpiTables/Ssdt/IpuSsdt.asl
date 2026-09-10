/** @file
  This file contains the Intel Graphics SSDT Table ASL code.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
  "IpuSsdt.aml",
  "SSDT",
  2,
  "INTEL ",
  "IpuSsdt",
  0x1000
  )
{
External (\_SB.PC00, DeviceObj)
External (\_SB.PC00.GFX0, DeviceObj)
External (\_SB.PC00.IPU0, DeviceObj)
ACPI_DEBUG_EXTERNAL_REFERENCE
External (IPTP)

Include ("Ipu.asl")
}
