/** @file
  ACPI DSDT table

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Acpi/AcpiTablePlatform.h>

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02, // DSDT revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
  "INTEL",   // OEM ID (6 byte string)
  "PLATEPRP",// OEM table ID  (8 byte string)
  0x3 // OEM version of DSDT table (4 byte Integer)
)
{
  #include "GloblNvs.asl"
  #include "CommonPlatform.asl"
  #include "IioRootBridgeIcxD.asi"
  #include "PlatformPciTree.asl"
  #include "AMLUPD.asl"
  #include "DsdtPci.asl"
  #include "Pch.asl"
  #include "Platform.asl"
  #include "PlatformGpe.asi"
  #include "IioPcieEdpcNotify.asi"
  #include  "FwuWmi.asl"
}
