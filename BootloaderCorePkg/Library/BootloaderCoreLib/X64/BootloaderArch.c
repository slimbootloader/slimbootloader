/** @file

  Copyright (c) 2016 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>

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
  return NULL;
}
