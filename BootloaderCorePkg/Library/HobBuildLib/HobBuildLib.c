/** @file
  This module provide Hand-Off Block manupulation.

Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>

/**
  Add a new HOB to the HOB List.

  @param PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param Type             Type of the new HOB.
  @param Length           Length of the new HOB to allocate.
  @param Hob              Pointer to the new HOB.

  @return  EFI_SUCCESS           Success to create hob.
  @retval  EFI_INVALID_PARAMETER if Hob is NULL
  @retval  EFI_NOT_AVAILABLE_YET if HobList is still not available.
  @retval  EFI_OUT_OF_RESOURCES  if there is no more memory to grow the Hoblist.

**/
EFI_STATUS
EFIAPI
PeiCreateHob (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT16            Type,
  IN UINT16            Length,
  IN OUT VOID          **Hob
  )
{
  EFI_HOB_HANDOFF_INFO_TABLE           *HandOffHob;
  EFI_HOB_GENERIC_HEADER               *HobEnd;
  EFI_PHYSICAL_ADDRESS                 FreeMemory;

  //
  // Get Hob list pointer
  //
  *Hob = GetHobList();

  if ( *Hob == NULL ) {
    return EFI_NOT_FOUND;
  }

  HandOffHob = *Hob;

  //
  // Check Length to avoid data overflow.
  //
  if (0x10000 - Length <= 0x7) {
    return EFI_INVALID_PARAMETER;
  }
  Length     = (UINT16) ((Length + 0x7) & (~0x7));

  FreeMemory = HandOffHob->EfiFreeMemoryTop -
               HandOffHob->EfiFreeMemoryBottom;

  if (FreeMemory < Length) {
    DEBUG ((DEBUG_ERROR, "PeiCreateHob fail: Length - 0x%08x\n", (UINTN)Length));
    DEBUG ((DEBUG_ERROR, "  FreeMemoryTop    - 0x%08x\n", (UINTN)HandOffHob->EfiFreeMemoryTop));
    DEBUG ((DEBUG_ERROR, "  FreeMemoryBottom - 0x%08x\n", (UINTN)HandOffHob->EfiFreeMemoryBottom));
    return EFI_OUT_OF_RESOURCES;
  }

  *Hob = (VOID *) (UINTN) HandOffHob->EfiEndOfHobList;
  ((EFI_HOB_GENERIC_HEADER *) *Hob)->HobType   = Type;
  ((EFI_HOB_GENERIC_HEADER *) *Hob)->HobLength = Length;
  ((EFI_HOB_GENERIC_HEADER *) *Hob)->Reserved  = 0;

  HobEnd = (EFI_HOB_GENERIC_HEADER *) ((UINTN) * Hob + Length);
  HandOffHob->EfiEndOfHobList = (EFI_PHYSICAL_ADDRESS) (UINTN) HobEnd;

  HobEnd->HobType   = EFI_HOB_TYPE_END_OF_HOB_LIST;
  HobEnd->HobLength = (UINT16) sizeof (EFI_HOB_GENERIC_HEADER);
  HobEnd->Reserved  = 0;
  HobEnd++;
  HandOffHob->EfiFreeMemoryBottom = (EFI_PHYSICAL_ADDRESS) (UINTN) HobEnd;

  return EFI_SUCCESS;
}

/**
  Adds a new HOB to the HOB List.

  This internal function enables PEIMs to create various types of HOBs.

  @param  Type          Type of the new HOB.
  @param  Length        Length of the new HOB to allocate.

  @retval  NULL         The HOB could not be allocated.
  @retval  others       The address of new HOB.

**/
VOID *
EFIAPI
InternalPeiCreateHob (
  IN UINT16                      Type,
  IN UINT16                      Length
  )
{
  EFI_STATUS        Status;
  VOID              *Hob;

  Status = PeiCreateHob (NULL, Type, Length, &Hob);
  if (EFI_ERROR (Status)) {
    Hob = NULL;
  }
  //
  // Assume the process of HOB building is always successful.
  //
  ASSERT (Hob != NULL);
  return Hob;
}

