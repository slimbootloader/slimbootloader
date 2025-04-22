/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/IoMmuLib.h>

#define  mIoMmu            NULL
#define  EDKII_IOMMU_PPI   VOID

/**

  Memory Layout:

  =========== +------------------+ <=============== PHMR.Limit (TOUM)
              |   FSP Reserved   |
              +------------------+
              |   SBL Reserved   |
  DMA Protect +------------------+
              |   ACPI Reserved  |
              +------------------+
              |   PLD Reserved   |
  =========== +==================+ <=============== PHMR.Base
              |                  |
  DMA    Free |   DMA Buffer     |  PcdDmaBufferSize / PcdDmaBufferAlignment
              |                  |
  =========== +==================+ <=============== PLMR.Limit
              |   Payload Heap   |
              +------------------+
  DMA Protect |   Payload Stack  |
              +------------------+
              |   Free   Memory  |
  =========== +------------------+ <=============== PLMR.Base (0)
**/

/**
  Set IOMMU attribute for a system memory.

  If the IOMMU PPI exists, the system memory cannot be used
  for DMA by default.

  When a device requests a DMA access for a system memory,
  the device driver need use SetAttribute() to update the IOMMU
  attribute to request DMA access (read and/or write).

  @param[in]  This              The PPI instance pointer.
  @param[in]  Mapping           The mapping value returned from Map().
  @param[in]  IoMmuAccess       The IOMMU access.

  @retval EFI_SUCCESS            The IoMmuAccess is set for the memory range specified by DeviceAddress and Length.
  @retval EFI_INVALID_PARAMETER  Mapping is not a value that was returned by Map().
  @retval EFI_INVALID_PARAMETER  IoMmuAccess specified an illegal combination of access.
  @retval EFI_UNSUPPORTED        The bit mask of IoMmuAccess is not supported by the IOMMU.
  @retval EFI_UNSUPPORTED        The IOMMU does not support the memory range specified by Mapping.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to modify the IOMMU access.
  @retval EFI_DEVICE_ERROR       The IOMMU device reported an error while attempting the operation.
  @retval EFI_NOT_AVAILABLE_YET  DMA protection has been enabled, but DMA buffer are
                                 not available to be allocated yet.

**/
STATIC
EFI_STATUS
EFIAPI
BlIoMmuSetAttribute (
  IN EDKII_IOMMU_PPI       *This,
  IN VOID                  *Mapping,
  IN UINT64                IoMmuAccess
  )
{
  return EFI_SUCCESS;
}

/**
  Provides the controller-specific addresses required to access system memory from a
  DMA bus master.

  @param  This                  The PPI instance pointer.
  @param  Operation             Indicates if the bus master is going to read or write to system memory.
  @param  HostAddress           The system memory address to map to the PCI controller.
  @param  NumberOfBytes         On input the number of bytes to map. On output the number of bytes
                                that were mapped.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The range was mapped for the returned NumberOfBytes.
  @retval EFI_UNSUPPORTED       The HostAddress cannot be mapped as a common buffer.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_DEVICE_ERROR      The system hardware could not map the requested address.
  @retval EFI_NOT_AVAILABLE_YET DMA protection has been enabled, but DMA buffer are
                                not available to be allocated yet.

**/
STATIC
EFI_STATUS
EFIAPI
BlIoMmuMap (
  IN     EDKII_IOMMU_PPI                            *This,
  IN     EDKII_IOMMU_OPERATION                      Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  )
{
  MAP_INFO                    *MapInfo;
  UINTN                       Length;

  if (Operation == EdkiiIoMmuOperationBusMasterCommonBuffer ||
      Operation == EdkiiIoMmuOperationBusMasterCommonBuffer64) {
    *DeviceAddress = (UINTN)HostAddress;
    *Mapping = NULL;
    return EFI_SUCCESS;
  }

  Length = *NumberOfBytes + sizeof(MAP_INFO);
  *DeviceAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocateRuntimePages (EFI_SIZE_TO_PAGES(Length));

  if (*DeviceAddress == 0) {
    DEBUG ((DEBUG_ERROR, "IoMmuMap - OUT_OF_RESOURCE\n"));
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }


  MapInfo = (VOID *)(UINTN)(*DeviceAddress + *NumberOfBytes);
  MapInfo->Signature     = MAP_INFO_SIGNATURE;
  MapInfo->Operation     = Operation;
  MapInfo->NumberOfBytes = *NumberOfBytes;
  MapInfo->HostAddress   = (UINTN)HostAddress;
  MapInfo->DeviceAddress = *DeviceAddress;
  *Mapping = MapInfo;
  DEBUG ((DEBUG_VERBOSE, "  Op(%x):DeviceAddress - %x, Mapping - %x\n", Operation, (UINTN)*DeviceAddress, MapInfo));

  //
  // If this is a read operation from the Bus Master's point of view,
  // then copy the contents of the real buffer into the mapped buffer
  // so the Bus Master can read the contents of the real buffer.
  //
  if (Operation == EdkiiIoMmuOperationBusMasterRead ||
      Operation == EdkiiIoMmuOperationBusMasterRead64) {
    CopyMem (
      (VOID *) (UINTN) MapInfo->DeviceAddress,
      (VOID *) (UINTN) MapInfo->HostAddress,
      MapInfo->NumberOfBytes
      );
  }

  return EFI_SUCCESS;
}

