/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <BootloaderCoreGlobal.h>

/**
  This function sets the Loader global data pointer.

  @param[in] LoaderData       Loader data pointer.

**/
VOID
EFIAPI
SetLoaderGlobalDataPointer (
  IN LOADER_GLOBAL_DATA   *LoaderData
  )
{
  IA32_DESCRIPTOR        Idtr;

  AsmReadIdtr (&Idtr);
  (* (UINTN *) (Idtr.Base - sizeof (UINT64))) = (UINTN)LoaderData;
}

/**
  This function gets the Loader global data pointer.

**/
LOADER_GLOBAL_DATA *
EFIAPI
GetLoaderGlobalDataPointer (
  VOID
  )
{
  IA32_DESCRIPTOR     Idtr;

  AsmReadIdtr (&Idtr);
  return (LOADER_GLOBAL_DATA *) (* (UINTN *) (Idtr.Base - sizeof (UINT64)));
}

/**
  Returns the pointer to the Fsp HOB list.

  If the pointer to the Fsp HOB list is NULL, then ASSERT().

  @return The pointer to the Fsp HOB list.

**/
VOID *
EFIAPI
GetFspHobListPtr (
  VOID
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  if (LdrGlobal == NULL) {
    return NULL;
  }
  return LdrGlobal->FspHobList;
}