/**

  Builds a Handoff Information Table HOB

  @param BootMode        - Current Bootmode
  @param MemoryBegin     - Start Memory Address.
  @param MemoryLength    - Length of Memory.

  @return EFI_SUCCESS Always success to initialize HOB.

**/
EFI_STATUS
EFIAPI
BuildHobHandoffInfoTable (
  IN EFI_BOOT_MODE         BootMode,
  IN EFI_PHYSICAL_ADDRESS  MemoryBegin,
  IN UINT64                MemoryLength
  )
{
  EFI_HOB_HANDOFF_INFO_TABLE   *Hob;
  EFI_HOB_GENERIC_HEADER       *HobEnd;

  Hob                      = (VOID *) (UINTN)MemoryBegin;
  HobEnd                   = (EFI_HOB_GENERIC_HEADER *) (Hob + 1);
  Hob->Header.HobType      = EFI_HOB_TYPE_HANDOFF;
  Hob->Header.HobLength    = (UINT16) sizeof (EFI_HOB_HANDOFF_INFO_TABLE);
  Hob->Header.Reserved     = 0;

  HobEnd->HobType          = EFI_HOB_TYPE_END_OF_HOB_LIST;
  HobEnd->HobLength        = (UINT16) sizeof (EFI_HOB_GENERIC_HEADER);
  HobEnd->Reserved         = 0;

  Hob->Version             = EFI_HOB_HANDOFF_TABLE_VERSION;
  Hob->BootMode            = BootMode;

  Hob->EfiMemoryTop        = MemoryBegin + MemoryLength;
  Hob->EfiMemoryBottom     = MemoryBegin;
  Hob->EfiFreeMemoryTop    = MemoryBegin + MemoryLength;
  Hob->EfiFreeMemoryBottom = (EFI_PHYSICAL_ADDRESS) (UINTN) (HobEnd + 1);
  Hob->EfiEndOfHobList     = (EFI_PHYSICAL_ADDRESS) (UINTN) HobEnd;

  SetHobList ((VOID *)Hob);

  return EFI_SUCCESS;
}


/**
  Builds a HOB that describes a chunk of system memory.

  This function builds a HOB that describes a chunk of system memory.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.

  If there is no additional space for HOB creation, then ASSERT().

  @param  ResourceType        The type of resource described by this HOB.
  @param  ResourceAttribute   The resource attributes of the memory described by this HOB.
  @param  PhysicalStart       The 64 bit physical address of memory described by this HOB.
  @param  NumberOfBytes       The length of the memory described by this HOB in bytes.

**/
VOID
EFIAPI
BuildResourceDescriptorHob (
  IN EFI_RESOURCE_TYPE            ResourceType,
  IN EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttribute,
  IN EFI_PHYSICAL_ADDRESS         PhysicalStart,
  IN UINT64                       NumberOfBytes
  )
{
  EFI_HOB_RESOURCE_DESCRIPTOR  *Hob;

  Hob = InternalPeiCreateHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, (UINT16) sizeof (EFI_HOB_RESOURCE_DESCRIPTOR));
  if (Hob == NULL) {
    return;
  }

  Hob->ResourceType      = ResourceType;
  Hob->ResourceAttribute = ResourceAttribute;
  Hob->PhysicalStart     = PhysicalStart;
  Hob->ResourceLength    = NumberOfBytes;
  ZeroMem (& (Hob->Owner), sizeof (EFI_GUID));
}

/**
  Builds a customized HOB tagged with a GUID for identification and returns
  the start address of GUID HOB data.

  This function builds a customized HOB tagged with a GUID for identification
  and returns the start address of GUID HOB data so that caller can fill the customized data.
  The HOB Header and Name field is already stripped.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.

  If Guid is NULL, then ASSERT().
  If there is no additional space for HOB creation, then ASSERT().
  If DataLength > (0xFFF8 - sizeof (EFI_HOB_GUID_TYPE)), then ASSERT().
  HobLength is UINT16 and multiples of 8 bytes, so the max HobLength is 0xFFF8.

  @param  Guid          The GUID to tag the customized HOB.
  @param  DataLength    The size of the data payload for the GUID HOB.

  @retval  NULL         The GUID HOB could not be allocated.
  @retval  others       The start address of GUID HOB data.

**/
VOID *
EFIAPI
BuildGuidHob (
  IN CONST EFI_GUID              *Guid,
  IN UINTN                       DataLength
  )
{
  EFI_HOB_GUID_TYPE *Hob;

  //
  // Make sure Guid is valid
  //
  ASSERT (Guid != NULL);

  //
  // Make sure that data length is not too long.
  //
  ASSERT (DataLength <= (0xFFF8 - sizeof (EFI_HOB_GUID_TYPE)));

  Hob = InternalPeiCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, (UINT16) (sizeof (EFI_HOB_GUID_TYPE) + DataLength));
  if (Hob == NULL) {
    return Hob;
  }
  CopyGuid (&Hob->Name, Guid);
  return Hob + 1;
}

