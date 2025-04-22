/** @file
  Payload specific memory allocation library.

  Copyright (c) 2017- 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PAYLOAD_MEMORY_ALLOCATION_LIB_H_
#define _PAYLOAD_MEMORY_ALLOCATION_LIB_H_

#include <Library/BlMemoryAllocationLib.h>

/**
  Add system memory resource for allocation.

  Add system range into the memory resource pool

  @param[in]  MemoryRanges    Memory range array structure.
  @param[in]  Count           Memory range entry count.

**/
VOID
EFIAPI
AddMemoryResourceRange (
  IN  EFI_MEMORY_RANGE_ENTRY  *MemoryRanges,
  IN  UINT32                   Count
  );

/**
  Retrieve the memory resource inforamtion for the specified type.

  @param[in]   Type          The type of memory.
  @param[out]  StartAddr     The pointer to receive the start address of the memory type.
  @param[out]  FreeAddr      The pointer to receive the last free address of the memory type.
  @param[out]  EndAddr       The pointer to receive the end address of the memory type.

  @retval     EFI_INVALID_PARAMETER  Invalid parameter for Type.
              EFI_SUCCESS            Memory resoruce information is returned successfully.
**/
EFI_STATUS
EFIAPI
GetMemoryResourceInfo (
  IN   EFI_MEMORY_TYPE   Type,
  OUT  UINT64           *StartAddr  OPTIONAL,
  OUT  UINT64           *FreeAddr   OPTIONAL,
  OUT  UINT64           *EndAddr    OPTIONAL
  );

#endif
