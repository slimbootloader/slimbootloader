/** @file
  This file defines the hob structure for system tables like ACPI, SMBIOS tables.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LOADER_FSP_INFO_GUID_H__
#define __LOADER_FSP_INFO_GUID_H__

///
/// System Table Information GUID
///
extern EFI_GUID gLoaderFspInfoGuid;

typedef struct {
  UINT8          Revision;
  UINT8          Reserved0[3];
  UINT32         FspsBase;
  VOID          *FspHobList;
} LOADER_FSP_INFO;

#endif
