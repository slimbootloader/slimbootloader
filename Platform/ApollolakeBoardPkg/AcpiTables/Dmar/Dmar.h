/** @file
  This file describes the contents of the ACPI DMA address Remapping
  Some additional ACPI values are defined in Acpi1_0.h and Acpi2_0.h.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SA_DMAR_H_
#define _SA_DMAR_H_

///
/// Include standard ACPI table definitions
///
#include <IndustryStandard/Acpi30.h>
#include <Library/DmaRemappingTable.h>

#pragma pack(1)

#define EFI_ACPI_DMAR_OEM_TABLE_ID    0x746F6F626D696C53  ///< "Slimboot"
#define EFI_ACPI_DMAR_OEM_CREATOR_ID  0x4C544E49  ///< "INTL"
#pragma pack()

#endif
