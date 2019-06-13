/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
