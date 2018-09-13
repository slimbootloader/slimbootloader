/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <FspApiLibInternal.h>
#include <Library/LitePeCoffLib.h>

/**
  This FSP API is used to notify the FSP about the different phases in the boot process.
  This allows the FSP to take appropriate actions as needed during different initialization
  phases. The phases will be platform dependent and will be documented with the FSP
  release. The current FSP supports following notify phases:
    Post PCI enumeration
    Ready To Boot
    End of firmware

  @param[in] Delta              The delta between the new and old base.
                                Delta = NewBase - OldBase

  @retval EFI_SUCCESS           The rebasing was handled successfully.

**/
EFI_STATUS
EFIAPI
RebaseFspComponent (
  UINT32   Delta
  )
{
  FSP_INFO_HEADER   *FspHeader;

  FspHeader = (FSP_INFO_HEADER *) (PcdGet32 (PcdFSPSBase) + Delta + FSP_INFO_HEADER_OFF);
  FspHeader->ImageBase += Delta;

  return EFI_SUCCESS;
}
