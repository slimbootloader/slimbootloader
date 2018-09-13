/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
