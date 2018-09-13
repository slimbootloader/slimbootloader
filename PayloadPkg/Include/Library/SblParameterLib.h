/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are
  licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Guid/OsBootOptionGuid.h>

#ifndef __SBL_PARAMETER_LIB_H__
#define __SBL_PARAMETER_LIB_H__

/**
  Add Sbl specific command line parameters

  Some Os requires SBL specific command line parameters.

  @param[in]     BootOption        Current boot option
  @param[out]    CommandLine       Command line for adding new parameters
  @param[in,out] CommandLineSize   The max size of buffer CommandLine when input.
                                   and the actual Command line string size, including NULL
                                   terminator when return.

**/
EFI_STATUS
EFIAPI
AddSblCommandLine (
  IN     OS_BOOT_OPTION     *BootOption,
  OUT    CHAR8              *CommandLine,
  IN OUT UINT32             *CommandLineSize
  );

#endif
