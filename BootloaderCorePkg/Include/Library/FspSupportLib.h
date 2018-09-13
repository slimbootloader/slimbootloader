/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FSP_SUPPORT_LIB_H__
#define __FSP_SUPPORT_LIB_H__

typedef VOID \
(*MEM_RES_HOB_CALLBACK) (EFI_HOB_RESOURCE_DESCRIPTOR *ResourceDescriptor, VOID *Param);

/**
  This function retrieves FSP Non-volatile Storage HOB buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the NVS data buffer length.  If the FSP NVS
                       HOB is located, the length will be updated.
  @retval NULL         Failed to find the NVS HOB.
  @retval others       FSP NVS data buffer pointer.

**/
VOID *
GetFspNvsDataBuffer (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
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
GetFspReservedMemoryFromGuid (
  CONST VOID     *HobListPtr,
  UINT64         *Length,
  EFI_GUID       *OwnerGuid
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

#endif