/**
  Completes the Map() operation and releases any corresponding resources.

  @param  This                  The PPI instance pointer.
  @param  Mapping               The mapping value returned from Map().

  @retval EFI_SUCCESS           The range was unmapped.
  @retval EFI_INVALID_PARAMETER Mapping is not a value that was returned by Map().
  @retval EFI_DEVICE_ERROR      The data was not committed to the target system memory.
  @retval EFI_NOT_AVAILABLE_YET DMA protection has been enabled, but DMA buffer are
                                not available to be allocated yet.

**/
STATIC
EFI_STATUS
EFIAPI
BlIoMmuUnmap (
  IN  EDKII_IOMMU_PPI                          *This,
  IN  VOID                                     *Mapping
  )
{
  MAP_INFO                    *MapInfo;
  UINTN                       Length;

  if (Mapping == NULL) {
    return EFI_SUCCESS;
  }

  MapInfo = Mapping;
  ASSERT (MapInfo->Signature == MAP_INFO_SIGNATURE);
  DEBUG ((DEBUG_VERBOSE, "  Op(%x):DeviceAddress - %x, NumberOfBytes - %x\n", MapInfo->Operation, (UINTN)MapInfo->DeviceAddress, MapInfo->NumberOfBytes));

  //
  // If this is a write operation from the Bus Master's point of view,
  // then copy the contents of the mapped buffer into the real buffer
  // so the processor can read the contents of the real buffer.
  //
  if (MapInfo->Operation == EdkiiIoMmuOperationBusMasterWrite ||
      MapInfo->Operation == EdkiiIoMmuOperationBusMasterWrite64) {
    CopyMem (
      (VOID *) (UINTN) MapInfo->HostAddress,
      (VOID *) (UINTN) MapInfo->DeviceAddress,
      MapInfo->NumberOfBytes
      );
  }

  Length = MapInfo->NumberOfBytes + sizeof(MAP_INFO);
  FreePages ((VOID *)(UINTN)MapInfo->DeviceAddress, EFI_SIZE_TO_PAGES(Length));

  return EFI_SUCCESS;
}

