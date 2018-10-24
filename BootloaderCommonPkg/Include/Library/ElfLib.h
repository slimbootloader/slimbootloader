/** @file
  ELF library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  IN  VOID                   *ImageBase
  );

/**
  Load the ELF image to specified address in ELF header.

  This function load ELF image section by section into memory address specified
  in ELF program header, and also load ELF symbols.

  @param[in]  ImageBase      Memory address of an image

  @retval Image entry point  The entry point of ELF image if load image success
  @retval NULL               Error with loading ELF image
**/
UINT32 *
LoadElfImage (
  IN  VOID                   *ImageBase
  );

#endif /* __ELF_LIB_H__ */
