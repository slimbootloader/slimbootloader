/** @file

  Copyright (c) 2012, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MULTIBOOT_LIB_INTERNAL_H_
#define _MULTIBOOT_LIB_INTERNAL_H_

extern UINT8 mLoaderName[];

/**
  Align multiboot modules if required by spec.

  @param[in,out] MultiBoot   Point to loaded Multiboot image structure

  @retval  RETURN_SUCCESS     Align modules successfully
  @retval  Others             There is error when align image
**/
EFI_STATUS
EFIAPI
AlignMultibootModules (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  );

#endif  //_MULTIBOOT_LIB_INTERNAL_H
