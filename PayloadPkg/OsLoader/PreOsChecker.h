/** @file

Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PRE_OS_CHECKER_H__
#define __PRE_OS_CHECKER_H__

/**
  To see whether PreOsChecker is loaded properly

  @retval           TRUE if successfully loaded, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsPreOsCheckerLoaded (
  IN  VOID
  );

/**
  Search for and load pre-OS checker for execution.

  This function will search for a pre-OS checker binary that might
  be loaded as part of the payload, if found get the entry point for
  execution later instead of jumping into the OS directly.

  @retval  EFI_LOAD_ERROR         Pre-OS checker is not loaded successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter is used at loading PreOsChecker image
  @retval  EFI_UNSUPPORTED        Pre-OS checker image is not supported

**/
EFI_STATUS
EFIAPI
LoadPreOsChecker (
  IN  VOID
  );

/**
  Start PreOsChecker with OS information

  @param[in] BootParam            Pointer of OS BOOT_PARAMS or IA32_BOOT_STATE
  @param[in] ImageFlags           Flags of the loaded image

  @retval EFI_NOT_FOUND           PreOsChecker not found
  @retval EFI_INVALID_PARAMETER   Invalid OS BOOT_PARAMS found

**/
EFI_STATUS
EFIAPI
StartPreOsChecker (
  IN  VOID   *OsBootParam,
  IN  UINT16  ImageFlags
  );

#endif /* __PRE_OS_CHECKER_H__ */