/**
  Allocates pages that are suitable for an OperationBusMasterCommonBuffer or
  OperationBusMasterCommonBuffer64 mapping.

  @param  This                  The PPI instance pointer.
  @param  MemoryType            The type of memory to allocate, EfiBootServicesData or
                                EfiRuntimeServicesData.
  @param  Pages                 The number of pages to allocate.
  @param  HostAddress           A pointer to store the base system memory address of the
                                allocated range.
  @param  Attributes            The requested bit mask of attributes for the allocated range.

  @retval EFI_SUCCESS           The requested memory pages were allocated.
  @retval EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
                                MEMORY_WRITE_COMBINE, MEMORY_CACHED and DUAL_ADDRESS_CYCLE.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.
  @retval EFI_NOT_AVAILABLE_YET DMA protection has been enabled, but DMA buffer are
                                not available to be allocated yet.

**/
STATIC
EFI_STATUS
EFIAPI
BlIoMmuAllocateBuffer (
  IN     EDKII_IOMMU_PPI                          *This,
  IN     EFI_MEMORY_TYPE                          MemoryType,
  IN     UINTN                                    Pages,
  IN OUT VOID                                     **HostAddress,
  IN     UINT64                                   Attributes
  )
{
  *HostAddress = AllocateRuntimePages (Pages);

  if (*HostAddress == NULL) {
    DEBUG ((DEBUG_ERROR, "IoMmuAllocateBuffer - OUT_OF_RESOURCE\n"));
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Frees memory that was allocated with AllocateBuffer().

  @param  This                  The PPI instance pointer.
  @param  Pages                 The number of pages to free.
  @param  HostAddress           The base system memory address of the allocated range.

  @retval EFI_SUCCESS           The requested memory pages were freed.
  @retval EFI_INVALID_PARAMETER The memory range specified by HostAddress and Pages
                                was not allocated with AllocateBuffer().
  @retval EFI_NOT_AVAILABLE_YET DMA protection has been enabled, but DMA buffer are
                                not available to be allocated yet.

**/
STATIC
EFI_STATUS
EFIAPI
BlIoMmuFreeBuffer (
  IN  EDKII_IOMMU_PPI                          *This,
  IN  UINTN                                    Pages,
  IN  VOID                                     *HostAddress
  )
{
  FreePages (HostAddress, Pages);
  return EFI_SUCCESS;
}

/**
  Provides the controller-specific addresses required to access system memory from a
  DMA bus master.

  @param  Operation             Indicates if the bus master is going to read or write to system memory.
  @param  HostAddress           The system memory address to map to the PCI controller.
  @param  NumberOfBytes         On input the number of bytes to map. On output the number of bytes
                                that were mapped.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The range was mapped for the returned NumberOfBytes.
  @retval EFI_UNSUPPORTED       The HostAddress cannot be mapped as a common buffer.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_DEVICE_ERROR      The system hardware could not map the requested address.

**/
EFI_STATUS
IoMmuMap (
  IN  EDKII_IOMMU_OPERATION Operation,
  IN VOID                   *HostAddress,
  IN  OUT UINTN             *NumberOfBytes,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  )
{
  EFI_STATUS  Status;
  UINT64      Attribute;

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    Status = BlIoMmuMap (
                       mIoMmu,
                       Operation,
                       HostAddress,
                       NumberOfBytes,
                       DeviceAddress,
                       Mapping
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    switch (Operation) {
    case EdkiiIoMmuOperationBusMasterRead:
    case EdkiiIoMmuOperationBusMasterRead64:
      Attribute = EDKII_IOMMU_ACCESS_READ;
      break;
    case EdkiiIoMmuOperationBusMasterWrite:
    case EdkiiIoMmuOperationBusMasterWrite64:
      Attribute = EDKII_IOMMU_ACCESS_WRITE;
      break;
    case EdkiiIoMmuOperationBusMasterCommonBuffer:
    case EdkiiIoMmuOperationBusMasterCommonBuffer64:
      Attribute = EDKII_IOMMU_ACCESS_READ | EDKII_IOMMU_ACCESS_WRITE;
      break;
    default:
      ASSERT(FALSE);
      return EFI_INVALID_PARAMETER;
    }
    Status = BlIoMmuSetAttribute (
                       mIoMmu,
                       *Mapping,
                       Attribute
                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    *DeviceAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) HostAddress;
    *Mapping = NULL;
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**
  Completes the Map() operation and releases any corresponding resources.

  @param  Mapping               The mapping value returned from Map().

  @retval EFI_SUCCESS           The range was unmapped.
  @retval EFI_INVALID_PARAMETER Mapping is not a value that was returned by Map().
  @retval EFI_DEVICE_ERROR      The data was not committed to the target system memory.
**/
EFI_STATUS
IoMmuUnmap (
  IN VOID                  *Mapping
  )
{
  EFI_STATUS  Status;

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    Status = BlIoMmuSetAttribute (mIoMmu, Mapping, 0);
    Status = BlIoMmuUnmap (mIoMmu, Mapping);
  } else {
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**
  Allocates pages that are suitable for an OperationBusMasterCommonBuffer or
  OperationBusMasterCommonBuffer64 mapping.

  @param  Pages                 The number of pages to allocate.
  @param  HostAddress           A pointer to store the base system memory address of the
                                allocated range.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The requested memory pages were allocated.
  @retval EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
                                MEMORY_WRITE_COMBINE and MEMORY_CACHED.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.

**/
EFI_STATUS
IoMmuAllocateBuffer (
  IN UINTN                  Pages,
  OUT VOID                  **HostAddress,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  )
{
  EFI_STATUS            Status;
  UINTN                 NumberOfBytes;
  EFI_PHYSICAL_ADDRESS  HostPhyAddress;

  *HostAddress = NULL;
  *DeviceAddress = 0;

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    Status = BlIoMmuAllocateBuffer (
                       mIoMmu,
                       EfiBootServicesData,
                       Pages,
                       HostAddress,
                       0
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    NumberOfBytes = EFI_PAGES_TO_SIZE(Pages);
    Status = BlIoMmuMap (
                       mIoMmu,
                       EdkiiIoMmuOperationBusMasterCommonBuffer,
                       *HostAddress,
                       &NumberOfBytes,
                       DeviceAddress,
                       Mapping
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = BlIoMmuSetAttribute (
                       mIoMmu,
                       *Mapping,
                       EDKII_IOMMU_ACCESS_READ | EDKII_IOMMU_ACCESS_WRITE
                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    HostPhyAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocatePages (Pages);
    if (HostPhyAddress == 0) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = EFI_SUCCESS;
    *HostAddress = (VOID *)(UINTN)HostPhyAddress;
    *DeviceAddress = HostPhyAddress;
    *Mapping = NULL;
  }
  return Status;
}

/**
  Frees memory that was allocated with AllocateBuffer().

  @param  Pages                 The number of pages to free.
  @param  HostAddress           The base system memory address of the allocated range.
  @param  Mapping               The mapping value returned from Map().

  @retval EFI_SUCCESS           The requested memory pages were freed.
  @retval EFI_INVALID_PARAMETER The memory range specified by HostAddress and Pages
                                was not allocated with AllocateBuffer().

**/
EFI_STATUS
IoMmuFreeBuffer (
  IN UINTN                  Pages,
  IN VOID                   *HostAddress,
  IN VOID                   *Mapping
  )
{
  EFI_STATUS  Status;

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    Status = BlIoMmuSetAttribute (mIoMmu, Mapping, 0);
    Status = BlIoMmuUnmap (mIoMmu, Mapping);
    Status = BlIoMmuFreeBuffer (mIoMmu, Pages, HostAddress);
  } else {
    FreePages (HostAddress, Pages);
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**
  Allocates aligned pages that are suitable for an OperationBusMasterCommonBuffer or
  OperationBusMasterCommonBuffer64 mapping.

  @param  Pages                 The number of pages to allocate.
  @param  Alignment             The requested alignment of the allocation.  Must be a power of two.
  @param  HostAddress           A pointer to store the base system memory address of the
                                allocated range.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The requested memory pages were allocated.
  @retval EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
                                MEMORY_WRITE_COMBINE and MEMORY_CACHED.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.

**/
EFI_STATUS
IoMmuAllocateAlignedBuffer (
  IN UINTN                  Pages,
  IN UINTN                  Alignment,
  OUT VOID                  **HostAddress,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  )
{
  EFI_STATUS            Status;
  VOID                  *Memory;
  UINTN                 AlignedMemory;
  UINTN                 AlignmentMask;
  UINTN                 UnalignedPages;
  UINTN                 RealPages;
  UINTN                 NumberOfBytes;
  EFI_PHYSICAL_ADDRESS  HostPhyAddress;

  *HostAddress   = NULL;
  *DeviceAddress = 0;
  AlignmentMask  = Alignment - 1;

  //
  // Calculate the total number of pages since alignment is larger than page size.
  //
  RealPages = Pages + EFI_SIZE_TO_PAGES (Alignment);

  //
  // Make sure that Pages plus EFI_SIZE_TO_PAGES (Alignment) does not overflow.
  //
  ASSERT (RealPages > Pages);

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    Status = BlIoMmuAllocateBuffer (
                       mIoMmu,
                       EfiBootServicesData,
                       RealPages,
                       HostAddress,
                       0
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Memory         = *HostAddress;
    AlignedMemory  = ((UINTN) Memory + AlignmentMask) & ~AlignmentMask;
    UnalignedPages = EFI_SIZE_TO_PAGES (AlignedMemory - (UINTN) Memory);
    if (UnalignedPages > 0) {
      //
      // Free first unaligned page(s).
      //
      Status = BlIoMmuFreeBuffer (
                         mIoMmu,
                         UnalignedPages,
                         Memory);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    Memory         = (VOID *)(UINTN)(AlignedMemory + EFI_PAGES_TO_SIZE (Pages));
    UnalignedPages = RealPages - Pages - UnalignedPages;
    if (UnalignedPages > 0) {
      //
      // Free last unaligned page(s).
      //
      Status = BlIoMmuFreeBuffer (
                         mIoMmu,
                         UnalignedPages,
                         Memory);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    *HostAddress  = (VOID *) AlignedMemory;
    NumberOfBytes = EFI_PAGES_TO_SIZE(Pages);
    Status = BlIoMmuMap (
                       mIoMmu,
                       EdkiiIoMmuOperationBusMasterCommonBuffer,
                       *HostAddress,
                       &NumberOfBytes,
                       DeviceAddress,
                       Mapping
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = BlIoMmuSetAttribute (
                       mIoMmu,
                       *Mapping,
                       EDKII_IOMMU_ACCESS_READ | EDKII_IOMMU_ACCESS_WRITE
                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    HostPhyAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocatePages (RealPages);
    if (HostPhyAddress == 0) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = EFI_SUCCESS;
    *HostAddress = (VOID *)(((UINTN) HostPhyAddress + AlignmentMask) & ~AlignmentMask);
    *DeviceAddress = ((UINTN) HostPhyAddress + AlignmentMask) & ~AlignmentMask;
    *Mapping = NULL;

  }
  return Status;
}

