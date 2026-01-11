/** @file
  Pep Override SSDT Table

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
  "Pepovr.aml",
  "SSDT",
  2,
  "INTEL ",
  "PepoSsdt",
  0x1000
  )
{
  ACPI_DEBUG_EXTERNAL_REFERENCE
  ADBG ("[Pep override SSDT][AcpiTableEntry]")
  Include ("PepNvs.asl")
  Include ("PepOverride.asl")
  ADBG ("[Pep override SSDT][AcpiTableExit]")
}
