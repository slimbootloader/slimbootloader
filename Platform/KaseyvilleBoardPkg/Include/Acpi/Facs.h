/** @file
  This file describes the contents of the ACPI Firmware ACPI Control
  Structure (FACS).  Some additional ACPI values are defined in Acpi1_0.h and
  Acpi2_0.h
  All changes to the FACS contents should be done in this file.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FACS_H
#define _FACS_H

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>

//
// FACS Definitions
//
#define EFI_ACPI_FIRMWARE_WAKING_VECTOR 0x00000000
#define EFI_ACPI_GLOBAL_LOCK            0x00000000

//
// Firmware Control Structure Feature Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_FIRMWARE_CONTROL_STRUCTURE_FLAGS 0x00000000
#define EFI_ACPI_X_FIRMWARE_WAKING_VECTOR         0x0000000000000000
#define EFI_ACPI_OSPM_FLAGS                       0x00000000

#endif
