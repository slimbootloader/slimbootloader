/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>


typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
} PLATFORM_DATA;

#endif /* __PLATFORM_DATA_H__ */
