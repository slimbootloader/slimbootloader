/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __FUSA_CONFIG_LIB_H__
#define __FUSA_CONFIG_LIB_H__

/**
  Perform any Pre-memory FuSa Configuration required, including updating FSP-M UPDs

  @param  FspmUpdPtr    The pointer to the FSP-M UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPreMem (
  VOID *FspmUpdPtr
  );

/**
  Perform any Post-memory FuSa Configuration required, including updating FSP-S UPDs

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPostMem (
  VOID *FspsUpdPtr
  );

/**
  Perform any Post-Silicon FuSa Configuration required.

  @retval EFI_STATUS
 */
EFI_STATUS
EFIAPI
FusaConfigPostSi (
  VOID
  );

/**
  Perform Fusa Boot Option Configuration required.

  @param OsBootOptionList The pointer to boot option list

  @retval EFI_STATUS
 */
EFI_STATUS
EFIAPI
FusaConfigBootOption (
  VOID *OsBootOptionList
  );

#endif
