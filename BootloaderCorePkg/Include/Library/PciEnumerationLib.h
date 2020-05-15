/** @file

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_ENUMERTION_LIB_H__
#define __PCI_ENUMERTION_LIB_H__

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
