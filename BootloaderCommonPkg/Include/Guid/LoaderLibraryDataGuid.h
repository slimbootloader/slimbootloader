/** @file
  This file defines the hob structure for library data.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LOADER_LIBRARY_DATA_GUID_H__
#define __LOADER_LIBRARY_DATA_GUID_H__

extern EFI_GUID gLoaderLibraryDataGuid;

///
/// Library Data Information GUID
///
typedef struct {
  UINT8         Revision;
  UINT8         Reserved0[3];
  UINT16        Count;
  UINT16        Flags;
  VOID         *Data;
} LOADER_LIBRARY_DATA;

#endif
