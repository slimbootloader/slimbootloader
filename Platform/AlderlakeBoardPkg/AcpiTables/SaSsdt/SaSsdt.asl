/** @file
  This file contains the SystemAgent SSDT Table ASL code.
  It defines a Global NVS table which exchanges datas between OS
  and SBL.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PcieRpMacroDefinition.h>
#include <AcpiDebug.h>

DefinitionBlock (
  "SaSsdt.aml",
  "SSDT",
  0x02,
  "SaSsdt",
  "SaSsdt ",
  0x3000
  )
{
  Include ("SaNvs.asl")
  Include ("Sa.asl")
}
