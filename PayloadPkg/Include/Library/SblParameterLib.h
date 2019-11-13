/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/IasImageLib.h>

#ifndef __SBL_PARAMETER_LIB_H__
#define __SBL_PARAMETER_LIB_H__

typedef struct {
  IMAGE_DATA            BootDevicesData;    // allocated for bootdevices cmdline
  IMAGE_DATA            OsPerfData;         // allocated for timestamps cmdline
  IMAGE_DATA            OsCrashMemoryData;  // allocated for ramoops.mem_address cmdline
} RESERVED_CMDLINE_DATA;

/**
  Add Sbl specific command line parameters

  Some Os requires SBL specific command line parameters.

  @param[in]     BootOption           Current boot option
  @param[out]    CommandLine          Command line for adding new parameters
  @param[in,out] CommandLineSize      The max size of buffer CommandLine when input.
                                      and the actual Command line string size, including NULL
                                      terminator when return.
  @param[in,out] ReservedCmdlineData  Reserved memory info about kernel command line for OS

**/
EFI_STATUS
EFIAPI
AddSblCommandLine (
  IN     OS_BOOT_OPTION         *BootOption,
  OUT    CHAR8                  *CommandLine,
  IN OUT UINT32                 *CommandLineSize,
  IN OUT RESERVED_CMDLINE_DATA  *ReservedCmdlineData
  );

#endif
