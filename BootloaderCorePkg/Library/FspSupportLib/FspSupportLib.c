/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FspSupportLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Guid/GuidHobFspEas.h>

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
  CONST VOID       *HobListPtr,
  UINT32           *Length
  )
{
  return GetGuidHobData (HobListPtr, Length, &gFspNonVolatileStorageHobGuid);
}

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
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobListPtr;

  /*
   * Collect memory ranges
   */
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) {
        if (CompareGuid (&Hob.ResourceDescriptor->Owner, OwnerGuid)) {
          if (Length != 0) {
            *Length = (UINT32) (Hob.ResourceDescriptor->ResourceLength);
          }
          return (UINT64) (Hob.ResourceDescriptor->PhysicalStart);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return 0;
}

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
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobListPtr;

  /*
   * Collect memory ranges
   */
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) ||
          (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)) {
        MemResHobCallback (Hob.ResourceDescriptor, Param);
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
}
