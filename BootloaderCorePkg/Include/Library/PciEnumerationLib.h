/** @file

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_ENUMERTION_LIB_H__
#define __PCI_ENUMERTION_LIB_H__

typedef enum {
  ///
  /// This notification is only applicable to PCI-PCI bridges and
  /// indicates that the PCI enumerator is about to begin enumerating
  /// the bus behind the PCI-PCI Bridge. This notification is sent after
  /// the primary bus number, the secondary bus number and the subordinate
  /// bus number registers in the PCI-PCI Bridge are programmed to valid
  /// (not necessary final) values
  ///
  EfiPciBeforeChildBusEnumeration,

  ///
  /// This notification is sent before the PCI enumerator probes BAR registers
  /// for every valid PCI function.
  ///
  EfiPciBeforeResourceCollection
} EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE;

typedef VOID   (EFIAPI *PLATFORM_PCI_ENUM_HOOK_PROC) (UINT8 Bus, UINT8 Dev, UINT8 Fun, EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase);

/**
 Enumerates the PCI devices allocates the required memory resource.
 Program the allocated memory resource to PCI BAR.

 @param [in] MemPool point to memory pool to allocate for each PCI device.
 **/
EFI_STATUS
EFIAPI
PciEnumeration (
  IN  VOID   *MemPool
  );

#endif
