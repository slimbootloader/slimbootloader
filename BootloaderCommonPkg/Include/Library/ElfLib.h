/** @file
  ELF library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ELF_LIB_H__
#define __ELF_LIB_H__

#include <Uefi/UefiBaseType.h>

/**
  Check if the image is a bootable ELF image.

  @param[in]  ImageBase      Memory address of an image

  @retval     TRUE           Image is a bootable ELF image
  @retval     FALSE          Not a bootable ELF image
**/
BOOLEAN
EFIAPI
IsElfImage (
  IN  CONST VOID             *ImageBase
  );

/**
  Load the ELF image to specified address in ELF header.

  This function loads ELF image segments into memory address specified
  in ELF program header.

  @param[in]  ImageBase           Memory address of an image.
  @param[out] EntryPoint          The entry point of loaded ELF image.

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_UNSUPPORTED         Unsupported binary type.
  @retval EFI_LOAD_ERROR          ELF binary loading error.
  @retval EFI_SUCCESS             ELF binary is loaded successfully.
**/
EFI_STATUS
LoadElfImage (
  IN  CONST VOID                  *ImageBase,
  OUT       VOID                 **EntryPoint
  );

#endif /* __ELF_LIB_H__ */
