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

/**
  Check if a memory range is valid (present in memory map as usable/allocated).

  @param[in]  Start    Start address of the range
  @param[in]  Length   Length of the range

  @retval TRUE         Range is valid
  @retval FALSE        Range is invalid
**/
BOOLEAN
EFIAPI
IsMemoryRangeValid (
  IN  UINT64               Start,
  IN  UINT64               Length
  );

#endif  //_MULTIBOOT_LIB_INTERNAL_H
