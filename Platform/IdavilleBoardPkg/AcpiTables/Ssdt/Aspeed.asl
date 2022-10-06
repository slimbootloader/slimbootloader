/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

DefinitionBlock (
    "AspeedSsdt.aml",
    "SSDT",
    0x02,         // SSDT revision.
                  // A Revision field value greater than or equal to 2 signifies that integers
                  // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL",      // OEM ID (6 byte string)
    "ASPEEDTL",   // OEM table ID  (8 byte string)
    0x0           // OEM version of DSDT table (4 byte Integer)
    )
{

  External(\_SB.PC00.LPC0, DeviceObj)

  Scope (\_SB.PC00.LPC0)
  {
    #include "Aspd.asi"
  }

}
