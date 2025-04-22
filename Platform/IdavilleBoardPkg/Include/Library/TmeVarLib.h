/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TME_VAR_LIB_H_
#define _TME_VAR_LIB_H_

#include <PlatformBase.h>

typedef struct {
  struct {
    UINT64 Key0;
    UINT64 Key1;
  }
  TmeKey[MAX_SOCKET];
} TME_KEY;

/**
  Get TME Keys From NVRAM

  @param[in, out]  TmeKeydata  Pointer to TME Key stored in TME variable

  @retval          EFI_SUCCESS if TME keys are successfully read from TME NVRAM variable
  @retval          EFI_UNSUPPORTED if TME is not enabled
  @retval          EFI_INVALID_PARAMETER if TmeKeyData pointer is NULL
**/
EFI_STATUS
EFIAPI
GetTmeVar (
  IN OUT TME_KEY  *TmeKeyData
  );

/**
  Get TME Keys from CPU and Store in NVRAM

  @retval     EFI_SUCCESS if TME keys are successfully stored in TME NVRAM variable
  @retval     EFI_UNSUPPORTED if TME is not enabled
  @retval     EFI_NOT_FOUND if TME Keys are not found in FSP HOB
**/
EFI_STATUS
EFIAPI
SetTmeVar (
  VOID
  );

/**
  Get TME Status

  @retval     TRUE if TME is enabled, FALSE otherwise
**/
BOOLEAN
EFIAPI
IsTmeEnabled (
  VOID
  );

#endif //_TME_VAR_LIB_H_
