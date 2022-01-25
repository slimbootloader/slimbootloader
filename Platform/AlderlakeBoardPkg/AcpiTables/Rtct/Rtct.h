/** @file
  Header file for RTCT.

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _RTCT_H_
#define _RTCT_H_


#include <IndustryStandard/Acpi50.h>

// ACPI Definations for RTCT Table
#define EFI_ACPI_RTCT_SIGNATURE           SIGNATURE_32('R', 'T', 'C', 'T')  // 'RTCT'
#define EFI_ACPI_RTCT_TABLE_REVISION      0x1
#define EFI_ACPI_RTCT_OEM_ID              "INTEL "  // OEMID 6 bytes long
#define EFI_ACPI_RTCT_OEM_TABLE_ID        SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_RTCT_OEM_REVISION        0x00000005
#define EFI_ACPI_RTCT_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_RTCT_CREATOR_REVISION    0x0100000D

//
// Ensure proper structure formats
//
#pragma pack (1)

//
// RTCT Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER      Header;
  UINT64                           Reserved;
} EFI_ACPI_TCC_RTCT_TABLE;

#pragma pack ()

#endif //_RTCT_H_
