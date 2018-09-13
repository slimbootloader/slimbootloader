/** @file
  System reset Library Services.  This library class defines a set of
  methods that reset the whole system.

Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __RESET_SYSTEM_LIB_H__
#define __RESET_SYSTEM_LIB_H__
#include <PiPei.h>

/**
  Resets the entire platform.

  @param[in] ResetType          The type of reset to perform.

**/
VOID
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  );

#endif
