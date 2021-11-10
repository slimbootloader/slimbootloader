/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TRUSTY_BOOT_LIB_H__
#define __TRUSTY_BOOT_LIB_H__

#include <Library/MultibootLib.h>

/**
  Setup the dependencies for performing Trusty boot.

  @param[in,out]  TrustyImage     The trusty image pointer.
  @param[in,out]  BootOsImage     The normal OS image pointer.

  @retval  RETURN_SUCCESS         Trusty boot setup was completed.
  @retval  Others                 Encountered an error setting up for Trusty boot.
**/
EFI_STATUS
EFIAPI
SetupTrustyBoot (
  IN OUT  MULTIBOOT_IMAGE    *TrustyImage,
  IN OUT  MULTIBOOT_IMAGE    *BootOsImage
  );

#endif
