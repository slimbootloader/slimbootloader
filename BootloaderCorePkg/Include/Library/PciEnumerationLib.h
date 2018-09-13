/** @file

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
