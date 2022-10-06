/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DMAR_H_
#define _DMAR_H_

///
/// Include standard ACPI table definitions
///
#include <IndustryStandard/Acpi30.h>
#include <Library/DmaRemap.h>
#include <Library/VTdLib.h>
#include <Acpi/AcpiTablePlatform.h>

#pragma pack(1)

#define EFI_ACPI_DMAR_OEM_TABLE_ID    0x746F6F626D696C53  ///< "Slimboot"
#define EFI_ACPI_DMAR_OEM_CREATOR_ID  0x4C544E49  ///< "INTL"
#pragma pack()

#endif
