/** @file
  Struct and GUID definitions for CpuInitDataHob.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 Revision;
  UINT32                 Rsvd[2];
  EFI_PHYSICAL_ADDRESS   CpuGnvsPointer;    ///< CPU_NVS_AREA Pointer.
  EFI_PHYSICAL_ADDRESS   FvidTable;         ///< FVID Table.
} CPU_INIT_DATA_HOB;

#endif
