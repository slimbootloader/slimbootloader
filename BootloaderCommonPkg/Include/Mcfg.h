/** @file

Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MCFG_H_
#define _MCFG_H_

#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>

//
// MCFG Revision (defined in spec)
//
#define EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_REVISION  0x01

//
// MCFG Definitions, see specification for details.
//
#define EFI_ACPI_OEM_MCFG_REVISION  0x00000001


#pragma pack(1)

//
// MCFG Table definition.  The table must be defined in a platform
// specific manner.
//
//
// Ensure proper structure formats
//

typedef struct {
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER        Header;

  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  Segment;

} EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE;

#pragma pack()


#endif /* _MCFG_H_ */
