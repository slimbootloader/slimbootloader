/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _IO_MMU_LIB_H_
#define _IO_MMU_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>

//
// IOMMU Access for SetAttribute
//
// These types can be "ORed" together as needed.
// Any undefined bits are reserved and must be zero.
//
#define EDKII_IOMMU_ACCESS_READ   0x1
#define EDKII_IOMMU_ACCESS_WRITE  0x2

//
// IOMMU Operation for Map
//
typedef enum {
  ///
  /// A read operation from system memory by a bus master that is not capable of producing
  /// PCI dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterRead,
  ///
  /// A write operation from system memory by a bus master that is not capable of producing
  /// PCI dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterWrite,
  ///
  /// Provides both read and write access to system memory by both the processor and a bus
  /// master that is not capable of producing PCI dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterCommonBuffer,
  ///
  /// A read operation from system memory by a bus master that is capable of producing PCI
  /// dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterRead64,
  ///
  /// A write operation to system memory by a bus master that is capable of producing PCI
  /// dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterWrite64,
  ///
  /// Provides both read and write access to system memory by both the processor and a bus
  /// master that is capable of producing PCI dual address cycles.
  ///
  EdkiiIoMmuOperationBusMasterCommonBuffer64,
  EdkiiIoMmuOperationMaximum
} EDKII_IOMMU_OPERATION;


#define MAP_INFO_SIGNATURE  SIGNATURE_32 ('D', 'M', 'A', 'P')
typedef struct {
  UINT32                            Signature;
  EDKII_IOMMU_OPERATION             Operation;
  UINTN                             NumberOfBytes;
  EFI_PHYSICAL_ADDRESS              HostAddress;
  EFI_PHYSICAL_ADDRESS              DeviceAddress;
} MAP_INFO;

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
  IN  VOID                  *HostAddress,
  IN  OUT UINTN             *NumberOfBytes,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  );

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
  );

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
  );

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
  );

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
  );

#endif

