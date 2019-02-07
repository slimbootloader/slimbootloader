/** @file
  This file defines the hob structure for system tables like ACPI, SMBIOS tables.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LOADER_FSP_INFO_GUID_H__
#define __LOADER_FSP_INFO_GUID_H__

///
/// System Table Information GUID
///
extern EFI_GUID gLoaderFspInfoGuid;

typedef struct {
  UINT8          Revision;
  UINT8          VerifiedBoot:1;
  UINT8          MeasuredBoot:1;
  UINT8          Reserved0:6;
  UINT8          Reserved1[2];
  UINT32         FspsBase;
  VOID           *FspHobList;
} LOADER_FSP_INFO;

#endif
