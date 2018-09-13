/** @file
  Payload specific memory allocation library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PAYLOAD_MEMORY_ALLOCATION_LIB_H_
#define _PAYLOAD_MEMORY_ALLOCATION_LIB_H_

#include <Library/MemoryAllocationLib.h>

/**
  Add system memory resource for allocation.

  Add system range into the memory resource pool

  @param  Base            The base of the normal memory range.
  @param  Pages           The number of pages for the normal memory range.
  @param  RsvdBase        The base of the reserved memory range.
  @param  RsvdPages       The number of pages for the reserved memory range.
**/
VOID
EFIAPI
AddMemoryResourceRange (
  IN  UINT64   Base,
  IN  UINT32   Pages,
  IN  UINT64   RsvdBase,
  IN  UINT32   RsvdPages
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
