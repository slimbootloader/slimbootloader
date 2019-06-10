/** @file
  Struct and GUID definitions for CpuInitDataHob.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
