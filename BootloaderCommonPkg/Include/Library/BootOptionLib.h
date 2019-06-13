/** @file
Header file for boot option library

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOT_OPTION_LIB_H_
#define _BOOT_OPTION_LIB_H_

/**
  Get boot option list

  This function will get OS boot option from HOB.

  @retval HOB data         Return HOB data if it is found.
  @retval NULL             Return NULL if it is NOT found.
**/
OS_BOOT_OPTION_LIST *
EFIAPI
GetBootOptionList (
  VOID
  );


/**
  Get file system string by file type

  @param[in]  FsType       File system type

  @retval String           Return a string when file systme is found.
  @retval NULL             Return NULL if file system is not found.
**/
CHAR8 *
EFIAPI
GetFsTypeString (
  IN OS_FILE_SYSTEM_TYPE   FsType
  );

/**
  Get boot device name string by boot device type

  @param[in]  DevType      Boot device type

  @retval String           Return a string when boot device type is found.
  @retval NULL             Return NULL if boot device type is not found.
**/
CHAR8 *
EFIAPI
GetBootDeviceNameString (
  OS_BOOT_MEDIUM_TYPE      DevType
  );

#endif
