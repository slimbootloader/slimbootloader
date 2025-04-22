/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STAGE1A_BOARD_INIT_LIB_H__
#define __STAGE1A_BOARD_INIT_LIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BoardInitLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/IoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/GpioLib.h>
#include <Library/StageCommonBoardInitLib.h>
#include <Register/PchRegsLpc.h>
#include <GpioConfig.h>
#include <GpioPinsCdf.h>
#include <PlatformData.h>
#include <FsptUpd.h>
#include <Register/CpuRegs.h>

/**
  Initialize Platform SIO chip.

**/
VOID
EFIAPI
PlatformSioEarlyInit (
  VOID
);

#endif // __STAGE1A_BOARD_INIT_LIB_H__