/**
  Builds a customized HOB tagged with a GUID for identification, copies the input data to the HOB
  data field, and returns the start address of the GUID HOB data.

  This function builds a customized HOB tagged with a GUID for identification and copies the input
  data to the HOB data field and returns the start address of the GUID HOB data.  It can only be
  invoked during PEI phase; for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.
  The HOB Header and Name field is already stripped.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.

  If Guid is NULL, then ASSERT().
  If Data is NULL and DataLength > 0, then ASSERT().
  If there is no additional space for HOB creation, then ASSERT().
  If DataLength > (0xFFF8 - sizeof (EFI_HOB_GUID_TYPE)), then ASSERT().
  HobLength is UINT16 and multiples of 8 bytes, so the max HobLength is 0xFFF8.

  @param  Guid          The GUID to tag the customized HOB.
  @param  Data          The data to be copied into the data field of the GUID HOB.
  @param  DataLength    The size of the data payload for the GUID HOB.

  @retval  NULL         The GUID HOB could not be allocated.
  @retval  others       The start address of GUID HOB data.

**/
VOID *
EFIAPI
BuildGuidDataHob (
  IN CONST EFI_GUID              *Guid,
  IN VOID                        *Data,
  IN UINTN                       DataLength
  )
{
  VOID  *HobData;

  ASSERT (Data != NULL || DataLength == 0);

  HobData = BuildGuidHob (Guid, DataLength);
  if (HobData == NULL) {
    return HobData;
  }

  return CopyMem (HobData, Data, DataLength);
}

/**
  Builds a HOB that describes a chunk of system memory with Owner GUID.

  This function builds a HOB that describes a chunk of system memory.
  It can only be invoked during PEI phase;
  for DXE phase, it will ASSERT() since PEI HOB is read-only for DXE phase.

  If there is no additional space for HOB creation, then ASSERT().

  @param  ResourceType        The type of resource described by this HOB.
  @param  ResourceAttribute   The resource attributes of the memory described by this HOB.
  @param  PhysicalStart       The 64 bit physical address of memory described by this HOB.
  @param  NumberOfBytes       The length of the memory described by this HOB in bytes.
  @param  OwnerGUID           GUID for the owner of this resource.

**/
VOID
EFIAPI
BuildResourceDescriptorWithOwnerHob (
  IN EFI_RESOURCE_TYPE            ResourceType,
  IN EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttribute,
  IN EFI_PHYSICAL_ADDRESS         PhysicalStart,
  IN UINT64                       NumberOfBytes,
  IN EFI_GUID                     *OwnerGUID
  )
{
  EFI_HOB_RESOURCE_DESCRIPTOR  *Hob;

  Hob = InternalPeiCreateHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, (UINT16) sizeof (EFI_HOB_RESOURCE_DESCRIPTOR));
  if (Hob == NULL) {
    return;
  }

  Hob->ResourceType      = ResourceType;
  Hob->ResourceAttribute = ResourceAttribute;
  Hob->PhysicalStart     = PhysicalStart;
  Hob->ResourceLength    = NumberOfBytes;

  CopyGuid (&Hob->Owner, OwnerGUID);
}

/**
  Builds a HOB for the CPU.

  @param  SizeOfMemorySpace   The maximum physical memory addressability of the processor.
  @param  SizeOfIoSpace       The maximum physical I/O addressability of the processor.
**/
VOID
EFIAPI
BuildCpuHob (
  IN UINT8                       SizeOfMemorySpace,
  IN UINT8                       SizeOfIoSpace
  )
{
  EFI_HOB_CPU  *Hob;

  Hob = InternalPeiCreateHob (EFI_HOB_TYPE_CPU, sizeof (EFI_HOB_CPU));

  Hob->SizeOfMemorySpace = SizeOfMemorySpace;
  Hob->SizeOfIoSpace     = SizeOfIoSpace;

  //
  // Zero the reserved space to match HOB spec
  //
  ZeroMem (Hob->Reserved, sizeof (Hob->Reserved));
}


/**
  Builds a HOB for the memory allocation.

  @param  BaseAddress   The 64 bit physical address of the memory.
  @param  Length        The length of the memory allocation in bytes.
  @param  MemoryType    Type of memory allocated by this HOB.

**/
VOID
EFIAPI
BuildMemoryAllocationHob (
  IN EFI_PHYSICAL_ADDRESS        BaseAddress,
  IN UINT64                      Length,
  IN EFI_MEMORY_TYPE             MemoryType
  )
{
  EFI_HOB_MEMORY_ALLOCATION  *Hob;

  ASSERT (((BaseAddress & (EFI_PAGE_SIZE - 1)) == 0) && ((Length & (EFI_PAGE_SIZE - 1)) == 0));

  Hob = InternalPeiCreateHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, sizeof (EFI_HOB_MEMORY_ALLOCATION));

  ZeroMem (&(Hob->AllocDescriptor.Name), sizeof (EFI_GUID));
  Hob->AllocDescriptor.MemoryBaseAddress = BaseAddress;
  Hob->AllocDescriptor.MemoryLength      = Length;
  Hob->AllocDescriptor.MemoryType        = MemoryType;
  //
  // Zero the reserved space to match HOB spec
  //
  ZeroMem (Hob->AllocDescriptor.Reserved, sizeof (Hob->AllocDescriptor.Reserved));
}

