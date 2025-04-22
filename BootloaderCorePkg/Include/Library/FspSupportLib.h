/** @file

  Copyright (c) 2016-2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FSP_SUPPORT_LIB_H__
#define __FSP_SUPPORT_LIB_H__

typedef VOID \
(EFIAPI *MEM_RES_HOB_CALLBACK) (EFI_HOB_RESOURCE_DESCRIPTOR *ResourceDescriptor, VOID *Param);

/**
  This function retrieves FSP Non-volatile Storage HOB buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the NVS data buffer length.  If the FSP NVS
                       HOB is located, the length will be updated.
  @retval NULL         Failed to find the NVS HOB.
  @retval others       FSP NVS data buffer pointer.

**/
VOID *
EFIAPI
GetFspNvsDataBuffer (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
  );
/**
  This function retrieves FSP Non-volatile Storage HOB 2 buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the NVS data buffer length.  If the FSP NVS
                       HOB is located, the length will be updated.
  @retval NULL         Failed to find the NVS HOB.
  @retval others       FSP NVS data buffer pointer.

**/
VOID *
EFIAPI
GetFspNvsData2Buffer (
  CONST VOID       *HobListPtr,
  UINT32           *Length
  );

/**
  This function retrieves a special reserved memory region.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the GUID HOB data buffer length.  If the GUID HOB is
                       located, the length will be updated.
  @param  OwnerGuid    A pointer to the owner guild.
  @retval              Reserved region start address.  0 if this region does not exist.

**/
UINT64
EFIAPI
GetFspReservedMemoryFromGuid (
  CONST VOID     *HobListPtr,
  UINT64         *Length,
  EFI_GUID       *OwnerGuid
  );

/**
  This function retrieves a top of low and high memory address.

  @param  HobListPtr    A HOB list pointer.
  @param  TopOfHighMem  A pointer to receive the top of high memory.

  @retval              Top of low memory.

**/
UINT32
EFIAPI
GetSystemTopOfMemeory (
  CONST VOID     *HobListPtr,
  UINT64         *TopOfHighMem  OPTIONAL
  );

/**
  This function traverses each memory resource hob type and calls the handler.

  @param  HobListPtr         A HOB list pointer.
  @param  MemResHobCallback  A function pointer to the callback handler
  @param  Param              A pointer to the parameter which will be passed into handler.

**/
VOID
EFIAPI
TraverseMemoryResourceHob (
  IN  CONST VOID            *HobListPtr,
  IN  MEM_RES_HOB_CALLBACK   MemResHobCallback,
  IN  VOID                  *Param
  );

/**
  Dump FSP memory resource

  @param  HobListPtr         A HOB list pointer.

**/
VOID
EFIAPI
DumpFspResourceHob (
  IN  CONST VOID            *HobListPtr
  );

#endif
