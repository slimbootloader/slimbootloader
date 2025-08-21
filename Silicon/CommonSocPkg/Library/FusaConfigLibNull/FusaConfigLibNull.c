/** @file
  This file contains the FuSa configuration support library template.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Uefi.h>
#include <Library/FusaConfigLib.h>

/**
  Perform any Pre-memory FuSa Configuration required, including updating FSP-M UPDs

  @param  FspmUpdPtr    The pointer to the FSP-M UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPreMem (
  VOID *FspmUpdPtr
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Perform any Post-memory FuSa Configuration required, including updating FSP-S UPDs

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPostMem (
  VOID *FspsUpdPtr
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Perform any Post-Silicon FuSa Configuration required.

  @retval EFI_STATUS
 */
EFI_STATUS
EFIAPI
FusaConfigPostSi (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Perform Fusa Boot Option Configuration required.

  @param OsBootOptionList The pointer to boot option list

  @retval EFI_STATUS
 */
EFI_STATUS
EFIAPI
FusaConfigBootOption (
  VOID *OsBootOptionList
  )
{
  return EFI_UNSUPPORTED;
}
