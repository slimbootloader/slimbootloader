/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  IA32_DESCRIPTOR        Idtr;

  AsmReadIdtr (&Idtr);
  (* (UINTN *) (Idtr.Base - sizeof (UINTN))) = (UINTN)LoaderData;
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
  return (LOADER_GLOBAL_DATA *) (* (UINTN *) (Idtr.Base - sizeof (UINTN)));
}
